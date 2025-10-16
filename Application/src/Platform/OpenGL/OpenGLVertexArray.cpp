#include <Platform/OpenGL/OpenGLVertexArray.hpp>
#include <Apch.hpp>
#include <glad/glad.h>

namespace ag
{
  static GLenum shader_dt_to_openGL_type(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float:   return GL_FLOAT;
      case ShaderDataType::Float2:  return GL_FLOAT;
      case ShaderDataType::Float3:  return GL_FLOAT;
      case ShaderDataType::Float4:  return GL_FLOAT;
      case ShaderDataType::Mat3:    return GL_FLOAT;
      case ShaderDataType::Mat4:    return GL_FLOAT;
      case ShaderDataType::Int:     return GL_INT;
      case ShaderDataType::Int2:    return GL_INT;
      case ShaderDataType::Int3:    return GL_INT;
      case ShaderDataType::Int4:    return GL_INT;
      case ShaderDataType::Bool:    return GL_BOOL;
    }
    AERO_CORE_ASSERT(false, "Unknown Shader Data Type!");
    return 0;
  }

  OpenGLVertexArray::OpenGLVertexArray()
  {
    glGenVertexArrays(1, &m_ID);
    glBindVertexArray(m_ID);
  }


  void OpenGLVertexArray::bind() const
  {
    glBindVertexArray(m_ID);
  }
  void OpenGLVertexArray::unbind() const
  {
    glBindVertexArray(0);
  }

  void OpenGLVertexArray::add_vertex_buffer(const AG_ref<VertexBuffer> &p_vertexbuffer)
  {
    // AERO_CORE_ASSERT(p_vertexbuffer->get_layout().get_element().size(),"Vertex buffer has no layout!");
    glBindVertexArray(m_ID);
    p_vertexbuffer->bind();


    uint32_t index = 0;
    const auto& layout = p_vertexbuffer->get_layout();
    for (const auto &element : layout)
    {
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, element.get_component_count(),
        shader_dt_to_openGL_type(element.type),
        element.normalized ? GL_TRUE : GL_FALSE,
        layout.get_stride(),
        reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
      index++;
    }
    m_vertexbuffer = p_vertexbuffer;
    // m_vertexbuffers.push_back(p_vertexbuffer);
  }
  void OpenGLVertexArray::set_index_buffer(const AG_ref<IndexBuffer> &p_indexbuffer)
  {
    glBindVertexArray(m_ID);
    p_indexbuffer->bind();

    m_indexbuffer = p_indexbuffer;
  }
}
