#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <Core/Application.hpp>
#include <Font/FontManager.hpp>
#include <GameObjects/Components/Components.hpp>
#include <glm/glm.hpp>
#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Text.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Scene/SceneComponent.hpp>

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
  #include <malloc.h>
#endif

namespace ag
{

enum class Quad_Type
{
  Rectangle = 0,
  Circle    = 1,
  Sprite    = 2,
  Text      = 3
};

struct Quad_Instance
{
  vec2f size;
  vec2f position;
  vec2f origin;
  float rotation;
  int   mode;
  int   quad_mode;
  int   texture_slot;

  float border_thickness;
  vec4f fill_color;
  vec4f border_color;

  float corner_radius;

  vec2f texture_size;
  vec4f texture_rect;
  vec2f flip;
  int   entity_id;
};

constexpr int TEXTURE_TEXT   = 0;
constexpr int TEXTURE_SCENE  = 2;
constexpr int TEXTURE_SPRITE = 1;

using AsyncData      = Renderer2D::AsyncData;
using ProcessedBatch = Renderer2D::ProcessedBatch;

struct Renderer2D_Data
{
  AG_uint max_shape    = 0;
  AG_uint max_vertices = 0;
  AG_uint max_indices  = 0;

  AG_ref<VertexArray>  quad_vertex_array;
  AG_ref<VertexBuffer> quad_instanced_buffer;
  AG_ref<Shader>       quad_shader;
  AG_ref<Shader>       text_shader;
  AG_ref<Texture>      quad_texture, text_texture;
  std::string          active_font_path;
  AG_ref<Texture>      active_font_texture;

  std::atomic<int> text_render_mode{static_cast<int>(Renderer2D::TextRenderMode::Combined)};

  AG_ref<VertexArray>  fullscreen_vertex_array;
  AG_ref<VertexBuffer> fullscreen_instanced_buffer;
  AG_ref<Shader>       fullscreen_shader;

  View    view;
  vec2f   viewport_size;
  AG_uint slots;

