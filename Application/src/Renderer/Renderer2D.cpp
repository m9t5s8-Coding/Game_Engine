#include <Renderer/Renderer2D.hpp>
#include <array>
#include <Renderer/Renderer.hpp>

#include <glm/glm.hpp>

#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>

namespace ag
{
  static const std::array<vec2f, 4> s_quad_vertex = {
      vec2f{0.f, 0.f},
      vec2f{0.f, 1.f},
      vec2f{1.f, 1.f},
      vec2f{1.f, 0.f}};

  struct Rectangle_Data
  {
    vec2f position;
    vec4f fill_color;
  };

  struct Circle_Data
  {
    vec2f position;
    vec2f center;
    vec2f radius;
    vec4f fill_color;
  };

  struct Sprite_Data
  {
    vec2f position;
    vec2f tex_coord;
  };

  struct Renderer2D_Data
  {
    // Max Shapes Data
    AG_uint max_shape = 0;
    AG_uint max_vertices = 0;
    AG_uint max_indices = 0;

    // Rectangle Data
    AG_ref<VertexArray> rect_vertexarray;
    AG_ref<Shader> rect_shader;
    AG_uint rectangle_index = 0;
    Rectangle_Data *rectangle_vb_base = nullptr;
    Rectangle_Data *rectangle_vb_ptr = nullptr;

    // Circle Data
    AG_ref<VertexArray> circle_vertexarray;
    AG_ref<Shader> circle_shader;
    AG_uint circle_index = 0;
    Circle_Data *circle_vb_base = nullptr;
    Circle_Data *circle_vb_ptr = nullptr;

    // Sprite Data
    AG_ref<VertexArray> sprite_vertexarray;
    AG_ref<Shader> sprite_shader;
    AG_ref<Texture> texture;
    AG_uint sprite_index = 0;
    Sprite_Data *sprite_vb_base = nullptr;
    Sprite_Data *sprite_vb_ptr = nullptr;

    View view;

    AG_uint total_quads = 0;
  };
  static Renderer2D_Data *s_data;

  void Renderer2D::init()
  {

    s_data = new Renderer2D_Data();
    s_data->max_shape = 50000;
    s_data->max_vertices = s_data->max_shape * 4;
    s_data->max_indices = s_data->max_shape * 6;

    AG_uint *indices = new AG_uint[s_data->max_indices];
    for (AG_uint i = 0; i < s_data->max_shape; i++)
    {
      indices[i * 6 + 0] = i * 4 + 0;
      indices[i * 6 + 1] = i * 4 + 1;
      indices[i * 6 + 2] = i * 4 + 2;
      indices[i * 6 + 3] = i * 4 + 2;
      indices[i * 6 + 4] = i * 4 + 3;
      indices[i * 6 + 5] = i * 4 + 0;
    }

    AG_ref<IndexBuffer> indexbuffer = ag::IndexBuffer::create(indices, s_data->max_indices);
    delete[] indices;

    {
      // Setup Rectangle

      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position"},
          {ShaderDataType::Float4, "a_Fill_Color"}};

      s_data->rectangle_vb_base = new Rectangle_Data[s_data->max_vertices];

      s_data->rect_vertexarray = ag::VertexArray::create();

      AG_ref<VertexBuffer> rect_vertexbuffer = ag::VertexBuffer::create(nullptr, sizeof(Rectangle_Data) * s_data->max_vertices);
      rect_vertexbuffer->set_layout(layout);

      s_data->rect_vertexarray->add_vertex_buffer(rect_vertexbuffer);
      s_data->rect_vertexarray->set_index_buffer(indexbuffer);

      s_data->rect_shader = ag::Shader::create("assets/shaders/Rectangle2D.glsl");
    }

    {
      // Setup Circle

      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position"},
          {ShaderDataType::Float2, "a_Center"},
          {ShaderDataType::Float2, "a_Radius"},
          {ShaderDataType::Float4, "a_Fill_Color"}};

      s_data->circle_vb_base = new Circle_Data[s_data->max_vertices];

      s_data->circle_vertexarray = VertexArray::create();

      AG_ref<VertexBuffer> vertexbuffer = ag::VertexBuffer::create(nullptr, s_data->max_vertices * sizeof(Circle_Data));
      vertexbuffer->set_layout(layout);

      s_data->circle_vertexarray->add_vertex_buffer(vertexbuffer);
      s_data->circle_vertexarray->set_index_buffer(indexbuffer);

