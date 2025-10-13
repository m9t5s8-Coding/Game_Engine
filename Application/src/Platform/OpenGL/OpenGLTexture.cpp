#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLTexture.hpp>
#include <glad/glad.h>
#include <stb_image.h>

namespace aero
{
  OpenGLTexture2D::OpenGLTexture2D(const std::string &p_path)
      : m_path(p_path)
  {
    // Load the image from the file
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *data = stbi_load(p_path.c_str(), &width, &height, &channels, 4);
    AERO_CORE_ASSERT(data, "Failed to load Image!");
    m_width = width;
    m_height = height;

    // Create and Bind the texture
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    // How to load images linear or nearest
    // Upscaling and Downscaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Repeat Horizontally and Vertically
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload the Image Data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Generate MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // Unbind Texture
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  OpenGLTexture2D::~OpenGLTexture2D()
  {
    glDeleteTextures(1, &m_ID);
  }

  void OpenGLTexture2D::bind(uint32_t p_slot) const
  {
    glActiveTexture(GL_TEXTURE0 + p_slot);
    glBindTexture(GL_TEXTURE_2D, m_ID);
  }
}
