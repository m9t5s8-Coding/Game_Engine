#pragma once

#include <Renderer/ViewController.hpp>
#include <Core/Core.hpp>
#include <Math/Math.hpp>
#include <Renderer/Color.hpp>

#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>

namespace ag
{
  struct Rectangle
  {
    vec2u size;
    vec2f position;
    vec2f scale = {1, 1};
    vec2u origin;
    Color color;
  };
  struct Vertex_Data
  {
    float x, y;
    float r, g, b, a;
    float u, v;
  };

  class Renderer2D
  {
  public:
    static void init();
    static void shut_down();

    static void begin_scene(const ViewController &view_controller);
    static void use_shader(const AG_ref<Shader>& shader);
    static void end_scene();
    static void flush();

    static void draw_rect(const Rectangle& rect_data);

  private:
    struct Renderer2D_Data
    {
      AG_ref<VertexArray> vertexarray;
      AG_ref<Shader> shader;

      AG_uint max_rect = 0;
      AG_uint max_vertices = 0;
      AG_uint max_indices = 0;

      std::vector<Vertex_Data> vertices;

      AG_uint total_rect = 0;

      vec2f view_center;
      vec2u view_size;
    };
    static Renderer2D_Data* s_data;
    static vec2f calc_vertex(vec2f vertex);
  };
}
