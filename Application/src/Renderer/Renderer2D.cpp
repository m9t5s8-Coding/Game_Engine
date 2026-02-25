#include <array>
#include <cmath>
#include <Core/Application.hpp>
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

namespace ag
{

// ─────────────────────────────────────────────
//  Quad_Instance full definition
//  (forward-declared as `struct Quad_Instance` in the header)
// ─────────────────────────────────────────────

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

// ─────────────────────────────────────────────
//  Local type aliases for the header's nested types
//  (all process_* / worker are static members of
//   Renderer2D so they can access private nested types)
// ─────────────────────────────────────────────

using AsyncData      = Renderer2D::AsyncData;
using ProcessedBatch = Renderer2D::ProcessedBatch;

// ─────────────────────────────────────────────
//  File-scope renderer state
// ─────────────────────────────────────────────

struct Renderer2D_Data
{
  AG_uint max_shape    = 0;
  AG_uint max_vertices = 0;
  AG_uint max_indices  = 0;

  AG_ref<VertexArray>  quad_vertex_array;
  AG_ref<VertexBuffer> quad_instanced_buffer;
  AG_ref<Shader>       quad_shader;
  AG_ref<Texture>      quad_texture, text_texture;

  AG_ref<VertexArray>  fullscreen_vertex_array;
  AG_ref<VertexBuffer> fullscreen_instanced_buffer;
  AG_ref<Shader>       fullscreen_shader;

  View    view;
  vec2f   viewport_size;
  AG_uint slots;

  std::unique_ptr<AsyncData> async_data;  // AsyncData is public in header
};

static Renderer2D_Data* s_data = nullptr;

// ─────────────────────────────────────────────
//  Internal helper — throttled worker wake-up
// ─────────────────────────────────────────────

static void maybe_notify()
{
  static thread_local int counter = 0;
  if (++counter >= 1000)
  {
    s_data->async_data->command_cv.notify_one();
    counter = 0;
  }
}

// ─────────────────────────────────────────────
//  init
// ─────────────────────────────────────────────

void Renderer2D::init()
{
  s_data               = new Renderer2D_Data();
  s_data->max_shape    = 100000;
  s_data->max_vertices = s_data->max_shape * 4;
  s_data->max_indices  = s_data->max_shape * 6;
  s_data->slots        = 3;

  AG_uint             indices[]   = {0, 1, 2, 0, 2, 3};
  AG_ref<IndexBuffer> indexbuffer = ag::IndexBuffer::create(indices, 6);
  float               vertices[]  = {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f};

  // ── Fullscreen quad ──────────────────────────
  {
    float full_vertices[] =
        {-1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, -1.f, 1.f, 0.f};
    BufferLayout layout = {
        {ShaderDataType::Float2, "a_Position"},
        {ShaderDataType::Float2, "a_texcoord"}
    };
    s_data->fullscreen_vertex_array = VertexArray::create();
    auto vb                         = VertexBuffer::create(full_vertices, sizeof(full_vertices));
    vb->set_layout(layout);
    s_data->fullscreen_vertex_array->add_vertex_buffer(vb);
    s_data->fullscreen_vertex_array->set_index_buffer(indexbuffer);
    s_data->fullscreen_shader = Shader::create("shaders/Sprite2D.glsl");
  }

  // ── Instanced quad ───────────────────────────
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

    s_data->quad_instanced_buffer =
        VertexBuffer::create(nullptr, sizeof(Quad_Instance) * s_data->max_shape);
    s_data->quad_instanced_buffer->set_layout(instance_layout);
    s_data->quad_vertex_array->add_vertex_buffer(s_data->quad_instanced_buffer, true);
    s_data->quad_vertex_array->set_index_buffer(indexbuffer);
    s_data->quad_shader = ag::Shader::create("shaders/Quad.glsl");

    s_data->quad_texture =
        ag::Texture2D::create("textures/default.png", true, Filter_Mode::AG_NEAREST, true);
    s_data->text_texture =
        ag::Texture2D::create("textures/atlas.png", true, Filter_Mode::AG_LINEAR, true);
    TextLoader::loadGlyph("textures/atlas.json");
  }

  // ── Async double-buffer ───────────────────────
  s_data->async_data = std::make_unique<AsyncData>();

  auto alloc_batch = [&](ProcessedBatch& b)
  {
#ifdef _MSC_VER
    b.instances = (Quad_Instance*)_aligned_malloc(sizeof(Quad_Instance) * s_data->max_shape, 64);
#else
    b.instances = (Quad_Instance*)aligned_alloc(64, sizeof(Quad_Instance) * s_data->max_shape);
#endif
    b.capacity = s_data->max_shape;
    b.count    = 0;
  };

