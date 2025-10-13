#pragma once

#include <Renderer/Shader.hpp>

#include <sstream>
#include <unordered_map>
#include <Renderer/Color.hpp>
#include <Math/Vector2.hpp>
#include <Math/Rect.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace aero
{
  class OpenGLShader : public Shader
  {
  public:
    OpenGLShader(const std::string &p_shader_path);
    OpenGLShader(const std::string &p_name, const std::string& p_shader_path);
    OpenGLShader(const std::string& p_name, const std::string &p_vertex_path, const std::string &p_fragment_path);
    virtual ~OpenGLShader();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual const std::string& get_name() const override { return m_name; };

    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vector_2f(const std::string &name, const aero::vector_2f &value) const;
    void set_color(const std::string &name, const aero::Color &color) const;
    void set_float_rect(const std::string &name, const aero::FloatRect &rect) const;

    void set_mat4(const std::string &name, const glm::mat4 &p_mat) const;

  private:
    std::string read_file(const std::string &filePath) const;
    std::unordered_map<GLenum, std::string> process_shader_src(const std::string &p_shader_src);
    void compile_shaders(const std::unordered_map<GLenum, std::string>& p_shader_src);
    bool check_compile_errors(GLuint shader, const GLenum type);
    unsigned int m_ID;
    std::string m_name;
  };
}