  std::unique_ptr<AsyncData> async_data;  // AsyncData is public in header
};

static Renderer2D_Data* s_data = nullptr;

static void maybe_notify()
{
  static thread_local int counter = 0;
  if (++counter >= 1000)
  {
    s_data->async_data->command_cv.notify_one();
    counter = 0;
  }
}

void Renderer2D::init()
{
  s_data               = new Renderer2D_Data();
  s_data->max_shape    = 100000;

  // Optional runtime override so both text paths can be tried without code
  // changes: AERO_TEXT_RENDER=combined (default) or independent.
  if (const char* text_mode = std::getenv("AERO_TEXT_RENDER"))
  {
    const std::string mode(text_mode);
    if (mode == "independent" || mode == "new" || mode == "1")
    {
      s_data->text_render_mode.store(static_cast<int>(TextRenderMode::Independent));
    }
  }
  s_data->max_vertices = s_data->max_shape * 4;
  s_data->max_indices  = s_data->max_shape * 6;
  s_data->slots        = 3;

  AG_uint             indices[]   = {0, 1, 2, 0, 2, 3};
  AG_ref<IndexBuffer> indexbuffer = ag::IndexBuffer::create(indices, 6);
  float               vertices[]  = {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f};

  {
    float full_vertices[] =
        {0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 1.f};
    BufferLayout layout = {
        {ShaderDataType::Float2, "a_Position"},
        {ShaderDataType::Float2, "a_texcoord"}
    };
    s_data->fullscreen_vertex_array = VertexArray::create();
    auto vb                         = VertexBuffer::create(full_vertices, sizeof(full_vertices));
    vb->set_layout(layout);
    s_data->fullscreen_vertex_array->add_vertex_buffer(vb);
    s_data->fullscreen_vertex_array->set_index_buffer(indexbuffer);
    if (Renderer::get_API() == RendererAPI::API::OpenGL_ES)
      s_data->fullscreen_shader = ag::Shader::create("shaders/Sprite2DES.glsl");
    else
      s_data->fullscreen_shader = ag::Shader::create("shaders/Sprite2D.glsl");
  }

  {
    s_data->quad_vertex_array = ag::VertexArray::create();

    BufferLayout layout = {
        {ShaderDataType::Float2, "a_Position"}
    };
    auto quad_vb = ag::VertexBuffer::create(vertices, sizeof(vertices));
    quad_vb->set_layout(layout);
    s_data->quad_vertex_array->add_vertex_buffer(quad_vb);

    BufferLayout instance_layout = {
        {ShaderDataType::Float2,     "a_InstanceSize"},
        {ShaderDataType::Float2,      "a_InstancePos"},
        {ShaderDataType::Float2,   "a_InstanceOrigin"},
        { ShaderDataType::Float, "a_InstanceRotation"},
        {   ShaderDataType::Int,       "a_RenderMode"},
        {   ShaderDataType::Int,         "a_QuadMode"},
        {   ShaderDataType::Int,      "a_TextureSlot"},
        { ShaderDataType::Float,  "a_BorderThickness"},
        {ShaderDataType::Float4,        "a_FillColor"},
        {ShaderDataType::Float4,      "a_BorderColor"},
        { ShaderDataType::Float,     "a_CornerRadius"},
        {ShaderDataType::Float2,     "a_Texture_Size"},
        {ShaderDataType::Float4,      "a_TextureRect"},
        {ShaderDataType::Float2,             "a_flip"},
        {   ShaderDataType::Int,        "a_entity_id"},
    };

    s_data->quad_instanced_buffer = VertexBuffer::create(sizeof(Quad_Instance) * s_data->max_shape);
    s_data->quad_instanced_buffer->set_layout(instance_layout);
    s_data->quad_vertex_array->add_vertex_buffer(s_data->quad_instanced_buffer, true);
    s_data->quad_vertex_array->set_index_buffer(indexbuffer);
    if (Renderer::get_API() == RendererAPI::API::OpenGL_ES)
    {
      s_data->quad_shader = ag::Shader::create("shaders/QuadES.glsl");
      s_data->text_shader = ag::Shader::create("shaders/TextES.glsl");
    }
    else
    {
      s_data->quad_shader = ag::Shader::create("shaders/Quad.glsl");
      s_data->text_shader = ag::Shader::create("shaders/Text.glsl");
    }

    s_data->quad_texture =
        ag::Texture2D::create("textures/default.png", true, Filter_Mode::AG_NEAREST, true);
    // MSDF atlases must not be mipmapped: averaging the RGB distance channels
    // blurs neighbouring glyphs together at minification.
    FontManager::init();
    auto default_font = FontManager::get_default();
    s_data->text_texture = default_font ? default_font->texture : nullptr;
    s_data->active_font_texture = s_data->text_texture;
    s_data->active_font_path.clear();
  }

  s_data->async_data = std::make_unique<AsyncData>();

  auto alloc_instances = [&](Quad_Instance*& instances)
  {
#ifdef _MSC_VER
    instances = (Quad_Instance*)_aligned_malloc(sizeof(Quad_Instance) * s_data->max_shape, 64);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_WEB)
    instances = (Quad_Instance*)memalign(64, sizeof(Quad_Instance) * s_data->max_shape);
#else
    instances = (Quad_Instance*)aligned_alloc(64, sizeof(Quad_Instance) * s_data->max_shape);
#endif
  };

  auto alloc_batch = [&](ProcessedBatch& b)
  {
    alloc_instances(b.instances);
    alloc_instances(b.text_instances);
    b.capacity      = s_data->max_shape;
    b.count         = 0;
    b.text_capacity = s_data->max_shape;
    b.text_count    = 0;
  };

  alloc_batch(s_data->async_data->batch_a);
  alloc_batch(s_data->async_data->batch_b);
  s_data->async_data->current_batch = &s_data->async_data->batch_a;

#ifndef PLATFORM_WEB
  s_data->async_data->worker_thread = std::thread(worker_thread_function);
#endif
}

void Renderer2D::shut_down()
{
  if (!s_data)
    return;

  if (s_data->async_data)
  {
    s_data->async_data->running = false;
    s_data->async_data->command_cv.notify_one();

#ifndef PLATFORM_WEB
    if (s_data->async_data->worker_thread.joinable())
      s_data->async_data->worker_thread.join();
#endif

#ifdef _MSC_VER
    _aligned_free(s_data->async_data->batch_a.instances);
    _aligned_free(s_data->async_data->batch_a.text_instances);
    _aligned_free(s_data->async_data->batch_b.instances);
    _aligned_free(s_data->async_data->batch_b.text_instances);
#else
    free(s_data->async_data->batch_a.instances);
    free(s_data->async_data->batch_a.text_instances);
    free(s_data->async_data->batch_b.instances);
    free(s_data->async_data->batch_b.text_instances);
#endif
  }

  FontManager::shut_down();

  delete s_data;
  s_data = nullptr;
}

