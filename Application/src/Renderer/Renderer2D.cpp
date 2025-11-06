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

  struct Rectangle_Instance
  {
    vec2f size;
    vec2f position;
    vec2f origin;
    float rotation;
    float border_thickness;
    vec4f fill_color;
    vec4f border_color;
  };

  struct Circle_Instance
  {
    vec2f size;
    vec2f position;
    vec2f origin;
    float rotation;
    float border_thickness;
    vec4f fill_color;
    vec4f border_color;
  };

  struct Sprite_Instance
  {
    vec2f size;
    vec2f position;
    float rotation;
    vec2f texture_size;
    vec4f texture_rect;
  };

  struct Renderer2D_Data
  {
    // Max Shapes Data
    AG_uint max_shape = 0;
    AG_uint max_vertices = 0;
    AG_uint max_indices = 0;

    // Rectangle Data
    AG_ref<VertexArray> rect_vertex_array;
    AG_ref<VertexBuffer> rect_instanced_buffer;
    AG_ref<Shader> rect_shader;
    AG_uint rectangle_index = 0;
    Rectangle_Instance *rect_instanced_base = nullptr;
    Rectangle_Instance *rect_instanced_ptr = nullptr;

    // Circle Data
    AG_ref<VertexArray> circle_vertex_array;
    AG_ref<VertexBuffer> circle_instanced_buffer;
    AG_ref<Shader> circle_shader;
    AG_uint circle_index = 0;
    Circle_Instance *circle_instanced_base = nullptr;
    Circle_Instance*circle_instanced_ptr = nullptr;

    // Sprite Data
    AG_ref<VertexArray> sprite_vertex_array;
    AG_ref<VertexBuffer> sprite_instanced_buffer;
    AG_ref<Shader> sprite_shader;
    AG_ref<Texture> texture;
    AG_uint sprite_index = 0;
    Sprite_Instance *sprite_instanced_base = nullptr;
    Sprite_Instance *sprite_instanced_ptr = nullptr;

    View view;

    AG_uint total_quads = 0;
  };
  static Renderer2D_Data *s_data;

  void Renderer2D::init()
  {

    s_data = new Renderer2D_Data();
    s_data->max_shape = 10000;
    s_data->max_vertices = s_data->max_shape * 4;
    s_data->max_indices = s_data->max_shape * 6;


    AG_uint indices[] = { 0, 1, 2, 0, 2, 3 };

    AG_ref<IndexBuffer> indexbuffer = ag::IndexBuffer::create(indices, 6);


    float vertices[] = {
      0.f, 0.f,
      0.f, 1.f,
      1.f, 1.f,
      1.f, 0.f
    };

    {
      s_data->rect_instanced_base = new Rectangle_Instance[s_data->max_vertices];

      s_data->rect_vertex_array = ag::VertexArray::create();

      // Setup Rectangle
      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position"} };

      auto rect_vertex_buffer = ag::VertexBuffer::create(vertices, sizeof(vertices));
      rect_vertex_buffer->set_layout(layout);
      s_data->rect_vertex_array->add_vertex_buffer(rect_vertex_buffer);

      BufferLayout instance_layout = {
          {ShaderDataType::Float2, "a_InstanceSize"},
          {ShaderDataType::Float2, "a_InstancePos"},
          {ShaderDataType::Float2, "a_InstanceOrigin"},
          {ShaderDataType::Float, "a_InstanceRotation"},
          {ShaderDataType::Float, "a_BorderThickness"},
          {ShaderDataType::Float4, "a_FillColor"},
          {ShaderDataType::Float4, "a_BorderColor"},
      };
      s_data->rect_instanced_buffer = VertexBuffer::create(nullptr, sizeof(Rectangle_Instance) * s_data->max_vertices);
      s_data->rect_instanced_buffer->set_layout(instance_layout);

      s_data->rect_vertex_array->add_vertex_buffer(s_data->rect_instanced_buffer, true);
      s_data->rect_vertex_array->set_index_buffer(indexbuffer);
      s_data->rect_shader = ag::Shader::create("assets/shaders/Rectangle2D.glsl");
    }

    {
      BufferLayout vertex_layout = {
          {ShaderDataType::Float2, "a_Position"} };

      s_data->circle_instanced_base = new Circle_Instance[s_data->max_vertices];

      s_data->circle_vertex_array = VertexArray::create();

      auto circle_vertex_buffer = VertexBuffer::create(vertices, sizeof(vertices));
      circle_vertex_buffer->set_layout(vertex_layout);
      s_data->circle_vertex_array->add_vertex_buffer(circle_vertex_buffer);

      BufferLayout instance_layout = {
          {ShaderDataType::Float2, "a_InstanceSize"},
          {ShaderDataType::Float2, "a_InstancePos"},
          {ShaderDataType::Float2, "a_InstanceOrigin"},
          {ShaderDataType::Float, "a_InstanceRotation"},
          {ShaderDataType::Float, "a_BorderThickness"},
          {ShaderDataType::Float4, "a_FillColor"},
          {ShaderDataType::Float4, "a_BorderColor"},
      };

      s_data->circle_instanced_buffer = VertexBuffer::create(nullptr, s_data->max_vertices * sizeof(Circle_Instance));
      s_data->circle_instanced_buffer->set_layout(instance_layout);
      s_data->circle_vertex_array->add_vertex_buffer(s_data->circle_instanced_buffer, true);
      s_data->circle_vertex_array->set_index_buffer(indexbuffer);

      s_data->circle_shader = ag::Shader::create("assets/shaders/Circle2D.glsl");
    }

    {
      float sprite_vertices[] = {
      0.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 1.f,
      1.f, 1.f, 1.f, 1.f,
      1.f, 0.f, 1.f, 0.f
      };
      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position"},
          {ShaderDataType::Float2, "a_TexCoord"}};

      s_data->sprite_instanced_base = new Sprite_Instance[s_data->max_vertices];

      s_data->sprite_vertex_array = VertexArray::create();

      auto sprite_vertex_buffer = VertexBuffer::create(sprite_vertices, sizeof(sprite_vertices));
      sprite_vertex_buffer->set_layout(layout);
      s_data->sprite_vertex_array->add_vertex_buffer(sprite_vertex_buffer);

      BufferLayout instance_layout = {
         {ShaderDataType::Float2, "a_InstanceSize"},
         {ShaderDataType::Float2, "a_InstancePos"},
         {ShaderDataType::Float, "a_InstanceRotation"},
         {ShaderDataType::Float2, "a_Texture_Size"},
         {ShaderDataType::Float4, "a_TextureRect"},
      };
      s_data->sprite_instanced_buffer = VertexBuffer::create(nullptr, sizeof(Sprite_Instance) * s_data->max_vertices);
      s_data->sprite_instanced_buffer->set_layout(instance_layout);
      s_data->sprite_vertex_array->add_vertex_buffer(s_data->sprite_instanced_buffer, true);

      s_data->sprite_vertex_array->set_index_buffer(indexbuffer);

      s_data->sprite_shader = ag::Shader::create("assets/shaders/Sprite2D.glsl");
    }

    s_data->texture = ag::Texture2D::create("assets/textures/default.png");
    s_data->sprite_shader->bind();
    s_data->sprite_shader->set_int("u_texture", 0);
  }

  void Renderer2D::shut_down()
  {
    if (s_data)
    {
      if (s_data->rect_instanced_base)
        delete[] s_data->rect_instanced_base;

      if (s_data->circle_instanced_base)
        delete[] s_data->circle_instanced_base;

      if (s_data->sprite_instanced_base)
        delete[] s_data->sprite_instanced_base;

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
    s_data->total_quads = 0;
    flush();
  }

  void Renderer2D::start_batch()
  {
    s_data->rectangle_index = 0;
    s_data->rect_instanced_ptr = s_data->rect_instanced_base;

    s_data->circle_index = 0;
    s_data->circle_instanced_ptr = s_data->circle_instanced_base;

    s_data->sprite_index = 0;
    s_data->sprite_instanced_ptr = s_data->sprite_instanced_base;
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

  void Renderer2D::draw_rectangle(const Rectangle &rect, const Transform &transform)
  {
    // bool inside_view = s_data->view.get_float_rect().intersects(rect.get_float_rect());
    // if(!inside_view) return;

    if (s_data->rectangle_index >= s_data->max_shape)
    {
      flush_rectangle();
    }
    {
      Rectangle_Instance* instance = s_data->rect_instanced_ptr++;
      instance->size = rect.size * transform.scale;
      instance->position = transform.position;
      instance->origin = instance->size / 2;
      instance->rotation = Math::to_radians(transform.rotation);
      instance->border_thickness = rect.border_thickness;
      rect.fill_color.normalize_color(instance->fill_color);
      rect.border_color.normalize_color(instance->border_color);
    }
    s_data->rectangle_index++;
  }

  void Renderer2D::draw_circle(const Circle &circle, const Transform &transform)
  {
    if (s_data->circle_index >= s_data->max_shape)
    {
      flush_circle();
    }
    {
      Circle_Instance* instance = s_data->circle_instanced_ptr++;
      instance->size = circle.size * transform.scale;
      instance->position = transform.position;
      instance->origin = instance->size / 2;
      instance->rotation = Math::to_radians(transform.rotation);
      instance->border_thickness = circle.border_thickness;
      circle.fill_color.normalize_color(instance->fill_color);
      circle.border_color.normalize_color(instance->border_color);
    }
    s_data->circle_index++;
  }

  void Renderer2D::draw_sprite(const Sprite &sprite, const Transform &transform)
  {
    if (s_data->sprite_index >= s_data->max_shape)
    {
      flush_sprite();
    }
    {
      Sprite_Instance* instance = s_data->sprite_instanced_ptr++;
      instance->texture_size = s_data->texture->get_size();
      instance->position = transform.position;
      instance->rotation = Math::to_radians(transform.rotation);
      instance->size = sprite.size * transform.scale;
      sprite.texture_rect.to_vec4(instance->texture_rect);
    }
    s_data->sprite_index++;
  }

  void Renderer2D::flush_rectangle()
  {
    if (s_data->rectangle_index == 0)
      return;

    size_t data_size = (uint8_t*)s_data->rect_instanced_ptr - (uint8_t*)s_data->rect_instanced_base;
    s_data->rect_instanced_buffer->set_data(s_data->rect_instanced_base, data_size);

    Renderer::submit_instanced(s_data->rect_shader, s_data->rect_vertex_array, s_data->rectangle_index);

    s_data->rectangle_index = 0;
    s_data->rect_instanced_ptr = s_data->rect_instanced_base;
  }

  void Renderer2D::flush_circle()
  {
    if (s_data->circle_index == 0)
      return;

    size_t data_size = (uint8_t*)s_data->circle_instanced_ptr - (uint8_t*)s_data->circle_instanced_base;
    s_data->circle_instanced_buffer->set_data(s_data->circle_instanced_base, data_size);

    Renderer::submit_instanced(s_data->circle_shader, s_data->circle_vertex_array, s_data->circle_index);
    s_data->circle_index = 0;
    s_data->circle_instanced_ptr = s_data->circle_instanced_base;
  }

  void Renderer2D::flush_sprite()
  {
    if (s_data->sprite_index == 0)
      return;

    size_t data_size = (uint8_t*)s_data->sprite_instanced_ptr - (uint8_t*)s_data->sprite_instanced_base;
    s_data->sprite_instanced_buffer->set_data(s_data->sprite_instanced_base, data_size);

    s_data->texture->bind();
    Renderer::submit_instanced(s_data->sprite_shader, s_data->sprite_vertex_array, s_data->sprite_index);

    s_data->sprite_index = 0;
    s_data->sprite_instanced_ptr = s_data->sprite_instanced_base;
  }
}