      s_data->circle_shader = ag::Shader::create("assets/shaders/Circle2D.glsl");
    }

    {
      // Sprite setup
      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position"},
          {ShaderDataType::Float2, "a_TexCoord"}};

      s_data->sprite_vb_base = new Sprite_Data[s_data->max_vertices];

      s_data->sprite_vertexarray = VertexArray::create();

      AG_ref<VertexBuffer> vertexbuffer = VertexBuffer::create(nullptr, sizeof(Sprite_Data) * s_data->max_vertices);
      vertexbuffer->set_layout(layout);

      s_data->sprite_vertexarray->add_vertex_buffer(vertexbuffer);
      s_data->sprite_vertexarray->set_index_buffer(indexbuffer);

      s_data->sprite_shader = ag::Shader::create("assets/shaders/Sprite2D.glsl");
    }

    s_data->texture = ag::Texture2D::create("assets/textures/solo.jpg");
    s_data->sprite_shader->bind();
    s_data->sprite_shader->set_int("u_texture", 0);
  }

  void Renderer2D::shut_down()
  {
    if (s_data)
    {
      if (s_data->rectangle_vb_base)
        delete[] s_data->rectangle_vb_base;

      if (s_data->circle_vb_base)
        delete[] s_data->circle_vb_base;

      if (s_data->sprite_vb_base)
        delete[] s_data->sprite_vb_base;

      delete s_data;
      s_data = nullptr;
    }
  }

  void Renderer2D::begin_scene(const View &view)
  {
    Renderer::begin_scene(view);
    s_data->view = view;

    start_batch();

    s_data->total_quads = 0;
  }

  void Renderer2D::end_scene()
  {
      // AERO_CORE_INFO("Total Quads: {0}", s_data->total_quads);
      s_data->total_quads = 0;
    flush();
  }

  void Renderer2D::start_batch()
  {
    s_data->rectangle_index = 0;
    s_data->rectangle_vb_ptr = s_data->rectangle_vb_base;

    s_data->circle_index = 0;
    s_data->circle_vb_ptr = s_data->circle_vb_base;

    s_data->sprite_index = 0;
    s_data->sprite_vb_ptr = s_data->sprite_vb_base;
  }

  void Renderer2D::set_texture(const AG_ref<Texture> &texture)
  {
    if (s_data->texture == texture)
      return;

    flush_sprite();
    s_data->texture = texture;
  }

  void Renderer2D::flush()
  {
    flush_circle();
    flush_rectangle();
    flush_sprite();
  }

  void Renderer2D::draw_rectangle(const Rectangle &rect, const Transform& transform)
  {
    //bool inside_view = s_data->view.get_float_rect().intersects(rect.get_float_rect());
    //if(!inside_view) return;

    if (s_data->rectangle_index >= s_data->max_shape)
    {
      flush_rectangle();
    }

    vec2f size = rect.size * transform.scale;
    vec2f origin = transform.origin * transform.scale;

    vec4f fill_color;
    rect.fill_color.normalize_color(fill_color);

    float rotation = glm::radians(transform.rotation);
    float cosA = cos(rotation);
    float sinA = sin(rotation);

    for (int i = 0; i < 4; i++)
    {
      vec2f local_pos = s_quad_vertex[i] * size - origin;
      vec2f rotated;
      rotated.x = local_pos.x * cosA - local_pos.y * sinA;
      rotated.y = local_pos.x * sinA + local_pos.y * cosA;
      rotated += transform.position;
      s_data->rectangle_vb_ptr->position = rotated;
      s_data->rectangle_vb_ptr->fill_color = fill_color;
      ++s_data->rectangle_vb_ptr;
    }
    s_data->rectangle_index++;
  }

  void Renderer2D::draw_circle(const Circle &circle, const Transform& transform)
  {
    if (s_data->circle_index >= s_data->max_shape)
    {
      flush_circle();
    }

    vec4f color;
    circle.fill_color.normalize_color(color);

    vec2f size = circle.size * transform.scale;
    vec2f origin = transform.origin * transform.scale;

    vec2f center = transform.position - transform.origin + size / 2;

    float rotation = glm::radians(transform.rotation);
    float cosA = cos(rotation);
    float sinA = sin(rotation);

    for (int i = 0; i < 4; i++)
    {
        vec2f local_pos = s_quad_vertex[i] * size - origin;
        vec2f rotated;
        rotated.x = local_pos.x * cosA - local_pos.y * sinA;
        rotated.y = local_pos.x * sinA + local_pos.y * cosA;
        rotated += transform.position;

      s_data->circle_vb_ptr->position = rotated;
      s_data->circle_vb_ptr->center = center;
      s_data->circle_vb_ptr->radius = size / 2;
      s_data->circle_vb_ptr->fill_color = color;
      ++s_data->circle_vb_ptr;
    }
    s_data->circle_index++;
  }

  void Renderer2D::draw_sprite(const Sprite &sprite, const Transform &transform)
  {
    if (s_data->sprite_index >= s_data->max_shape)
    {
      flush_sprite();
    }

    vec2u tex_size = s_data->texture->get_size();
    vec2u tex_pos = sprite.texture_rect.position;
    vec2u tex_dim = sprite.texture_rect.size;

    auto u0 = float(tex_pos.x) / tex_size.x;
    auto v0 = float(tex_pos.y) / tex_size.y;
    auto u1 = float(tex_pos.x + tex_dim.x) / tex_size.x;
    auto v1 = float(tex_pos.y + tex_dim.y) / tex_size.y;
    std::array<vec2f, 4> t = {vec2f(u0, v0), vec2f(u0, v1), vec2f(u1, v1), vec2f(u1, v0)};

    vec2f size = (sprite.size == 0) ? tex_dim : sprite.size;

    size *= transform.scale;
    float rotation = glm::radians(transform.rotation);
    float cosA = cos(rotation);
    float sinA = sin(rotation);
    for (int i = 0; i < 4; i++)
    {
      vec2f local_pos = s_quad_vertex[i] * size - transform.origin;
      vec2f rotated;
      rotated.x = local_pos.x * cosA - local_pos.y * sinA;
      rotated.y = local_pos.x * sinA + local_pos.y * cosA;
      rotated += transform.position;

      s_data->sprite_vb_ptr->position = {rotated.x, rotated.y};
      s_data->sprite_vb_ptr->tex_coord = {t[i].x, t[i].y};
      ++s_data->sprite_vb_ptr;
    }
    s_data->sprite_index++;
  }

  void Renderer2D::flush_rectangle()
  {
    if (s_data->rectangle_index == 0)
      return;

    AG_ref<VertexBuffer> vertexbuffer = s_data->rect_vertexarray->get_vertex_buffer();
    vertexbuffer->set_data(s_data->rectangle_vb_base, s_data->rectangle_index * 4 * sizeof(Rectangle_Data));

    AG_ref<IndexBuffer> indexbuffer = s_data->rect_vertexarray->get_index_buffer();
    indexbuffer->set_count(s_data->rectangle_index * 6);

    Renderer::submit(s_data->rect_shader, s_data->rect_vertexarray);

    s_data->rectangle_index = 0;
    s_data->rectangle_vb_ptr = s_data->rectangle_vb_base;
  }

  void Renderer2D::flush_circle()
  {
    if (s_data->circle_index == 0)
      return;

    AG_ref<VertexBuffer> vertexbuffer = s_data->circle_vertexarray->get_vertex_buffer();
    vertexbuffer->set_data(s_data->circle_vb_base, s_data->circle_index * 4 * sizeof(Circle_Data));

    AG_ref<IndexBuffer> indexbuffer = s_data->circle_vertexarray->get_index_buffer();
    indexbuffer->set_count(s_data->circle_index * 6);

    Renderer::submit(s_data->circle_shader, s_data->circle_vertexarray);

    s_data->circle_index = 0;
    s_data->circle_vb_ptr = s_data->circle_vb_base;
  }

  void Renderer2D::flush_sprite()
  {
    if (s_data->sprite_index == 0)
      return;

    AG_ref<VertexBuffer> vertexbuffer = s_data->sprite_vertexarray->get_vertex_buffer();
    vertexbuffer->set_data(s_data->sprite_vb_base, s_data->sprite_index * 4 * sizeof(Sprite_Data));

    AG_ref<IndexBuffer> indexbuffer = s_data->sprite_vertexarray->get_index_buffer();
    indexbuffer->set_count(s_data->sprite_index * 6);

    s_data->texture->bind();
    Renderer::submit(s_data->sprite_shader, s_data->sprite_vertexarray);

    s_data->sprite_index = 0;
    s_data->sprite_vb_ptr = s_data->sprite_vb_base;
  }
}
