#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace ag
{
  static GLenum string_to_shadertype(const std::string &p_type)
  {
    if (p_type == "vertex")
      return GL_VERTEX_SHADER;
    if (p_type == "fragment")
      return GL_FRAGMENT_SHADER;

    AERO_CORE_ASSERT(false, "Unknown Shader Types!");
    return 0;
  }

  OpenGLShader::OpenGLShader(const std::string &p_shader_path)
  {
    std::string src = read_file(p_shader_path);
    auto shader_src = process_shader_src(src);

    compile_shaders(shader_src);

    // Extract the File name from the path
    auto last_slash = p_shader_path.find_last_of("/\\");
    last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
    auto last_dot = p_shader_path.find(".");
    auto count = last_dot == std::string::npos ? p_shader_path.size() - last_slash : last_dot - last_slash;

    m_name = p_shader_path.substr(last_slash, count);
  }

  OpenGLShader::OpenGLShader(const std::string &p_name, const std::string& p_shader_path)
    : m_name(p_name)
  {
    std::string src = read_file(p_shader_path);
    auto shader_src = process_shader_src(src);

    compile_shaders(shader_src);
  }

  OpenGLShader::OpenGLShader(const std::string& p_name, const std::string &p_vertex_path, const std::string &p_fragment_path)
    : m_name(p_name)
  {
    std::string l_vertex_code = read_file(p_vertex_path);
    std::string l_fragment_code = read_file(p_fragment_path);

    std::unordered_map<GLenum, std::string> l_shader_src = {
        {GL_VERTEX_SHADER, l_vertex_code},
        {GL_FRAGMENT_SHADER, l_fragment_code}};

    compile_shaders(l_shader_src);
  }

  OpenGLShader::~OpenGLShader()
  {
    glDeleteProgram(m_ID);
  }

  void OpenGLShader::bind() const
  {
    glUseProgram(m_ID);
  }

  void OpenGLShader::unbind() const
  {
    glUseProgram(0);
  }

  std::string OpenGLShader::read_file(const std::string &filePath) const
  {
    std::string l_result_src;
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (file)
    {
      file.seekg(0, std::ios::end);
      l_result_src.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(&l_result_src[0], l_result_src.size());
      file.close();
    }
    else
    {
      AERO_CORE_ERROR("Could not Open Shader File: {0}", filePath);
    }
    return l_result_src;
  }

  std::unordered_map<GLenum, std::string> OpenGLShader::process_shader_src(const std::string &p_shader_src)
  {
    std::unordered_map<GLenum, std::string> l_shader_src;

    const char *type_token = "#type";
    const size_t type_token_length = strlen(type_token);
    size_t pos = p_shader_src.find(type_token, 0);

    while (pos != std::string::npos)
    {
      // Find end of line
      size_t eol = p_shader_src.find_first_of("\r\n", pos);
      AERO_CORE_ASSERT((eol != std::string::npos), "Syntax error: missing end of line after #type");

      // Extract shader type ("vertex" or "fragment")
      size_t begin = pos + type_token_length + 1;
      std::string type = p_shader_src.substr(begin, eol - begin);
      AERO_CORE_ASSERT((type == "vertex" || type == "fragment"), "Invalid shader type!");

      // Find next line (actual GLSL starts after this)
      size_t next_line_pos = p_shader_src.find_first_not_of("\r\n", eol);
      AERO_CORE_ASSERT((next_line_pos != std::string::npos), "Shader source missing after #type");

      // Find next #type or end of file
      pos = p_shader_src.find(type_token, next_line_pos);

      if (pos == std::string::npos)
      {
        // Last shader block — goes to EOF
        l_shader_src[string_to_shadertype(type)] = p_shader_src.substr(next_line_pos);
      }
      else
      {
        // Extract text until next #type
        l_shader_src[string_to_shadertype(type)] = p_shader_src.substr(next_line_pos, pos - next_line_pos);
      }
    }

    return l_shader_src;
  }

  void OpenGLShader::compile_shaders(const std::unordered_map<GLenum, std::string> &p_shader_src)
  {
    unsigned int program = glCreateProgram();
    std::array<GLenum, 2> shader_id;
    int shader_index = 0;
    for (auto &src : p_shader_src)
    {
      GLenum shader_type = src.first;
      const std::string shader_str = src.second;
      const char *shader_code = shader_str.c_str();
      GLuint shader = glCreateShader(shader_type);

      glShaderSource(shader, 1, &shader_code, nullptr);
      glCompileShader(shader);
      if (check_compile_errors(shader, shader_type))
      {
        glAttachShader(program, shader);
        shader_id[shader_index++] = shader;
      }
      else
      {
        glDeleteShader(shader);
        break;
      }
    }
    glLinkProgram(program);
    check_compile_errors(program, GL_PROGRAM);
    m_ID = program;

    for (auto id : shader_id)
    {
      glDeleteShader(id);
    }
  }

  bool OpenGLShader::check_compile_errors(GLuint shader, const GLenum type)
  {
    int success;
    char infoLog[1024];
    if (type != GL_PROGRAM)
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        AERO_CORE_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {0} \n {1}", type, infoLog);
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        AERO_CORE_ERROR("ERROR::PROGRAM_LINKING_ERROR of type: {0} \n {1}", type, infoLog);
      }
    }
    return success;
  }

  void OpenGLShader::set_bool(const std::string &name, bool value) const
  {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
  }

  void OpenGLShader::set_int(const std::string &name, int value) const
  {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void OpenGLShader::set_float(const std::string &name, float value) const
  {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void OpenGLShader::set_vec2f(const std::string &name, const ag::vec2f &value) const
  {
    glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y);
  }

  void OpenGLShader::set_color(const std::string &name, const ag::Color &color) const
  {
    float r, g, b, a;
    color.NormalizedColor(r, g, b, a);
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), r, g, b, a);
  }
  void OpenGLShader::set_float_rect(const std::string &name, const ag::rectf &rect) const
  {
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), rect.x, rect.y, rect.width, rect.height);
  }

  void OpenGLShader::set_mat4(const std::string &name, const glm::mat4 &p_mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(p_mat));
  }
}
