#include <Aero.hpp>

#include <Renderer/Renderer2D.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptDraw.hpp>

#include <algorithm>
#include <cmath>

namespace ag {
namespace {

Transform_Component transform_at(const vec2f& position, float rotation = 0.0f) {
  Transform_Component transform;
  transform.position = position;
  transform.scale    = {1.0f, 1.0f};
  transform.rotation = rotation;
  return transform;
}

void submit_rect(const DrawRect& command) {
  Rectangle rect;
  rect.size             = command.size;
  rect.fill_color       = command.color;
  rect.corner_radius    = std::max(0.0f, command.corner_radius);
  rect.border_thickness = std::max(0.0f, command.border_thickness);
  rect.border_color     = command.border_color;
  rect.mode             = command.mode;

  Renderer2D::draw_rectangle(rect, transform_at(command.position, command.rotation));
}

void submit_circle(const DrawCircle& command) {
  Circle circle;
  const float radius = std::max(0.0f, command.radius);
  circle.size        = {radius * 2.0f, radius * 2.0f};
  circle.fill_color       = command.color;
  circle.border_thickness = std::max(0.0f, command.border_thickness);
  circle.border_color     = command.border_color;
  circle.mode             = command.mode;

  Renderer2D::draw_circle(circle, transform_at(command.position));
}

void submit_line(const DrawLine& command) {
  const vec2f delta = command.end - command.start;
  const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
  if (length <= 0.0001f)
    return;

  Rectangle rect;
  rect.size             = {length, std::max(0.1f, command.thickness)};
  rect.fill_color       = command.color;
  rect.border_thickness = 0.0f;
  rect.mode             = command.mode;

  const vec2f center = (command.start + command.end) * 0.5f;
  const float angle  = Math::to_degree(std::atan2(delta.y, delta.x));
  Renderer2D::draw_rectangle(rect, transform_at(center, angle));
}

void submit_text(const DrawText& command) {
  Text text;
  text.text       = command.text;
  text.font_path  = command.font_path;
  text.font_size  = command.font_size;
  text.text_color = command.color;
  text.mode       = command.mode;
  text.line_height = command.line_height;
  text.bounds      = command.bounds;
  text.draw_rect   = false;

  Renderer2D::draw_text(text, transform_at(command.position));
}

}  // namespace

void DrawRect::draw() const {
  submit_rect(*this);
}

void DrawCircle::draw() const {
  submit_circle(*this);
}

void DrawLine::draw() const {
  submit_line(*this);
}

void DrawText::draw() const {
  submit_text(*this);
}

void ScriptBinding::register_draw() {
  auto& lua = ScriptManager::get_lua();

  lua.new_usertype<DrawRect>(
      "DrawRect",
      sol::constructors<DrawRect(),
                        DrawRect(float, float, float, float),
                        DrawRect(float, float, float, float, const Color&),
                        DrawRect(const vec2f&, const vec2f&),
                        DrawRect(const vec2f&, const vec2f&, const Color&)>(),
      "position", &DrawRect::position,
      "size", &DrawRect::size,
      "color", &DrawRect::color,
      "rotation", &DrawRect::rotation,
      "corner_radius", &DrawRect::corner_radius,
      "border_thickness", &DrawRect::border_thickness,
      "border_color", &DrawRect::border_color,
      "mode", &DrawRect::mode,
      "draw", &DrawRect::draw);

  lua.new_usertype<DrawCircle>(
      "DrawCircle",
      sol::constructors<DrawCircle(),
                        DrawCircle(float, float, float),
                        DrawCircle(float, float, float, const Color&),
                        DrawCircle(const vec2f&, float),
                        DrawCircle(const vec2f&, float, const Color&)>(),
      "position", &DrawCircle::position,
      "radius", &DrawCircle::radius,
      "color", &DrawCircle::color,
      "border_thickness", &DrawCircle::border_thickness,
      "border_color", &DrawCircle::border_color,
      "mode", &DrawCircle::mode,
      "draw", &DrawCircle::draw);

  lua.new_usertype<DrawLine>(
      "DrawLine",
      sol::constructors<DrawLine(),
                        DrawLine(float, float, float, float),
                        DrawLine(float, float, float, float, const Color&, float),
                        DrawLine(const vec2f&, const vec2f&),
                        DrawLine(const vec2f&, const vec2f&, const Color&, float)>(),
      "start", &DrawLine::start,
      "end", &DrawLine::end,
      "color", &DrawLine::color,
      "thickness", &DrawLine::thickness,
      "mode", &DrawLine::mode,
      "draw", &DrawLine::draw);

  lua.new_usertype<DrawText>(
      "DrawText",
      sol::constructors<DrawText(),
                        DrawText(std::string, float, float, float),
                        DrawText(std::string, float, float, float, const Color&),
                        DrawText(std::string, const vec2f&, float),
                        DrawText(std::string, const vec2f&, float, const Color&)>(),
      "text", &DrawText::text,
      "position", &DrawText::position,
      "font_size", &DrawText::font_size,
      "color", &DrawText::color,
      "font_path", &DrawText::font_path,
      "mode", &DrawText::mode,
      "line_height", &DrawText::line_height,
      "bounds", &DrawText::bounds,
      "draw", &DrawText::draw);

  auto draw = lua.create_named_table("Draw");

  draw.set_function("rect",
                    sol::overload([](const DrawRect& command) { command.draw(); },
                                  [](float x, float y, float width, float height) {
                                    DrawRect(x, y, width, height).draw();
                                  },
                                  [](float x, float y, float width, float height, const Color& color) {
                                    DrawRect(x, y, width, height, color).draw();
                                  }));
  draw.set_function("circle",
                    sol::overload([](const DrawCircle& command) { command.draw(); },
                                  [](float x, float y, float radius) {
                                    DrawCircle(x, y, radius).draw();
                                  },
                                  [](float x, float y, float radius, const Color& color) {
                                    DrawCircle(x, y, radius, color).draw();
                                  }));
  draw.set_function("line",
                    sol::overload([](const DrawLine& command) { command.draw(); },
                                  [](float x1, float y1, float x2, float y2) {
                                    DrawLine(x1, y1, x2, y2).draw();
                                  },
                                  [](float x1, float y1, float x2, float y2, const Color& color,
                                     float thickness) {
                                    DrawLine(x1, y1, x2, y2, color, thickness).draw();
                                  }));
  draw.set_function("text",
                    sol::overload([](const DrawText& command) { command.draw(); },
                                  [](const std::string& value, float x, float y, float size) {
                                    DrawText(value, x, y, size).draw();
                                  },
                                  [](const std::string& value, float x, float y, float size,
                                     const Color& color) {
                                    DrawText(value, x, y, size, color).draw();
                                  }));

  // Global aliases keep simple scripts concise while the Draw table provides a
  // discoverable namespace for editor tooling.
  lua.set_function("draw_rect", draw["rect"]);
  lua.set_function("draw_circle", draw["circle"]);
  lua.set_function("draw_line", draw["line"]);
  lua.set_function("draw_text", draw["text"]);
}

}  // namespace ag
