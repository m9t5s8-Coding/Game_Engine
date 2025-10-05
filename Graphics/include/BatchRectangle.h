#pragma once

#include <vector>
#include <Vector2.hpp>
#include <Color.hpp>

namespace aero
{
  struct Rect_Data
  {
    aero::vector_2f size;
    aero::vector_2f position;
    aero::Color fill_color;
  };
  class Batch_Rectangle
  {
  public:
    static void init();
    static void draw();
    static void end();
  };
}