  alloc_batch(s_data->async_data->batch_a);
  alloc_batch(s_data->async_data->batch_b);
  s_data->async_data->current_batch = &s_data->async_data->batch_a;

  s_data->async_data->worker_thread = std::thread(worker_thread_function);
}

// ─────────────────────────────────────────────
//  shut_down
// ─────────────────────────────────────────────

void Renderer2D::shut_down()
{
  if (!s_data)
    return;

  if (s_data->async_data)
  {
    s_data->async_data->running = false;
    s_data->async_data->command_cv.notify_one();

    if (s_data->async_data->worker_thread.joinable())
      s_data->async_data->worker_thread.join();

#ifdef _MSC_VER
    _aligned_free(s_data->async_data->batch_a.instances);
    _aligned_free(s_data->async_data->batch_b.instances);
#else
    free(s_data->async_data->batch_a.instances);
    free(s_data->async_data->batch_b.instances);
#endif
  }

  delete s_data;
  s_data = nullptr;
}

// ─────────────────────────────────────────────
//  Scene control
// ─────────────────────────────────────────────

void Renderer2D::begin_scene(const View& view, const vec2f& viewport_size)
{
  Renderer::begin_scene(view, viewport_size);
  s_data->view          = view;
  s_data->viewport_size = viewport_size;
  Renderer::enable_blend();
}

void Renderer2D::begin_scene()
{ /* accumulate — no flush on begin */
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

// ─────────────────────────────────────────────
//  Fullscreen quad  (immediate — main thread)
// ─────────────────────────────────────────────

void Renderer2D::draw_fullscreen_quad(AG_uint id)
{
  Renderer::disable_blend();
  Renderer::bind(id, TEXTURE_SCENE);
  s_data->fullscreen_shader->bind();
  s_data->fullscreen_shader->set_int("u_texture", TEXTURE_SCENE);
  Renderer::submit(s_data->fullscreen_vertex_array);
}

// ─────────────────────────────────────────────
//  Draw calls — enqueue to command queue
// ─────────────────────────────────────────────

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
  // ECS / blink state resolved on main thread — worker never touches ECS
  TextCommand cmd;
  cmd.text          = text_string;
  cmd.transform     = transform;
  cmd.entity_id     = entity_id;
  cmd.longest_size  = Text::calc_longest_size(text_string, transform.scale);
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
}

// ─────────────────────────────────────────────
//  Worker thread
// ─────────────────────────────────────────────

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
    // lock released — process without contention

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

// ─────────────────────────────────────────────
//  process_rectangle
// ─────────────────────────────────────────────

