#include<Platform/OpenGL/OpenGLFrameBuffer.hpp>
#include<Apch.hpp>
#include <glad/glad.h>

namespace ag
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_specification(spec)
	{
    invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
    glDeleteFramebuffers(1, &m_ID);
    glDeleteTextures(1, &m_colorattachment);
    glDeleteRenderbuffers(1, &m_depthattachment);
	}

  void OpenGLFrameBuffer::invalidate()
  {
    if (m_ID)
    {
      glDeleteFramebuffers(1, &m_ID);
      glDeleteTextures(1, &m_colorattachment);
      glDeleteRenderbuffers(1, &m_depthattachment);
    }

    // Generate and Bind Frame Buffers
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

    // Gen and Bind Textures
    glGenTextures(1, &m_colorattachment);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specification.size.x, m_specification.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorattachment, 0);

    glGenTextures(1, &m_depthattachment);
    glBindTexture(GL_TEXTURE_2D, m_depthattachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_specification.size.x, m_specification.size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthattachment);

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
    m_specification.size = size;
    invalidate();
  }
}
