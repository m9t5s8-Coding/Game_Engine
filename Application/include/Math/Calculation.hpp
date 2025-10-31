#pragma once

#include <Math/Rect.hpp>
#include <Math/vec2.hpp>
#include <Math/vec3.hpp>
#include <Math/vec4.hpp>
#include <cmath>


namespace ag::Math
{
  constexpr float PI = 3.14159265359f;

  // Conversion between degree and radians
	inline float to_radians(const float degrees)
  {
    return degrees * PI / 180.0f;
  }
	inline float to_degree(const float radians)
  {
    return radians * 180.0f / PI;
  }

  // Distance Caluclation
  template <typename T>
  inline float distance(const vec2<T> first, const vec2<T> second)
  {
    float dx = static_cast<float>(second.x - first.x);
    float dy = static_cast<float>(second.y - first.y);

    return std::sqrt(dx * dx + dy * dy);
  }

  inline float angle_betn_points(const vec2f& first, const vec2f& second)
  {
    float radians = std::atan2(second.y - first.y, second.x - first.x);
    return to_degree(radians);
  }

  inline float angle_betn_3points(const vec2f& a, const vec2f& b, const vec2f& c)
  {
    vec2f ba = { a.x - b.x, a.y - b.y };
    vec2f bc = { c.x - b.x, c.y - b.y };

    float dot = ba.x * bc.x + ba.y * bc.y;
    float det = ba.x * bc.y - ba.y * bc.x;

    return to_degree(std::atan2(det, dot));
  }

  // Converting the screen position to the world position
  inline vec2f screen_to_world(const vec2f& screen, const float_rect& window_rect, const vec2f &size)
  {
    vec2f view_size = window_rect.size;
    vec2f view_center = window_rect.position + window_rect.size / 2;

    vec2f ndc = (screen / size) * 2.0f - 1.0f;
    vec2f world = ndc * (view_size / 2.0f) + view_center;

    return world;
  }

  inline vec2f world_to_screen(const vec2f& world, const float_rect& window_rect, const vec2f &size)
  {
    vec2f view_size = window_rect.size;
    vec2f view_center = window_rect.position + window_rect.size / 2;

    vec2f ndc = (world - view_center) / (view_size/ 2.0f);
    vec2f screen = ((ndc + 1.0f)/ 2.0f) * size;

    return screen;
  }

}
