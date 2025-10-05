#pragma once

#include <cstdint>

namespace aero
{
    struct Color
    {
        uint8_t r, g, b, a;

        Color() : r(0), g(0), b(0), a(255) {}
        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
            : r(red), g(green), b(blue), a(alpha)
        {
        }

        void NormalizedColor(float &rn, float &gn, float &bn, float &an) const
        {
            rn = r / 255.0f;
            gn = g / 255.0f;
            bn = b / 255.0f;
            an = a / 255.0f;
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


