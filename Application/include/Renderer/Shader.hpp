#pragma once

#include <memory>
#include <unordered_map>

namespace ag
{
  class Shader
  {
  public:
    virtual ~Shader() = default;
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual const std::string& get_name() const = 0;


    static std::shared_ptr<Shader> create(const std::string &p_shader_path);
    static std::shared_ptr<Shader> create(const std::string& p_name, const std::string &p_shader_path);
    static std::shared_ptr<Shader> create(const std::string& p_name, const std::string &p_vertex_path, const std::string &p_fragment_path);
  };

  class ShaderLibrary
  {
  public:
    void add(const std::shared_ptr<Shader>& p_shader);

    std::shared_ptr<Shader> load(const std::string& p_filepath);
    std::shared_ptr<Shader> load(const std::string& p_name, const std::string& p_filepath);
    std::shared_ptr<Shader> load(const std::string& p_name, const std::string& p_vertex_path, const std::string& p_fragment_path);

    std::shared_ptr<Shader> get(const std::string& p_name);

  private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shader;
  };
}
