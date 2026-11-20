#pragma once

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Scene/SceneComponent.hpp>
#include <string>
#include <utility>

namespace ag {

// Lua-facing draw commands. Instances can be built from Lua tables or used
// through the global draw_* helpers; calling :draw() submits the command to the
// active Renderer2D scene immediately.
struct DrawRect {
  vec2f      position;
  vec2f      size         = {100.0f, 100.0f};
  Color      color        = Color::White;
  float      rotation     = 0.0f;  // degrees
  float      corner_radius = 0.0f;
  float      border_thickness = 0.0f;
  Color      border_color = Color::Black;
  RenderMode mode         = RenderMode::World;

  DrawRect() = default;
  DrawRect(float x, float y, float width, float height)
      : position(x, y), size(width, height) {}
  DrawRect(float x, float y, float width, float height, const Color& fill)
      : position(x, y), size(width, height), color(fill) {}
  DrawRect(const vec2f& center, const vec2f& dimensions)
      : position(center), size(dimensions) {}
  DrawRect(const vec2f& center, const vec2f& dimensions, const Color& fill)
      : position(center), size(dimensions), color(fill) {}

  void draw() const;
};

struct DrawCircle {
  vec2f      position;
  float      radius           = 50.0f;
  Color      color            = Color::White;
  float      border_thickness = 0.0f;
  Color      border_color     = Color::Black;
  RenderMode mode             = RenderMode::World;

  DrawCircle() = default;
  DrawCircle(float x, float y, float circle_radius)
      : position(x, y), radius(circle_radius) {}
  DrawCircle(float x, float y, float circle_radius, const Color& fill)
      : position(x, y), radius(circle_radius), color(fill) {}
  DrawCircle(const vec2f& center, float circle_radius)
      : position(center), radius(circle_radius) {}
  DrawCircle(const vec2f& center, float circle_radius, const Color& fill)
      : position(center), radius(circle_radius), color(fill) {}

  void draw() const;
};

struct DrawLine {
  vec2f      start;
  vec2f      end;
  Color      color     = Color::White;
  float      thickness = 1.0f;
  RenderMode mode      = RenderMode::World;

  DrawLine() = default;
  DrawLine(float x1, float y1, float x2, float y2)
      : start(x1, y1), end(x2, y2) {}
  DrawLine(float x1, float y1, float x2, float y2, const Color& line_color, float line_thickness = 1.0f)
      : start(x1, y1), end(x2, y2), color(line_color), thickness(line_thickness) {}
  DrawLine(const vec2f& from, const vec2f& to)
      : start(from), end(to) {}
  DrawLine(const vec2f& from, const vec2f& to, const Color& line_color, float line_thickness = 1.0f)
      : start(from), end(to), color(line_color), thickness(line_thickness) {}

  void draw() const;
};

struct DrawText {
  std::string text;
  vec2f       position;
  float       font_size   = 24.0f;
  Color       color       = Color::White;
  std::string font_path;  // empty uses the engine font
  RenderMode  mode        = RenderMode::World;
  float       line_height = 1.0f;
  vec2f       bounds      = {0.0f, 0.0f};

  DrawText() = default;
  DrawText(std::string value, float x, float y, float size)
      : text(std::move(value)), position(x, y), font_size(size) {}
  DrawText(std::string value, float x, float y, float size, const Color& text_color)
      : text(std::move(value)), position(x, y), font_size(size), color(text_color) {}
  DrawText(std::string value, const vec2f& at, float size)
      : text(std::move(value)), position(at), font_size(size) {}
  DrawText(std::string value, const vec2f& at, float size, const Color& text_color)
      : text(std::move(value)), position(at), font_size(size), color(text_color) {}

  void draw() const;
};

}  // namespace ag
