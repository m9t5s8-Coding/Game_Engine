#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESVertexArray.hpp>

namespace ag
{

static GLenum shader_dt_to_openGL_type(ShaderDataType type)
{
  switch (type)
  {
    case ShaderDataType::Float:
    case ShaderDataType::Float2:
    case ShaderDataType::Float3:
    case ShaderDataType::Float4:
      return GL_FLOAT;
    case ShaderDataType::Mat3:
    case ShaderDataType::Mat4:
      return GL_FLOAT;
    case ShaderDataType::Int:
    case ShaderDataType::Int2:
    case ShaderDataType::Int3:
    case ShaderDataType::Int4:
      return GL_INT;
    case ShaderDataType::Bool:
      return GL_BOOL;
    default:
      AERO_CORE_ASSERT(false, "Unknown Shader Data Type!");
      return 0;
  }
}

OpenGL_ESVertexArray::OpenGL_ESVertexArray()
{
  glGenVertexArrays(1, &m_ID);
  glBindVertexArray(m_ID);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES VertexArray creation error: 0x{:04x}", error);
  }
}

OpenGL_ESVertexArray::~OpenGL_ESVertexArray()
{
  if (m_ID)
  {
    glDeleteVertexArrays(1, &m_ID);
  }
}

void OpenGL_ESVertexArray::bind() const
{
  glBindVertexArray(m_ID);
}

void OpenGL_ESVertexArray::unbind() const
{
  glBindVertexArray(0);
}

void OpenGL_ESVertexArray::add_vertex_buffer(const AG_ref<VertexBuffer>& p_vertexbuffer,
                                             bool                        instanced)
{
  if (!p_vertexbuffer)
  {
    AERO_CORE_ERROR("Cannot add null vertex buffer");
    return;
  }

  glBindVertexArray(m_ID);
  p_vertexbuffer->bind();

  const auto& layout = p_vertexbuffer->get_layout();

  // Calculate starting index based on existing vertex buffers
  uint32_t starting_index = 0;
  for (const auto& buffer : m_vertexbuffers)
  {
    starting_index += buffer->get_layout().get_element_count();
  }

  uint32_t index = starting_index;

  for (const auto& element : layout)
  {
    glEnableVertexAttribArray(index);

    GLenum gl_type         = shader_dt_to_openGL_type(element.type);
    int    component_count = element.get_component_count();

    if (gl_type == GL_FLOAT)
    {
      glVertexAttribPointer(index,
                            component_count,
                            gl_type,
                            element.normalized ? GL_TRUE : GL_FALSE,
                            layout.get_stride(),
                            reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
    }
    else if (gl_type == GL_INT)
    {
      glVertexAttribIPointer(index,
                             component_count,
                             GL_INT,
                             layout.get_stride(),
                             reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
    }

    // Handle instanced rendering
    if (instanced)
    {
      // Check if instanced rendering is supported (OpenGL ES 3.0+)
      // glVertexAttribDivisor is available in OpenGL ES 3.0+
      glVertexAttribDivisor(index, 1);
    }

    index++;
  }

  m_vertexbuffers.push_back(p_vertexbuffer);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES VertexArray add_vertex_buffer error: 0x{:04x}", error);
  }
}

void OpenGL_ESVertexArray::set_index_buffer(const AG_ref<IndexBuffer>& p_indexbuffer)
{
  if (!p_indexbuffer)
  {
    AERO_CORE_ERROR("Cannot set null index buffer");
    return;
  }

  glBindVertexArray(m_ID);
  p_indexbuffer->bind();
  m_indexbuffer = p_indexbuffer;

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES VertexArray set_index_buffer error: 0x{:04x}", error);
  }
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
