// #include <Platform/OpenGL/VertexArray.hpp>
// #include <glad/glad.h>

// namespace aero
// {
//   VertexArray::VertexArray()
//   {
//     glGenVertexArrays(1, &m_ID);
//   }
//   VertexArray::~VertexArray()
//   {
//     glDeleteVertexArrays(1, &m_ID);
//   }
//   void VertexArray::link_vbo(VertexBuffer *vbo, unsigned int layout, int stride, void *offset)
//   {
//     vbo->bind();

//     glVertexAttribPointer(layout, 2, GL_FLOAT, GL_FALSE, stride, offset);
//     glEnableVertexAttribArray(layout);

//     vbo->unbind();
//   }

//   void VertexArray::bind() const
//   {
//     glBindVertexArray(m_ID);
//   }

//   void VertexArray::unbind()
//   {
//     glBindVertexArray(0);
//   }
// }
