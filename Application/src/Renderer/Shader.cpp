#include <Apch.hpp>
#include <Renderer/Shader.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>
#include <Renderer/Renderer.hpp>

namespace ag
{
  // Combined Shader
  std::shared_ptr<Shader> Shader::create(const std::string& p_shader_path)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(p_shader_path);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }

  // Name with Combined Shader
  std::shared_ptr<Shader> Shader::create(const std::string& p_name, const std::string &p_shader_path)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(p_name, p_shader_path);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }

  //Name and Seperate Shader
  std::shared_ptr<Shader> Shader::create(const std::string& p_name, const std::string &p_vertex_path, const std::string &p_fragment_path)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(p_name, p_vertex_path, p_fragment_path);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }


  void ShaderLibrary::add(const std::shared_ptr<Shader>& p_shader)
  { // adding the shader based on the name or become easy to retrive
    auto& name = p_shader->get_name();
    AERO_CORE_ASSERT((m_shader.find(name) == m_shader.end()), "Shader Name already exists!");
    m_shader[name] = p_shader;
  }

  std::shared_ptr<Shader> ShaderLibrary::load(const std::string& p_filepath)
  {
    auto shader = Shader::create(p_filepath);
    add(shader);
    return shader;
  }

  std::shared_ptr<Shader> ShaderLibrary::load(const std::string& p_name, const std::string& p_filepath)
  {
    auto shader = Shader::create(p_name, p_filepath);
    add(shader);
    return shader;
  }

  std::shared_ptr<Shader> ShaderLibrary::load(const std::string& p_name, const std::string& p_vertex_path, const std::string& p_fragment_path)
  {
    auto shader = Shader::create(p_name, p_vertex_path, p_fragment_path);
    add(shader);
    return shader;
  }

  std::shared_ptr<Shader> ShaderLibrary::get(const std::string& p_name)
  {
    AERO_CORE_ASSERT((m_shader.find(p_name) != m_shader.end()), "Shader donot exists!");
    return m_shader[p_name];
  }
}
