#pragma once
#include <Apch.hpp>
#include <vector>
#include <string>

namespace aero
{
  enum class ShaderDataType
  {
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
  };

  static uint32_t shader_dt_size(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float:   return 4;
      case ShaderDataType::Float2:  return 4 * 2;
      case ShaderDataType::Float3:  return 4 * 3;
      case ShaderDataType::Float4:  return 4 * 4;
      case ShaderDataType::Mat3:    return 4 * 3 * 3;
      case ShaderDataType::Mat4:    return 4 * 4 * 4;
      case ShaderDataType::Int:     return 4;
      case ShaderDataType::Int2:    return 4 * 2;
      case ShaderDataType::Int3:    return 4 * 3;
      case ShaderDataType::Int4:    return 4 * 4;
      case ShaderDataType::Bool:    return 1;
    }
    AERO_CORE_ASSERT(false, "Unknown Shader Data Type!");
    return 0;
  }

  struct BufferElement
  {
    std::string name;
    ShaderDataType type;
    uint32_t offset;
    uint32_t size;

    BufferElement(ShaderDataType p_type, const std::string &p_name)
        : name(p_name), type(p_type), size(shader_dt_size(type)), offset(0)
    {
    }
  };

  class BufferLayout
  {
  public:
    BufferLayout(const std::initializer_list<BufferElement>& element)
     : m_buffer_element(element) {}

    inline const std::vector<BufferElement> &get_element() { return m_buffer_element; }

  private:
    std::vector<BufferElement> m_buffer_element;
  };
}
