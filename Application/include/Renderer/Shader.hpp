#pragma once

#include <fstream>
#include <sstream>
#include <memory>
#include <Renderer/Color.hpp>
#include <Math/Vector2.hpp>
#include <Math/Rect.hpp>
namespace aero
{
  class Shader
  {

  public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    void use_shader() const;

    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vector_2f(const std::string &name, const aero::vector_2f &value) const;
    void set_color(const std::string &name, const aero::Color &color) const;
    void set_float_rect(const std::string &name, const aero::FloatRect &rect) const;

  private:
    void check_compile_errors(unsigned int shader, const std::string &type) const;
    std::string read_file(const std::string &filePath) const;
    unsigned int m_ID;
  };
}
