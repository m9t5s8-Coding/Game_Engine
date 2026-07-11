#pragma once

#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Renderer/Shader.hpp>
  #include <string>
  #include <unordered_map>

namespace ag
{
class OpenGL_ESShader : public Shader
{
public:
  OpenGL_ESShader(const std::string& p_shader_path);
  OpenGL_ESShader(const std::string& p_name, const std::string& p_shader_path);
  OpenGL_ESShader(const std::string& p_name,
                  const std::string& p_vertex_path,
                  const std::string& p_fragment_path);
  virtual ~OpenGL_ESShader();

  virtual void bind() const override;
  virtual void unbind() const override;

  virtual const std::string& get_name() const override
  {
    return m_name;
  };

  virtual void set_vec2f(const std::string& name, const vec2f& value) const override;
  virtual void set_bool(const std::string& name, bool value) const override;
  virtual void set_int(const std::string& name, int value) const override;
  virtual void set_float(const std::string& name, float value) const override;
  virtual void set_mat3(const std::string& name, const glm::mat3& p_mat) const override;
  virtual void set_int_array(const std::string& name, int* values, AG_uint count) const override;

  virtual void set_float_rect(const std::string& name, const float_rect& rect) const override;
  void         set_mat4(const std::string& name, const glm::mat4& p_mat) const;

private:
  std::string                             read_file(const std::string& filePath) const;
  std::unordered_map<GLenum, std::string> process_shader_src(const std::string& p_shader_src);
  void compile_shaders(const std::unordered_map<GLenum, std::string>& p_shader_src);

  GLuint      m_ID;
  std::string m_name;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
