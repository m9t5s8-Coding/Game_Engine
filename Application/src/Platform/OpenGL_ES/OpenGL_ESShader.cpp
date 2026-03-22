#ifdef PLATFORM_ANDROID

  #include <android/log.h>

  #include <array>
  #include <Core/Log.hpp>
  #include <fstream>
  #include <glm/gtc/type_ptr.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESShader.hpp>
  #include <Project/Assetmanager.hpp>
  #include <sstream>

namespace ag
{

static GLenum string_to_shadertype(const std::string& p_type)
{
  if (p_type == "vertex")
    return GL_VERTEX_SHADER;
  if (p_type == "fragment")
    return GL_FRAGMENT_SHADER;

  AERO_CORE_ASSERT(false, "Unknown Shader Type!");
  return 0;
}

OpenGL_ESShader::OpenGL_ESShader(const std::string& p_shader_path)
{
  std::string src        = read_file(p_shader_path);
  auto        shader_src = process_shader_src(src);
  compile_shaders(shader_src);

  // Extract the file name from the path
  auto last_slash = p_shader_path.find_last_of("/\\");
  last_slash      = last_slash == std::string::npos ? 0 : last_slash + 1;
  auto last_dot   = p_shader_path.find_last_of(".");
  auto count =
      last_dot == std::string::npos ? p_shader_path.size() - last_slash : last_dot - last_slash;
  m_name = p_shader_path.substr(last_slash, count);
}

OpenGL_ESShader::OpenGL_ESShader(const std::string& p_name, const std::string& p_shader_path)
  : m_name(p_name)
{
  std::string src        = read_file(p_shader_path);
  auto        shader_src = process_shader_src(src);
  compile_shaders(shader_src);
}

OpenGL_ESShader::OpenGL_ESShader(const std::string& p_name,
                                 const std::string& p_vertex_path,
                                 const std::string& p_fragment_path)
  : m_name(p_name)
{
  std::string l_vertex_code   = read_file(p_vertex_path);
  std::string l_fragment_code = read_file(p_fragment_path);

  std::unordered_map<GLenum, std::string> l_shader_src = {
      {  GL_VERTEX_SHADER,   l_vertex_code},
      {GL_FRAGMENT_SHADER, l_fragment_code}
  };

  compile_shaders(l_shader_src);
}

OpenGL_ESShader::~OpenGL_ESShader()
{
  if (m_ID)
  {
    glDeleteProgram(m_ID);
  }
}

void OpenGL_ESShader::bind() const
{
  glUseProgram(m_ID);
}

void OpenGL_ESShader::unbind() const
{
  glUseProgram(0);
}

std::string OpenGL_ESShader::read_file(const std::string& filePath) const
{
  if (AssetManager::is_packed(AssetManager::Domain::Engine))
  {
    return AssetManager::read_string(filePath, AssetManager::Domain::Engine);
  }

  std::string   result_src;
  std::ifstream file(filePath, std::ios::in | std::ios::binary);
  if (file)
  {
    file.seekg(0, std::ios::end);
    result_src.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&result_src[0], result_src.size());
    file.close();
  }
  else
  {
    AERO_CORE_ERROR("Could not open shader file: {}", filePath);
  }
  return result_src;
}

std::unordered_map<GLenum, std::string>
OpenGL_ESShader::process_shader_src(const std::string& p_shader_src)
{
  std::unordered_map<GLenum, std::string> l_shader_src;

  const char*  type_token        = "#type";
  const size_t type_token_length = strlen(type_token);
  size_t       pos               = p_shader_src.find(type_token, 0);

  while (pos != std::string::npos)
  {
    // Find end of line
    size_t eol = p_shader_src.find_first_of("\r\n", pos);
    AERO_CORE_ASSERT((eol != std::string::npos), "Syntax error: missing end of line after #type");

    // Extract shader type ("vertex" or "fragment")
    size_t      begin = pos + type_token_length + 1;
    std::string type  = p_shader_src.substr(begin, eol - begin);
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
      l_shader_src[string_to_shadertype(type)] =
          p_shader_src.substr(next_line_pos, pos - next_line_pos);
    }
  }

  return l_shader_src;
}

void OpenGL_ESShader::compile_shaders(const std::unordered_map<GLenum, std::string>& p_shader_src)
{
  GLuint                program = glCreateProgram();
  std::array<GLuint, 2> shader_ids;
  int                   shader_index = 0;

  // Compile and attach shaders
  for (auto& kv : p_shader_src)
  {
    GLenum             shader_type = kv.first;
    const std::string& shader_str  = kv.second;
    const char*        shader_code = shader_str.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_code, nullptr);
    glCompileShader(shader);

    // Check compilation errors
    int  success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success)
    {
      glAttachShader(program, shader);
      shader_ids[shader_index++] = shader;
    }
    else
    {
      glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
      const char* type_str = (shader_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
      AERO_CORE_ERROR("Shader compilation error ({}):\n{}", type_str, infoLog);
      glDeleteShader(shader);
      glDeleteProgram(program);
      return;
    }
  }

  // Link program
  glLinkProgram(program);

  // Check linking errors
  int  success;
  char infoLog[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success)
  {
    glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
    AERO_CORE_ERROR("Program linking error:\n{}", infoLog);
    glDeleteProgram(program);
    return;
  }

  m_ID = program;

  // Clean up shader objects
  for (int i = 0; i < shader_index; i++)
  {
    glDetachShader(m_ID, shader_ids[i]);
    glDeleteShader(shader_ids[i]);
  }

  AERO_CORE_INFO("Shader compiled successfully: {}", m_name);
}

void OpenGL_ESShader::set_bool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void OpenGL_ESShader::set_int(const std::string& name, int value) const
{
  glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void OpenGL_ESShader::set_float(const std::string& name, float value) const
{
  glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void OpenGL_ESShader::set_vec2f(const std::string& name, const ag::vec2f& value) const
{
  glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y);
}

void OpenGL_ESShader::set_float_rect(const std::string& name, const float_rect& rect) const
{
  glUniform4f(glGetUniformLocation(m_ID, name.c_str()),
              rect.position.x,
              rect.position.y,
              rect.size.x,
              rect.size.y);
}

void OpenGL_ESShader::set_mat4(const std::string& name, const glm::mat4& p_mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(p_mat));
}

void OpenGL_ESShader::set_mat3(const std::string& name, const glm::mat3& p_mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(p_mat));
}

void OpenGL_ESShader::set_int_array(const std::string& name, int* values, AG_uint count) const
{
  glUniform1iv(glGetUniformLocation(m_ID, name.c_str()), count, values);
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
