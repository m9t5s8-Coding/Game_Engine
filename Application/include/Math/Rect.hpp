#ifndef RECT_HPP
#define RECT_HPP

#include <Math/Vector2.hpp>

namespace aero
{
    template <typename T>
    struct Rect
    {
        T x, y;
        T width, height;

        Rect(T x_ = T(), T y_ = T(), T width_ = T(), T height_ = T())
            : x(x_), y(y_), width(width_), height(height_) {}

        Rect(const vector_2<T> &pos, const vector_2<T> &size)
            : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

        Rect(const vector_2<T> &pos, T width_, T height_)
            : x(pos.x), y(pos.y), width(width_), height(height_) {}

        Rect(T x_, T y_, const vector_2<T> &size)
            : x(x_), y(y_), width(size.x), height(size.y) {}



        bool operator == (const Rect& other) const
        {
            return x == other.x && y == other.y && width == other.width && height == other.height;
        }
        bool operator != (const Rect& other) const
        {
            return !(*this == other);
        }
    };

    using IntRect = Rect<int>;
    using FloatRect = Rect<float>;
}
#endif