void Renderer2D::process_rectangle(const RectangleCommand& cmd, ProcessedBatch& batch)
{
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

// ─────────────────────────────────────────────
//  process_circle
// ─────────────────────────────────────────────

void Renderer2D::process_circle(const CircleCommand& cmd, ProcessedBatch& batch)
{
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

// ─────────────────────────────────────────────
//  process_sprite
// ─────────────────────────────────────────────

void Renderer2D::process_sprite(const SpriteCommand& cmd, ProcessedBatch& batch)
{
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

// ─────────────────────────────────────────────
//  process_text  (worker — no ECS access)
// ─────────────────────────────────────────────

void Renderer2D::process_text(const TextCommand& cmd, ProcessedBatch& batch)
{
  size_t index       = 0;
  size_t break_index = 0;

  Transform_Component trans;
  trans.position = cmd.transform.position - (cmd.longest_size * 0.5);
  trans.scale    = cmd.transform.scale;
  trans.rotation = cmd.transform.rotation;

  const float scale_x = trans.scale.x * cmd.text.font_size / TextLoader::font.em_size;
  const float scale_y = trans.scale.y * cmd.text.font_size / TextLoader::font.em_size;
  const vec2f scale_vec(scale_x, scale_y);

  vec2f starting_pos =
      Text::center_single_line_text(cmd.text, trans, cmd.longest_size, index, &break_index);
  const vec2f padding = cmd.text.padding * scale_vec;
  starting_pos += padding;

  const float ascender = TextLoader::font.ascender * TextLoader::font.em_size * scale_y;
  const float line_height =
      TextLoader::font.line_height * TextLoader::font.em_size * scale_y * cmd.text.line_height;

  starting_pos.y += ascender;
  float base_line = starting_pos.y;

  auto space_it = TextLoader::font.glyphs.find(' ');
  if (space_it == TextLoader::font.glyphs.end())
    return;

  const float tab_advance = space_it->second.advance * scale_x * 4.0f;

  vec4f norm_color;
  cmd.text.text_color.normalize_color(norm_color);

  // ── Optional bounding rect ────────────────────
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
    rt.position = trans.position + rect.size * 0.5f;

    process_rectangle(RectangleCommand{rect, rt, cmd.entity_id}, batch);
  }

  const size_t text_length = cmd.text.text.size();

  for (; index < text_length; ++index)
  {
    const char c = cmd.text.text[index];

    // ── Cursor BEFORE current char ──────────────
    if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == index)
    {
      vec2f csz  = {6.0f * scale_x,
                    TextLoader::font.line_height * TextLoader::font.em_size * scale_y};
      vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

      Rectangle cr;
      cr.mode       = cmd.text.mode;
      cr.fill_color = cmd.text.text_color;
      vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
      cr.size       = {ws.x, csz.y};

      Transform_Component ct;
      ct.position = {cpos.x + cr.size.x * 0.5f, cpos.y};

      process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
    }

    if (c == '\n')
    {
      size_t ni = index + 1;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index);
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

    auto it = TextLoader::font.glyphs.find(c);
    if (it == TextLoader::font.glyphs.end())
      continue;

    const TextLoader::Glyph& g = it->second;

    if (cmd.longest_size.x > 0 && index == break_index && break_index != 0)
    {
      size_t ni = index;
      starting_pos =
          Text::center_single_line_text(cmd.text, trans, cmd.longest_size, ni, &break_index);
      starting_pos += padding;
      starting_pos.y += ascender;
      starting_pos.y += line_height;
      base_line += line_height;

      if (c == ' ')
        continue;
    }

    Quad_Instance* __restrict inst = &batch.instances[batch.count++];
    inst->texture_size             = s_data->text_texture->get_size();
    inst->size                     = g.texture_rect.size * scale_vec;
    inst->origin                   = inst->size * 0.5f;
    g.texture_rect.to_vec4(inst->texture_rect);
    inst->fill_color   = norm_color;
    inst->mode         = static_cast<int>(cmd.text.mode);
    inst->quad_mode    = static_cast<int>(Quad_Type::Text);
    inst->texture_slot = TEXTURE_TEXT;
    inst->position.x   = starting_pos.x + g.plane_left * scale_x;
    inst->position.y   = starting_pos.y - (g.plane_top * scale_y);
    inst->entity_id    = cmd.entity_id;

    starting_pos.x += g.advance * scale_x;
  }

  // ── Cursor AFTER last char ───────────────────
  if (cmd.has_cursor && cmd.cursor_visible && cmd.caret_index == text_length)
  {
    vec2f csz = {6.0f * scale_x, TextLoader::font.line_height * TextLoader::font.em_size * scale_y};
    vec2f cpos = {starting_pos.x, base_line - ascender + csz.y * 0.5f};

    Rectangle cr;
    cr.mode       = cmd.text.mode;
    cr.fill_color = cmd.text.text_color;
    vec2f ws      = Math::screen_size_to_world_size(csz, cmd.view.get_size(), cmd.viewport_size);
    cr.size       = {ws.x, csz.y};

    Transform_Component ct;
    ct.position = {cpos.x + cr.size.x * 0.5f, cpos.y};

    process_rectangle(RectangleCommand{cr, ct, cmd.entity_id}, batch);
  }
}

// ─────────────────────────────────────────────
//  flush_internal  (GPU upload — main thread)
// ─────────────────────────────────────────────

void Renderer2D::flush_internal(ProcessedBatch* batch)
{
  if (!batch || batch->is_empty())
    return;

  s_data->text_texture->bind(TEXTURE_TEXT);
  if (s_data->quad_texture)
    s_data->quad_texture->bind(TEXTURE_SPRITE);

  constexpr size_t MAX_PER_DRAW = 65536;
  size_t           remaining    = batch->count;
  size_t           offset       = 0;

  while (remaining > 0)
  {
    const size_t draw_count = std::min(remaining, MAX_PER_DRAW);
    const size_t data_size  = draw_count * sizeof(Quad_Instance);

    s_data->quad_instanced_buffer->set_data(&batch->instances[offset], data_size);
    Renderer::submit_instanced(s_data->quad_shader, s_data->quad_vertex_array, draw_count);

    offset += draw_count;
    remaining -= draw_count;
  }
}

}  // namespace ag