void Renderer2D::begin_scene(const View& view, const vec2f& viewport_size)
{
  Renderer::begin_scene(view, viewport_size);
  s_data->view          = view;
  s_data->viewport_size = viewport_size;
  Renderer::enable_blend();
}

void Renderer2D::begin_scene()
{
}

void Renderer2D::end_scene()
{
  s_data->async_data->command_cv.notify_one();
  flush();
}

void Renderer2D::start_batch()
{
  s_data->async_data->command_cv.notify_one();
  flush();
}

void Renderer2D::set_texture(const AG_ref<Texture>& texture)
{
  if (!texture || s_data->quad_texture == texture)
    return;
  flush();
  s_data->quad_texture = texture;
}

void Renderer2D::set_text_render_mode(TextRenderMode mode)
{
  if (!s_data)
    return;
  s_data->text_render_mode.store(static_cast<int>(mode));
}

Renderer2D::TextRenderMode Renderer2D::get_text_render_mode()
{
  if (!s_data)
    return TextRenderMode::Combined;
  return static_cast<TextRenderMode>(s_data->text_render_mode.load());
}

void Renderer2D::draw_fullscreen_quad(AG_uint id, const vec2f& viewport, const vec2f& size)
{
  auto screen_matrix = Math::get_view_matrix(viewport, viewport / 2.0f);
  Renderer::disable_blend();
  Renderer::bind(id, TEXTURE_SCENE);
  s_data->fullscreen_shader->bind();
  s_data->fullscreen_shader->set_vec2f("u_size", size);
  s_data->fullscreen_shader->set_vec2f("u_position", viewport / 2.0f);
  s_data->fullscreen_shader->set_mat3("u_screen_matrix", screen_matrix);
  s_data->fullscreen_shader->set_int("u_texture", TEXTURE_SCENE);
  Renderer::submit(s_data->fullscreen_vertex_array);

  // Unbind the scene texture once it has been sampled. Otherwise the next frame
  // binds the framebuffer that owns it while the texture is still attached to
  // the active unit, which GLES3/WebGL2 report as a framebuffer/texture feedback
  // loop.
  Renderer::bind(0, TEXTURE_SCENE);
}

void Renderer2D::draw_rectangle(const Rectangle&           rect,
                                const Transform_Component& transform,
                                int                        entity_id)
{
  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    s_data->async_data->command_queue.push(RectangleCommand{rect, transform, entity_id});
    s_data->async_data->total_commands++;
  }
  maybe_notify();
}

void Renderer2D::draw_circle(const Circle&              circle,
                             const Transform_Component& transform,
                             int                        entity_id)
{
  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    s_data->async_data->command_queue.push(CircleCommand{circle, transform, entity_id});
    s_data->async_data->total_commands++;
  }
  maybe_notify();
}

void Renderer2D::draw_sprite(const Sprite&              sprite,
                             const Transform_Component& transform,
                             int                        entity_id)
{
  vec2f tex_size = s_data->quad_texture ? vec2f(s_data->quad_texture->get_size()) : vec2f(0, 0);
  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    s_data->async_data->command_queue.push(SpriteCommand{sprite, transform, entity_id, tex_size});
    s_data->async_data->total_commands++;
  }
  maybe_notify();
}

void Renderer2D::draw_text(const Text&                text_string,
                           const Transform_Component& transform,
                           int                        entity_id)
{
  auto font = FontManager::get(text_string.font_path);
  if (!font || !font->texture || !font->metrics)
    return;

  // The renderer has one glyph texture slot. Finish the current batch before
  // switching atlases so every instance in that batch samples the right PNG.
  if (font->path != s_data->active_font_path)
  {
    flush();
    s_data->active_font_path    = font->path;
    s_data->active_font_texture = font->texture;
  }

  TextCommand cmd;
  cmd.text          = text_string;
  cmd.transform     = transform;
  cmd.entity_id     = entity_id;
  cmd.font          = font;
  cmd.longest_size  = Text::calc_longest_size(text_string, transform.scale, font->metrics.get());
  cmd.viewport_size = s_data->viewport_size;
  cmd.view          = s_data->view;

  if (entity_id >= 0)
  {
    Entity entity((entt::entity)(entity_id));
    if (entity && entity.has_component<Text_Editor_State>())
    {
      auto& state = entity.get_component<Text_Editor_State>();
      if (state.active)
      {
        state.blink_timer += Application::get().delta_time;
        cmd.has_cursor     = true;
        cmd.cursor_visible = fmod(state.blink_timer, 1.0f) < 0.6f;
        cmd.caret_index    = state.caret_index;
      }
    }
  }

  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    s_data->async_data->command_queue.push(std::move(cmd));
    s_data->async_data->total_commands++;
  }
  maybe_notify();
}

