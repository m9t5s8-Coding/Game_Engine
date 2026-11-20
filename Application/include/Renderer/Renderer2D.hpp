#pragma once

#include <atomic>
#include <condition_variable>
#include <Core/Core.hpp>
#include <Font/FontManager.hpp>
#include <GameObjects/Components/Components.hpp>
#include <mutex>
#include <queue>
#include <Renderer/Texture.hpp>
#include <Renderer/ViewController.hpp>
#include <thread>
#include <variant>

namespace ag
{
struct Quad_Instance;

class Renderer2D
{
public:
  // Combined draws text glyphs in the same instanced batch as rectangles,
  // circles and sprites (the original path). Independent draws text in a
  // second, dedicated MSDF pass with its own shader after the shape batch.
  // The default stays on Combined; at rotation 0 it remains visually
  // compatible with the original text path. Switch at runtime with
  // set_text_render_mode().
  enum class TextRenderMode
  {
    Combined = 0,
    Independent = 1
  };

  static void init();
  static void shut_down();
  static void begin_scene(const View& view, const vec2f& viewport_size);
  static void begin_scene();
  static void end_scene();
  static void flush();
  static void start_batch();
  static void set_texture(const AG_ref<Texture>& texture);
  static void set_text_render_mode(TextRenderMode mode);
  static TextRenderMode get_text_render_mode();
  static void
  draw_rectangle(const Rectangle& rect, const Transform_Component& transform, int entity_id = -1);
  static void
  draw_circle(const Circle& circle, const Transform_Component& transform, int entity_id = -1);
  static void
  draw_sprite(const Sprite& sprite, const Transform_Component& transform, int entity_id = -1);
  static void draw_text(const Text& text, const Transform_Component& transform, int entity_id = -1);
  static void draw_fullscreen_quad(AG_uint id, const vec2f& viewport, const vec2f& size);

  struct ProcessedBatch
  {
    Quad_Instance* instances      = nullptr;
    size_t         count          = 0;
    size_t         capacity       = 0;
    Quad_Instance* text_instances = nullptr;
    size_t         text_count     = 0;
    size_t         text_capacity  = 0;

    // Renderer2D has a single glyph texture slot, so a batch holds text from
    // one font. draw_text() flushes whenever the requested font changes.
    AG_ref<Texture> font_texture;
    std::string     font_path;

    bool is_empty() const
    {
      return count == 0 && text_count == 0;
    }
    void clear()
    {
      count       = 0;
      text_count  = 0;
      font_texture.reset();
      font_path.clear();
    }
  };

private:
  struct RectangleCommand
  {
    Rectangle           rect;
    Transform_Component transform;
    int                 entity_id = -1;
  };
  struct CircleCommand
  {
    Circle              circle;
    Transform_Component transform;
    int                 entity_id = -1;
  };
  struct SpriteCommand
  {
    Sprite              sprite;
    Transform_Component transform;
    int                 entity_id;
    vec2f               texture_size;
  };
  struct TextCommand
  {
    Text                text;
    Transform_Component transform;
    int                 entity_id;
    vec2f               longest_size;
    vec2f               viewport_size;
    View                view;
    std::shared_ptr<FontManager::LoadedFont> font;
    bool                has_cursor     = false;
    bool                cursor_visible = false;
    size_t              caret_index    = 0;
  };

  using DrawCommand = std::variant<RectangleCommand, CircleCommand, SpriteCommand, TextCommand>;

  static void worker_thread_function();
  static void process_rectangle(const RectangleCommand& cmd, ProcessedBatch& batch);
  static void process_circle(const CircleCommand& cmd, ProcessedBatch& batch);
  static void process_sprite(const SpriteCommand& cmd, ProcessedBatch& batch);
  static void process_text(const TextCommand& cmd, ProcessedBatch& batch);

  // Original text path: glyphs are appended to the same instance batch as
  // shapes and sampled through the shared quad shader.
  static void process_text_combined(const TextCommand& cmd, ProcessedBatch& batch);
  // Alternative text path: glyphs go to their own batch and are drawn by the
  // dedicated MSDF text shader in a separate pass.
  static void process_text_independent(const TextCommand& cmd, ProcessedBatch& batch);

  static void flush_internal(ProcessedBatch* batch);

public:
  struct AsyncData
  {
    std::queue<DrawCommand> command_queue;
    std::mutex              command_mutex;
    std::condition_variable command_cv;

    ProcessedBatch  batch_a;
    ProcessedBatch  batch_b;
    ProcessedBatch* current_batch = nullptr;

    std::mutex                   processed_mutex;
    std::condition_variable      processed_cv;
    std::atomic<ProcessedBatch*> ready_batch{nullptr};

    std::thread         worker_thread;
    std::atomic<bool>   running{true};
    std::atomic<bool>   flush_requested{false};
    std::atomic<size_t> total_commands{0};
  };
};

}  // namespace ag
