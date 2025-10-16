#include <Renderer/Renderer2D.hpp>
#include <array>
#include <Renderer/Renderer.hpp>

namespace ag
{
  Renderer2D::Renderer2D_Data *Renderer2D::s_data = nullptr;

  void Renderer2D::init()
  {
    s_data = new Renderer2D_Data();
    s_data->max_rect = 1000;
    s_data->max_vertices = s_data->max_rect * 4;
    s_data->max_indices = s_data->max_rect * 6;

    s_data->vertices.resize(s_data->max_vertices);


    s_data->vertexarray = ag::VertexArray::create();
    float vertices[] = {
        -0.25f, 0.25f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -0.25f, -0.25f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.25f, -0.25f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.25f, 0.25f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
        // position     color                   texture coord
    };

    AG_ref<VertexBuffer> vertexbuffer;
    vertexbuffer = ag::VertexBuffer::create(nullptr, sizeof(Vertex_Data) * s_data->max_vertices);
    BufferLayout layout = {
        {ShaderDataType::Float2, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_TextureCoord"}};
    vertexbuffer->set_layout(layout);
    s_data->vertexarray->add_vertex_buffer(vertexbuffer);

    AG_uint *indices = new AG_uint[s_data->max_indices];
    for (AG_uint i = 0; i < s_data->max_rect; i++)
    {
      indices[i * 6 + 0] = i * 4 + 0;
      indices[i * 6 + 1] = i * 4 + 1;
      indices[i * 6 + 2] = i * 4 + 2;
      indices[i * 6 + 3] = i * 4 + 2;
      indices[i * 6 + 4] = i * 4 + 3;
      indices[i * 6 + 5] = i * 4 + 0;
    }

    AG_ref<IndexBuffer> indexbuffer;
    indexbuffer = ag::IndexBuffer::create(indices, s_data->max_indices);
    s_data->vertexarray->set_index_buffer(indexbuffer);
    delete[] indices;

    s_data->shader = ag::Shader::create("assets/shaders/rectangle.glsl");
  }
  void Renderer2D::shut_down()
  {
    delete s_data;
    s_data = nullptr;
  }
  void Renderer2D::begin_scene(const ViewController &view_controller)
  {
    s_data->view_center = view_controller.get_view().get_center();
    s_data->view_size = view_controller.get_view().get_size();
    s_data->total_rect = 0;
    s_data->vertices.clear();
  }
  void Renderer2D::end_scene()
  {
    flush();
  }
  void Renderer2D::use_shader(const AG_ref<Shader> &shader)
  {
    s_data->shader = shader;
  }

  void Renderer2D::flush()
  {
    if (s_data->total_rect == 0)
      return;

    AG_ref<VertexBuffer> vertexbuffer;
    vertexbuffer = s_data->vertexarray->get_vertex_buffer();
    vertexbuffer->set_data((const void *)s_data->vertices.data(), s_data->total_rect * 4 * sizeof(Vertex_Data));

    AG_ref<IndexBuffer> indexbuffer;
    indexbuffer = s_data->vertexarray->get_index_buffer();
    indexbuffer->set_count(s_data->total_rect * 6);

    Renderer::submit(s_data->shader, s_data->vertexarray);

    s_data->total_rect = 0;
    s_data->vertices.clear();
    s_data->vertices.resize(s_data->max_vertices);
  }

  vec2f Renderer2D::calc_vertex(vec2f vertex)
  {
    vec2f ndc;
    ndc.x = (vertex.x - (s_data->view_center.x - s_data->view_size.x / 2.0f)) / s_data->view_size.x * 2.0f - 1.0f;
    ndc.y = 1.0 - (vertex.y - (s_data->view_center.y - s_data->view_size.y / 2.0f)) / s_data->view_size.y * 2.0f;
    return ndc;
  }

  void Renderer2D::draw_rect(const Rectangle &rect_data)
  {

    if (s_data->total_rect >= s_data->max_rect)
      flush();

    std::array<vec2f, 4> vertex = {
        vec2f{0.f, 0.f},
        vec2f{0.f, 1.f},
        vec2f{1.f, 1.f},
        vec2f{1.f, 0.f}};

    std::array<vec2f, 4> tex_coord = {
        vec2f{0.0f, 1.0f},
        vec2f{0.0f, 0.0f},
        vec2f{1.0f, 0.0f},
        vec2f{1.0f, 1.0f}};

    float r, g, b, a;
    rect_data.color.NormalizedColor(r, g, b, a);

    size_t vertex_offset = s_data->total_rect * 4;
    for (int i = 0; i < 4; i++)
    {
      vec2f size = rect_data.size * rect_data.scale;
      vec2f v = vertex[i];
      v = v * size;
      v -= rect_data.origin;
      v += rect_data.position;

      vec2f ndc_vertex = calc_vertex(v);
      s_data->vertices.push_back({ndc_vertex.x, ndc_vertex.y, r, g, b, a, tex_coord[i].x, tex_coord[i].y});
    }
    s_data->total_rect++;
  }
}