void Renderer2D::flush()
{
  if (s_data->async_data->total_commands == 0)
    return;

#ifdef PLATFORM_WEB
  // Web builds run on a single thread (no pthread worker). Process the queued
  // commands directly and upload the result synchronously.
  ProcessedBatch* batch = &s_data->async_data->batch_a;
  batch->clear();

  std::queue<DrawCommand> local;
  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    local.swap(s_data->async_data->command_queue);
  }

  while (!local.empty())
  {
    DrawCommand cmd = std::move(local.front());
    local.pop();

    std::visit(
        [batch](auto&& arg)
        {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, RectangleCommand>)
            process_rectangle(arg, *batch);
          else if constexpr (std::is_same_v<T, CircleCommand>)
            process_circle(arg, *batch);
          else if constexpr (std::is_same_v<T, SpriteCommand>)
            process_sprite(arg, *batch);
          else if constexpr (std::is_same_v<T, TextCommand>)
            process_text(arg, *batch);
        },
        cmd);
  }

  flush_internal(batch);
  s_data->async_data->total_commands = 0;
  return;
#else
  {
    std::lock_guard<std::mutex> lk(s_data->async_data->command_mutex);
    s_data->async_data->flush_requested = true;
  }
  s_data->async_data->command_cv.notify_one();

  ProcessedBatch* batch = nullptr;
  {
    std::unique_lock<std::mutex> lk(s_data->async_data->processed_mutex);
    bool                         ok = s_data->async_data->processed_cv.wait_for(
        lk,
        std::chrono::seconds(5),
        [] { return s_data->async_data->ready_batch.load() != nullptr; });

    if (!ok)
      return;  // 5 s safety timeout
    batch = s_data->async_data->ready_batch.exchange(nullptr);
  }

  flush_internal(batch);
  s_data->async_data->total_commands = 0;
#endif
}

void Renderer2D::worker_thread_function()
{
  ProcessedBatch* working = &s_data->async_data->batch_a;
  ProcessedBatch* standby = &s_data->async_data->batch_b;

  std::vector<DrawCommand> local;
  local.reserve(100000);

  while (s_data->async_data->running)
  {
    {
      std::unique_lock<std::mutex> lk(s_data->async_data->command_mutex);
      s_data->async_data->command_cv.wait_for(lk,
                                              std::chrono::milliseconds(1),
                                              []
                                              {
                                                return !s_data->async_data->command_queue.empty() ||
                                                       s_data->async_data->flush_requested ||
                                                       !s_data->async_data->running;
                                              });

      if (!s_data->async_data->running)
        break;

      local.clear();
      while (!s_data->async_data->command_queue.empty())
      {
        local.push_back(std::move(s_data->async_data->command_queue.front()));
        s_data->async_data->command_queue.pop();
      }
    }

    bool should_flush = s_data->async_data->flush_requested.exchange(false);

    for (auto& cmd : local)
    {
      std::visit(
          [working](auto&& arg)
          {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, RectangleCommand>)
              process_rectangle(arg, *working);
            else if constexpr (std::is_same_v<T, CircleCommand>)
              process_circle(arg, *working);
            else if constexpr (std::is_same_v<T, SpriteCommand>)
              process_sprite(arg, *working);
            else if constexpr (std::is_same_v<T, TextCommand>)
              process_text(arg, *working);
          },
          cmd);
    }

    if (should_flush)
    {
      {
        std::lock_guard<std::mutex> lk(s_data->async_data->processed_mutex);
        s_data->async_data->ready_batch.store(working);
      }
      s_data->async_data->processed_cv.notify_one();

      std::swap(working, standby);
      working->clear();
    }
  }
}

