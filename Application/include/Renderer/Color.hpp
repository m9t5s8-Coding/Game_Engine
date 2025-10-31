#pragma once

#include <cstdint>
#include <Math/vec4.hpp>

namespace ag
{
    struct Color
    {
        uint8_t r, g, b, a;

        Color() : r(0), g(0), b(0), a(255) {}
        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
            : r(red), g(green), b(blue), a(alpha)
        {
        }

        json save() const
        {
          return { r, g, b, a };
        }

        void load(const json& j)
        {
          r = j[0].get<uint8_t>();
          g = j[1].get<uint8_t>();
          b = j[2].get<uint8_t>();
          a = j[3].get<uint8_t>();
        }

        void normalize_color(vec4f &color) const
        {
          color.x = r / 255.0f;
          color.y = g / 255.0f;
          color.z = b / 255.0f;
          color.w = a / 255.0f;
        }

        bool operator == (const Color& other) const
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
        bool operator != (const Color& other) const
        {
            return !(*this == other);
        }

        static const Color Transparent;
        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Orange;
        static const Color Purple;
        static const Color Pink;
        static const Color Brown;
        static const Color Gray;
        static const Color LightGray;
        static const Color DarkGray;
        static const Color Lime;
        static const Color Navy;
        static const Color Teal;
        static const Color Olive;
        static const Color Maroon;
    };
}


