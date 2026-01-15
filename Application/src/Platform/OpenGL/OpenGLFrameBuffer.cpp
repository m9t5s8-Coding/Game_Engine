#include<Platform/OpenGL/OpenGLFrameBuffer.hpp>
#include<Apch.hpp>
#include <glad/glad.h>

namespace ag
{
  static const AG_uint s_max_framebuffer_size = 8192;


  namespace utils
  {
    static GLenum texture_target(bool multispamled)
    {
      return  multispamled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void create_texture(bool multisampled, AG_uint* out_id, AG_uint count)
    {
      glCreateTextures(texture_target(multisampled), count, out_id);
    }

    static void bind_texture(bool multisampled, AG_uint id)
    {
      glBindTexture(texture_target(multisampled), id);
    }

    static void attachcolor_texture(uint32_t id, int samples, GLenum internal_format, GLenum format, const vec2f& size, int index)
    {
      bool multisampled = samples > 1;
      if (multisampled)
      {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, size.x, size.y, GL_FALSE);
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture_target(multisampled), id, 0);
    }

    static void attachdepth_texture(uint32_t id, int samples, GLenum format, GLenum attachment_type, const vec2f& size)
    {
      bool multisampled = samples > 1;
      if (multisampled)
      {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, size.x, size.y, GL_FALSE);
      }
      else
      {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, size.x, size.y);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }

      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, texture_target(multisampled), id, 0);
    }

    static bool is_depth_format(FrameBuffer_Texture_Format format)
    {
      switch (format)
      {
      case FrameBuffer_Texture_Format::DEPTH24STENCIL8:  return true;
      }

      return false;
    }

    static GLenum texture_format_to_gl(FrameBuffer_Texture_Format format)
    {
      switch (format)
      {
      case FrameBuffer_Texture_Format::RGBA8:       return GL_RGBA8;
      case FrameBuffer_Texture_Format::RED_INTEGER: return GL_RED_INTEGER;
      }
      return 0;
    }
  }



	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_specification(spec)
	{
    for (auto spec : m_specification.attachments.attachments)
    {
      if (!utils::is_depth_format(spec.texture_format))
        m_color_attachment_specifications.emplace_back(spec);
      else
        m_depth_attachment_specifications = spec;
    }
    invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
    glDeleteFramebuffers(1, &m_ID);
    glDeleteTextures(m_colorattachment.size(), m_colorattachment.data());
    glDeleteRenderbuffers(1, &m_depthattachment);
	}

  void OpenGLFrameBuffer::invalidate()
  {
    if (m_ID)
    {
      glDeleteFramebuffers(1, &m_ID);
      glDeleteTextures(m_colorattachment.size(), m_colorattachment.data());
      glDeleteRenderbuffers(1, &m_depthattachment);

      m_colorattachment.clear();
      m_depthattachment = 0;
    }

    // Generate and Bind Frame Buffers
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

    bool multisampled = m_specification.samples > 1;

    if (m_color_attachment_specifications.size())
    {
      m_colorattachment.resize(m_color_attachment_specifications.size());
      utils::create_texture(multisampled, m_colorattachment.data(), m_colorattachment.size());

      for (size_t i = 0; i < m_colorattachment.size(); i++)
      {
        utils::bind_texture(multisampled, m_colorattachment[i]);
        switch (m_color_attachment_specifications[i].texture_format)
        {
        case FrameBuffer_Texture_Format::RGBA8:
          utils::attachcolor_texture(m_colorattachment[i], m_specification.samples, GL_RGBA8, GL_RGBA, m_specification.size, i);
          break;
        case FrameBuffer_Texture_Format::RED_INTEGER:
          utils::attachcolor_texture(m_colorattachment[i], m_specification.samples, GL_R32I, GL_RED_INTEGER, m_specification.size, i);
          break;
        }
      }
    }

    if (m_depth_attachment_specifications.texture_format != FrameBuffer_Texture_Format::None)
    {
      utils::create_texture(multisampled, &m_depthattachment, 1);
      utils::bind_texture(multisampled, m_depthattachment);
      switch (m_depth_attachment_specifications.texture_format)
      {
      case FrameBuffer_Texture_Format::DEPTH24STENCIL8:
        utils::attachdepth_texture(m_depthattachment, m_specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_specification.size);
        break;
      }
    }

    if (m_colorattachment.size() > 1)
    {
      GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
      glDrawBuffers(m_colorattachment.size(), buffers);
    }
    else if (m_colorattachment.empty())
    {
      glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      AERO_CORE_INFO("FrameBuffer Incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  }

  void OpenGLFrameBuffer::bind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    glViewport(0, 0, m_specification.size.x, m_specification.size.y);
  }

  void OpenGLFrameBuffer::unbind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  void OpenGLFrameBuffer::resize(const vec2u& size)
  {
    if (size.x == 0 || size.y == 0 || size.x > s_max_framebuffer_size || size.y > s_max_framebuffer_size)
    {
     AERO_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", size.x, size.y);
      return;
    }
    m_specification.size = size;

    invalidate();
  }

  int OpenGLFrameBuffer::read_pixel(AG_uint attachment_index, const vec2i& pos)
  {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    int pixel_data;
    int flipped_y = m_specification.size.y - 1 - pos.y;
    glReadPixels(pos.x, flipped_y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
    return pixel_data;
  }

  void OpenGLFrameBuffer::clear_attachment(AG_uint attachment_index, int value)
  {
    auto& spec = m_color_attachment_specifications[attachment_index];
    glClearTexImage(m_colorattachment[attachment_index], 0,
      utils::texture_format_to_gl(spec.texture_format), GL_INT, &value);
  }

}