void Renderer2D::process_rectangle(const RectangleCommand& cmd, ProcessedBatch& batch)
{
  if (batch.count >= batch.capacity)
    return;

  Quad_Instance* __restrict inst = &batch.instances[batch.count++];

  const vec2f scaled = cmd.rect.size * cmd.transform.scale;
  const float avg    = (cmd.transform.scale.x + cmd.transform.scale.y) * 0.5f;

  inst->size             = scaled;
  inst->position         = cmd.transform.position;
  inst->origin           = scaled * 0.5f;
  inst->rotation         = Math::to_radians(cmd.transform.rotation);
  inst->mode             = static_cast<int>(cmd.rect.mode);
  inst->quad_mode        = static_cast<int>(Quad_Type::Rectangle);
  inst->border_thickness = cmd.rect.border_thickness * avg;
  cmd.rect.fill_color.normalize_color(inst->fill_color);
  cmd.rect.border_color.normalize_color(inst->border_color);
  inst->corner_radius = cmd.rect.corner_radius * avg;
  inst->entity_id     = cmd.entity_id;
}

void Renderer2D::process_circle(const CircleCommand& cmd, ProcessedBatch& batch)
{
  if (batch.count >= batch.capacity)
    return;

  Quad_Instance* __restrict inst = &batch.instances[batch.count++];

  const vec2f scaled = cmd.circle.size * cmd.transform.scale;
  const float avg    = (cmd.transform.scale.x + cmd.transform.scale.y) * 0.5f;

  inst->size             = scaled;
  inst->position         = cmd.transform.position;
  inst->origin           = scaled * 0.5f;
  inst->rotation         = Math::to_radians(cmd.transform.rotation);
  inst->mode             = static_cast<int>(cmd.circle.mode);
  inst->quad_mode        = static_cast<int>(Quad_Type::Circle);
  inst->border_thickness = cmd.circle.border_thickness * avg;
  cmd.circle.fill_color.normalize_color(inst->fill_color);
  cmd.circle.border_color.normalize_color(inst->border_color);
  inst->entity_id = cmd.entity_id;
}

void Renderer2D::process_sprite(const SpriteCommand& cmd, ProcessedBatch& batch)
{
  if (batch.count >= batch.capacity)
    return;

  Quad_Instance* __restrict inst = &batch.instances[batch.count++];

  const vec2f scaled = cmd.sprite.size * cmd.transform.scale;

  inst->size         = scaled;
  inst->position     = cmd.transform.position;
  inst->origin       = scaled * 0.5f;
  inst->rotation     = Math::to_radians(cmd.transform.rotation);
  inst->mode         = static_cast<int>(cmd.sprite.mode);
  inst->quad_mode    = static_cast<int>(Quad_Type::Sprite);
  inst->texture_slot = TEXTURE_SPRITE;
  cmd.sprite.fill_color.normalize_color(inst->fill_color);
  inst->texture_size = cmd.texture_size;
  cmd.sprite.texture_rect.to_vec4(inst->texture_rect);

  if (cmd.sprite.texture_rect.size == 0)
  {
    inst->texture_rect.z = inst->texture_size.x;
    inst->texture_rect.w = inst->texture_size.y;
  }

  inst->flip.x    = cmd.sprite.flip_horizontal ? -1.0f : 1.0f;
  inst->flip.y    = cmd.sprite.flip_vertical ? -1.0f : 1.0f;
  inst->entity_id = cmd.entity_id;
}

void Renderer2D::process_text(const TextCommand& cmd, ProcessedBatch& batch)
{
  if (cmd.font && cmd.font->texture)
  {
    batch.font_texture = cmd.font->texture;
    batch.font_path    = cmd.font->path;
  }

  if (s_data->text_render_mode.load() == static_cast<int>(TextRenderMode::Independent))
    process_text_independent(cmd, batch);
  else
    process_text_combined(cmd, batch);
}

