#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLTexture.hpp>
#include <glad/glad.h>

#define STBI_NO_SIMD
#include <stb_image.h>


#include <Project/Assetmanager.hpp>

namespace ag
{
  OpenGLTexture2D::OpenGLTexture2D(const std::string& p_path, Filter_Mode filter_mode)
    : m_path(p_path)
  {
    int width = 0, height = 0, channels = 0;
    stbi_uc* pixels = nullptr;

    // pak file
    if (AssetManager::is_packed())
    {
      auto bytes = AssetManager::read_bytes(p_path);
      AERO_CORE_ASSERT(!bytes.empty(), "Failed to load texture from pak: {0}", p_path);

      pixels = stbi_load_from_memory(
        bytes.data(),
        (int)bytes.size(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
      );
    }
    else
    {
      pixels = stbi_load(
        p_path.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
      );
    }

    AERO_CORE_ASSERT(pixels, "stb_image failed to load: {0}", p_path);

    m_size = vec2u(width, height);


    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);


    switch (filter_mode)
    {
    case AG_NEAREST:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;

    case AG_LINEAR:
    default:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    }


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      width,
      height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      pixels
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
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

#endif
