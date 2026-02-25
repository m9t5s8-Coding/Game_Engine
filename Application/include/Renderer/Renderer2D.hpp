#pragma once

#include <atomic>
#include <condition_variable>
#include <Core/Core.hpp>
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
  static void init();
  static void shut_down();
  static void begin_scene(const View& view, const vec2f& viewport_size);
  static void begin_scene();
  static void end_scene();
  static void flush();
  static void start_batch();
  static void set_texture(const AG_ref<Texture>& texture);
  static void
  draw_rectangle(const Rectangle& rect, const Transform_Component& transform, int entity_id = -1);
  static void
  draw_circle(const Circle& circle, const Transform_Component& transform, int entity_id = -1);
  static void
  draw_sprite(const Sprite& sprite, const Transform_Component& transform, int entity_id = -1);
  static void draw_text(const Text& text, const Transform_Component& transform, int entity_id = -1);
  static void draw_fullscreen_quad(AG_uint id);

  struct ProcessedBatch
  {
    Quad_Instance* instances = nullptr;
    size_t         count     = 0;
    size_t         capacity  = 0;

    bool is_empty() const
    {
      return count == 0;
    }
    void clear()
    {
      count = 0;
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