void Renderer2D::process_text_combined(const TextCommand& cmd, ProcessedBatch& batch)
{
  const TextLoader::Font& metrics = cmd.font && cmd.font->metrics
                                        ? *cmd.font->metrics
                                        : *FontManager::get_default()->metrics;

  size_t index       = 0;
  size_t break_index = 0;

  Transform_Component trans;
  trans.position = cmd.transform.position - (cmd.longest_size * 0.5);
  trans.scale    = cmd.transform.scale;
  trans.rotation = cmd.transform.rotation;

  const float scale_x = trans.scale.x * cmd.text.font_size / metrics.em_size;
  const float scale_y = trans.scale.y * cmd.text.font_size / metrics.em_size;
  const vec2f scale_vec(scale_x, scale_y);

  vec2f starting_pos =
      Text::center_single_line_text(cmd.text, trans, cmd.longest_size, index, &break_index, &metrics);
  const vec2f padding = cmd.text.padding * scale_vec;
  starting_pos += padding;

  const float ascender = metrics.ascender * metrics.em_size * scale_y;
  const float line_height =
      metrics.line_height * metrics.em_size * scale_y * cmd.text.line_height;

  starting_pos.y += ascender;
  float base_line = starting_pos.y;

  auto space_it = metrics.glyphs.find(' ');
  if (space_it == metrics.glyphs.end())
    return;

  const float tab_advance = space_it->second.advance * scale_x * 4.0f;

  vec4f norm_color;
  cmd.text.text_color.normalize_color(norm_color);

  const float angle           = Math::to_radians(cmd.transform.rotation);
  const vec2f rotation_center = cmd.transform.position;
  const float cos_angle       = std::cos(angle);
  const float sin_angle       = std::sin(angle);

  auto rotate_point = [&](const vec2f& point)
  {
    const vec2f local = point - rotation_center;
    return vec2f(local.x * cos_angle - local.y * sin_angle,
                 local.x * sin_angle + local.y * cos_angle) +
           rotation_center;
  };

  if (cmd.text.draw_rect)
  {
    Rectangle rect;
    rect.size         = cmd.longest_size + (padding * 2);
    rect.fill_color   = Color::Transparent;
    rect.mode         = cmd.text.mode;
    rect.border_color = cmd.text.text_color;

    vec2f one_px =
        Math::screen_size_to_world_size({1.f, 1.f}, cmd.view.get_size(), cmd.viewport_size);
    rect.border_thickness = one_px.x;

    Transform_Component rt;
    rt.position = rotate_point(trans.position + rect.size * 0.5f);
    rt.rotation = cmd.transform.rotation;

    process_rectangle(RectangleCommand{rect, rt, cmd.entity_id}, batch);
  }

  const size_t text_length = cmd.text.text.size();

  for (; index < text_length; ++index)
  {
    const char c = cmd.text.text[index];

    if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == index)
    {
      vec2f csz  = {6.0f * scale_x,
                    metrics.line_height * metrics.em_size * scale_y};
      vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

      Rectangle cr;
      cr.mode       = cmd.text.mode;
      cr.fill_color = cmd.text.text_color;
      vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
      cr.size       = {ws.x, csz.y};

      Transform_Component ct;
      ct.position = rotate_point({cpos.x + cr.size.x * 0.5f, cpos.y});
      ct.rotation = cmd.transform.rotation;

      process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
    }

    if (c == '\n')
    {
      size_t ni = index + 1;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index, &metrics);
      starting_pos += padding;
      starting_pos.y += ascender;
      starting_pos.y += line_height;
      base_line += line_height;
      continue;
    }

    if (c == '\t')
    {
      starting_pos.x += tab_advance;
      continue;
    }

    auto it = metrics.glyphs.find(c);
    if (it == metrics.glyphs.end())
      continue;

    const TextLoader::Glyph& g = it->second;

    if (cmd.longest_size.x > 0 && index == break_index && break_index != 0)
    {
      size_t ni = index;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index, &metrics);
      starting_pos += padding;
      starting_pos.y += ascender;
      starting_pos.y += line_height;
      base_line += line_height;

      if (c == ' ')
        continue;
    }

    if (batch.count >= batch.capacity)
      break;

    Quad_Instance* __restrict inst = &batch.instances[batch.count++];
    inst->texture_size             = cmd.font->texture->get_size();
    inst->size                     = g.texture_rect.size * scale_vec;
    // Combined text keeps a_position at the glyph's top-left corner. Use the
    // node position as the pivot so rotating the line spins all glyphs around
    // the text origin instead of around individual glyph centres.
    inst->origin                   = rotation_center;
    inst->rotation                 = angle;
    inst->border_thickness         = 0.0f;
    inst->border_color             = {0.0f, 0.0f, 0.0f, 0.0f};
    inst->corner_radius            = 0.0f;
    g.texture_rect.to_vec4(inst->texture_rect);
    inst->fill_color   = norm_color;
    inst->mode         = static_cast<int>(cmd.text.mode);
    inst->quad_mode    = static_cast<int>(Quad_Type::Text);
    inst->texture_slot = TEXTURE_TEXT;
    inst->flip         = {1.0f, 1.0f};
    inst->position.x   = starting_pos.x + g.plane_left * scale_x;
    inst->position.y   = starting_pos.y - (g.plane_top * scale_y);
    inst->entity_id    = cmd.entity_id;

    starting_pos.x += g.advance * scale_x;
  }

  if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == text_length)
  {
    vec2f csz = {6.0f * scale_x, metrics.line_height * metrics.em_size * scale_y};
    vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

    Rectangle cr;
    cr.mode       = cmd.text.mode;
    cr.fill_color = cmd.text.text_color;
    vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
    cr.size       = {ws.x, csz.y};

    Transform_Component ct;
    ct.position = rotate_point({cpos.x + cr.size.x * 0.5f, cpos.y});
    ct.rotation = cmd.transform.rotation;

    process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
  }
}

