#ifndef RECT_HPP
#define RECT_HPP

#include <Math/vec2.hpp>

namespace ag
{
    template <typename T>
    struct rect
    {
        T x, y;
        T width, height;

        rect(T x_ = T(), T y_ = T(), T width_ = T(), T height_ = T())
            : x(x_), y(y_), width(width_), height(height_) {}

        rect(const vec2<T> &pos, const vec2<T> &size)
            : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

        rect(const vec2<T> &pos, T width_, T height_)
            : x(pos.x), y(pos.y), width(width_), height(height_) {}

        rect(T x_, T y_, const vec2<T> &size)
            : x(x_), y(y_), width(size.x), height(size.y) {}



        bool operator == (const rect& other) const
        {
            return x == other.x && y == other.y && width == other.width && height == other.height;
        }
        bool operator != (const rect& other) const
        {
            return !(*this == other);
        }
    };

    using recti = rect<int>;
    using rectf = rect<float>;
}
#endif
