#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESFrameBuffer.hpp>

namespace ag
{
static const AG_uint s_max_framebuffer_size = 4096;

OpenGL_ESFrameBuffer::OpenGL_ESFrameBuffer(const FrameBufferSpecification& spec)
  : m_specification(spec)
{
  for (auto attachment_spec : m_specification.attachments.attachments)
  {
    if (attachment_spec.texture_format != FrameBuffer_Texture_Format::DEPTH24STENCIL8)
      m_color_attachment_specifications.emplace_back(attachment_spec);
    else
      m_depth_attachment_specifications = attachment_spec;
  }
  invalidate();
}

OpenGL_ESFrameBuffer::~OpenGL_ESFrameBuffer()
{
  cleanup();
}

void OpenGL_ESFrameBuffer::cleanup()
{
  if (!m_ID)
    return;
  glDeleteFramebuffers(1, &m_ID);
  m_ID = 0;
  if (!m_colorattachment.empty())
  {
    glDeleteTextures((GLsizei)m_colorattachment.size(), m_colorattachment.data());
    m_colorattachment.clear();
  }
  if (m_depthattachment)
  {
    glDeleteRenderbuffers(1, &m_depthattachment);  // renderbuffer, not texture
    m_depthattachment = 0;
  }
}

void OpenGL_ESFrameBuffer::invalidate()
{
  cleanup();

  glGenFramebuffers(1, &m_ID);
  glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

  // ── Color attachments ──────────────────────────────────────────────────────
  if (!m_color_attachment_specifications.empty())
  {
    m_colorattachment.resize(m_color_attachment_specifications.size());
    glGenTextures((GLsizei)m_colorattachment.size(), m_colorattachment.data());

    for (size_t i = 0; i < m_colorattachment.size(); i++)
    {
      glBindTexture(GL_TEXTURE_2D, m_colorattachment[i]);

      auto fmt = m_color_attachment_specifications[i].texture_format;
      if (fmt == FrameBuffer_Texture_Format::RGBA8)
      {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA8,
                     (GLsizei)m_specification.size.x,
                     (GLsizei)m_specification.size.y,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     nullptr);
      }
      else if (fmt == FrameBuffer_Texture_Format::RED_INTEGER)
      {
        // R32I requires GL_INT pixel type, not GL_UNSIGNED_BYTE
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_R32I,
                     (GLsizei)m_specification.size.x,
                     (GLsizei)m_specification.size.y,
                     0,
                     GL_RED_INTEGER,
                     GL_INT,
                     nullptr);
      }

      // Integer textures MUST use GL_NEAREST — GL_LINEAR is invalid
      GLenum filter = (fmt == FrameBuffer_Texture_Format::RED_INTEGER) ? GL_NEAREST : GL_LINEAR;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glFramebufferTexture2D(GL_FRAMEBUFFER,
                             GL_COLOR_ATTACHMENT0 + (GLenum)i,
                             GL_TEXTURE_2D,
                             m_colorattachment[i],
                             0);

      GLenum err = glGetError();
      if (err != GL_NO_ERROR)
        AERO_CORE_ERROR("Color attachment {} error: 0x{:04x}", i, err);
    }
  }

  // ── Depth/stencil — use Renderbuffer (faster on mobile, no texture needed) ─
  if (m_depth_attachment_specifications.texture_format ==
      FrameBuffer_Texture_Format::DEPTH24STENCIL8)
  {
    glGenRenderbuffers(1, &m_depthattachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthattachment);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          (GLsizei)m_specification.size.x,
                          (GLsizei)m_specification.size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_depthattachment);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
      AERO_CORE_ERROR("Depth attachment error: 0x{:04x}", err);
  }

  // ── Draw buffers for MRT ───────────────────────────────────────────────────
  if (m_colorattachment.size() > 1)
  {
    std::vector<GLenum> buffers;
    for (size_t i = 0; i < m_colorattachment.size(); i++)
      buffers.push_back(GL_COLOR_ATTACHMENT0 + (GLenum)i);
    glDrawBuffers((GLsizei)buffers.size(), buffers.data());
  }

  // ── Completeness check ─────────────────────────────────────────────────────
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    AERO_CORE_ERROR("Framebuffer incomplete! Status: 0x{:04x}", status);
  else
    AERO_CORE_INFO("Framebuffer complete: {}x{}", m_specification.size.x, m_specification.size.y);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL_ESFrameBuffer::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
  glViewport(0, 0, (GLsizei)m_specification.size.x, (GLsizei)m_specification.size.y);
}

void OpenGL_ESFrameBuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL_ESFrameBuffer::resize(const vec2u& size)
{
  if (size.x == 0 || size.y == 0 || size.x > s_max_framebuffer_size ||
      size.y > s_max_framebuffer_size)
  {
    AERO_CORE_WARN("Invalid framebuffer resize: {}x{}", size.x, size.y);
    return;
  }
  m_specification.size = size;
  invalidate();
}

int OpenGL_ESFrameBuffer::read_pixel(AG_uint attachment_index, const vec2i& pos)
{
  GLint old_fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

  glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
  int pixel_data = -1;
  int flipped_y  = (int)m_specification.size.y - 1 - pos.y;
  glReadPixels(pos.x, flipped_y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);

  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)old_fbo);
  return pixel_data;
}

void OpenGL_ESFrameBuffer::clear_attachment(AG_uint attachment_index, int value)
{
  if (attachment_index >= m_colorattachment.size())
    return;

  GLint old_fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

  auto fmt = m_color_attachment_specifications[attachment_index].texture_format;
  if (fmt == FrameBuffer_Texture_Format::RED_INTEGER)
  {
    // Integer attachment — must use glClearBufferiv
    GLint clear_val[4] = {value, 0, 0, 0};
    glClearBufferiv(GL_COLOR, (GLint)attachment_index, clear_val);
  }
  else
  {
    // Float/RGBA attachment
    GLfloat clear_val[4] = {0.f, 0.f, 0.f, 1.f};
    glClearBufferfv(GL_COLOR, (GLint)attachment_index, clear_val);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)old_fbo);
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