void Renderer2D::process_text_independent(const TextCommand& cmd, ProcessedBatch& batch)
{
  if (!batch.text_instances)
    return;

  const TextLoader::Font& metrics = cmd.font && cmd.font->metrics
                                        ? *cmd.font->metrics
                                        : *FontManager::get_default()->metrics;

  size_t index       = 0;
  size_t break_index = 0;

  Transform_Component trans;
  trans.position = cmd.transform.position - (cmd.longest_size * 0.5);
  trans.scale    = cmd.transform.scale;
  trans.rotation = cmd.transform.rotation;

  const float scale_x = trans.scale.x * cmd.text.font_size / metrics.em_size;
  const float scale_y = trans.scale.y * cmd.text.font_size / metrics.em_size;
  const vec2f scale_vec(scale_x, scale_y);

  vec2f starting_pos =
      Text::center_single_line_text(cmd.text, trans, cmd.longest_size, index, &break_index, &metrics);
  const vec2f padding = cmd.text.padding * scale_vec;
  starting_pos += padding;

  const float ascender = metrics.ascender * metrics.em_size * scale_y;
  const float line_height =
      metrics.line_height * metrics.em_size * scale_y * cmd.text.line_height;

  starting_pos.y += ascender;
  float base_line = starting_pos.y;

  auto space_it = metrics.glyphs.find(' ');
  if (space_it == metrics.glyphs.end())
    return;

  const float tab_advance = space_it->second.advance * scale_x * 4.0f;

  vec4f norm_color;
  cmd.text.text_color.normalize_color(norm_color);

  // Glyphs are placed in unrotated text space first, then the whole line is
  // rotated around the entity position so rotation matches every other node.
  const float angle           = Math::to_radians(cmd.transform.rotation);
  const vec2f rotation_center = cmd.transform.position;
  const float cos_angle       = std::cos(angle);
  const float sin_angle       = std::sin(angle);

  auto rotate_point = [&](const vec2f& point)
  {
    const vec2f local = point - rotation_center;
    return vec2f(local.x * cos_angle - local.y * sin_angle,
                 local.x * sin_angle + local.y * cos_angle) +
           rotation_center;
  };

  if (cmd.text.draw_rect)
  {
    Rectangle rect;
    rect.size         = cmd.longest_size + (padding * 2);
    rect.fill_color   = Color::Transparent;
    rect.mode         = cmd.text.mode;
    rect.border_color = cmd.text.text_color;

    vec2f one_px =
        Math::screen_size_to_world_size({1.f, 1.f}, cmd.view.get_size(), cmd.viewport_size);
    rect.border_thickness = one_px.x;

    Transform_Component rt;
    rt.position = rotate_point(trans.position + rect.size * 0.5f);
    rt.rotation = cmd.transform.rotation;

    process_rectangle(RectangleCommand{rect, rt, cmd.entity_id}, batch);
  }

  const size_t text_length = cmd.text.text.size();

  for (; index < text_length; ++index)
  {
    const char c = cmd.text.text[index];

    if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == index)
    {
      vec2f csz  = {6.0f * scale_x,
                    metrics.line_height * metrics.em_size * scale_y};
      vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

      Rectangle cr;
      cr.mode       = cmd.text.mode;
      cr.fill_color = cmd.text.text_color;
      vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
      cr.size       = {ws.x, csz.y};

      Transform_Component ct;
      ct.position = rotate_point({cpos.x + cr.size.x * 0.5f, cpos.y});
      ct.rotation = cmd.transform.rotation;

      process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
    }

    if (c == '\n')
    {
      size_t ni = index + 1;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index, &metrics);
      starting_pos += padding;
      starting_pos.y += ascender;
      starting_pos.y += line_height;
      base_line += line_height;
      continue;
    }

    if (c == '\t')
    {
      starting_pos.x += tab_advance;
      continue;
    }

    auto it = metrics.glyphs.find(c);
    if (it == metrics.glyphs.end())
      continue;

    const TextLoader::Glyph& g = it->second;

    if (cmd.longest_size.x > 0 && index == break_index && break_index != 0)
    {
      size_t ni = index;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index, &metrics);
      starting_pos += padding;
      starting_pos.y += ascender;
      starting_pos.y += line_height;
      base_line += line_height;

      if (c == ' ')
        continue;
    }

    if (batch.text_count >= batch.text_capacity)
      break;

    Quad_Instance* __restrict inst = &batch.text_instances[batch.text_count++];

    inst->texture_size = cmd.font->texture->get_size();
    inst->size         = g.texture_rect.size * scale_vec;
    inst->origin       = inst->size * 0.5f;
    inst->rotation     = angle;
    inst->mode         = static_cast<int>(cmd.text.mode);
    inst->quad_mode    = static_cast<int>(Quad_Type::Text);
    inst->texture_slot = TEXTURE_TEXT;

    // Keep UVs half a texel inside the glyph cell. With linear filtering this
    // stops adjacent atlas glyphs from bleeding into each other.
    constexpr float uv_inset = 0.5f;
    vec4f           uv;
    g.texture_rect.to_vec4(uv);
    uv.z = std::max(uv.z - uv_inset * 2.0f, 0.0f);
    uv.w = std::max(uv.w - uv_inset * 2.0f, 0.0f);
    inst->texture_rect = {uv.x + uv_inset, uv.y + uv_inset, uv.z, uv.w};

    inst->border_thickness = 0.0f;
    inst->fill_color       = norm_color;
    inst->border_color     = {0.0f, 0.0f, 0.0f, 0.0f};
    inst->corner_radius    = 0.0f;
    inst->flip             = {1.0f, 1.0f};

    const vec2f glyph_corner = {starting_pos.x + g.plane_left * scale_x,
                                starting_pos.y - (g.plane_top * scale_y)};
    inst->position           = rotate_point(glyph_corner + inst->size * 0.5f);
    inst->entity_id          = cmd.entity_id;

    starting_pos.x += g.advance * scale_x;
  }

  if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == text_length)
  {
    vec2f csz = {6.0f * scale_x, metrics.line_height * metrics.em_size * scale_y};
    vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

    Rectangle cr;
    cr.mode       = cmd.text.mode;
    cr.fill_color = cmd.text.text_color;
    vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
    cr.size       = {ws.x, csz.y};

    Transform_Component ct;
    ct.position = rotate_point({cpos.x + cr.size.x * 0.5f, cpos.y});
    ct.rotation = cmd.transform.rotation;

    process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
  }
}

