#include <Renderer/Shader.hpp>
#include <glad/glad.h>
#include <iostream>

namespace aero
{
  Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
  {
    

    std::string vertexCode = read_file(vertexPath);
    std::string fragmentCode = read_file(fragmentPath);

    const char *vertexSrc = vertexCode.c_str();
    const char *fragmentSrc = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);
    check_compile_errors(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);
    check_compile_errors(fragmentShader, "FRAGMENT");

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);
    check_compile_errors(m_ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


  }

  std::string Shader::read_file(const std::string &filePath) const
  {
    std::ifstream file(filePath);
    if (!file.is_open())
    {
      std::cerr << "ERROR::FILE_NOT_SUCCESFULLY_READ: " << filePath << std::endl;
      return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  void Shader::use_shader() const
  {
    glUseProgram(m_ID);
  }

  void Shader::check_compile_errors(unsigned int shader, const std::string &type) const
  {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                  << infoLog << "\n";
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                  << infoLog << "\n";
      }
    }
  }

  void Shader::set_bool(const std::string &name, bool value) const
  {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
  }

  void Shader::set_int(const std::string &name, int value) const
  {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void Shader::set_float(const std::string &name, float value) const
  {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void Shader::set_vector_2f(const std::string &name, const aero::vector_2f &value) const
  {
    glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y);
  }

  void Shader::set_color(const std::string &name, const aero::Color &color) const
  {
    float r, g, b, a;
    color.NormalizedColor(r, g, b, a);
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), r, g, b, a);
  }
  void Shader::set_float_rect(const std::string &name, const aero::FloatRect &rect) const
  {
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), rect.x, rect.y, rect.width, rect.height);
  }
}