void Renderer2D::flush_internal(ProcessedBatch* batch)
{
  if (!batch || batch->is_empty())
    return;

  auto draw_instances = [](Quad_Instance* instances, size_t count, const AG_ref<Shader>& shader)
  {
    constexpr size_t MAX_PER_DRAW = 65536;
    size_t           remaining    = count;
    size_t           offset       = 0;

    while (remaining > 0)
    {
      const size_t draw_count = std::min(remaining, MAX_PER_DRAW);
      const size_t data_size  = draw_count * sizeof(Quad_Instance);

      s_data->quad_instanced_buffer->set_data(&instances[offset], data_size);

      Renderer::submit_instanced(shader, s_data->quad_vertex_array, draw_count);

      offset += draw_count;
      remaining -= draw_count;
    }
  };

  const AG_ref<Texture> font_texture =
      batch->font_texture ? batch->font_texture : s_data->text_texture;

  // Shape / sprite batch (and, in Combined mode, text glyphs too).
  if (batch->count > 0)
  {
    if (font_texture)
      font_texture->bind(TEXTURE_TEXT);
    if (s_data->quad_texture)
      s_data->quad_texture->bind(TEXTURE_SPRITE);

    draw_instances(batch->instances, batch->count, s_data->quad_shader);
  }

  // Dedicated MSDF text pass used by Independent mode.
  if (batch->text_count > 0)
  {
    if (font_texture)
      font_texture->bind(TEXTURE_TEXT);
    draw_instances(batch->text_instances, batch->text_count, s_data->text_shader);
  }
}
}  // namespace ag
