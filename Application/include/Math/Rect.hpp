#ifndef RECT_HPP
#define RECT_HPP

#include <Math/vec2.hpp>
#include <Math/vec4.hpp>

namespace ag
{
    template <typename T>
    struct rect
    {
        vec2<T> position;
        vec2<T> size;

        rect(T x_ = T(), T y_ = T(), T width_ = T(), T height_ = T())
            : position(x_, x_), size(width_, height_) {}

        rect(const vec2<T> &pos, const vec2<T> &size)
            : position(pos), size(size) {}

        rect(const vec2<T> &pos, T width_, T height_)
            : position(pos), size(width_, height_) {}

        rect(T x_, T y_, const vec2<T> &size)
            : position(x_, y_), size(size) {}

        json save() const
        {
          return { position.x, position.y, size.x, size.y };
        }

        void load(const json& j)
        {
          position.x = j[0].get<T>();
          position.y = j[1].get<T>();
          size.x = j[2].get<T>();
          size.y = j[3].get<T>();
        }

        void to_vec4(vec4f& other) const
        {
          other.x = position.x;
          other.y = position.y;
          other.z = size.x;
          other.w = size.y;
        }

        bool intersects(const  rect& other) const
        {
          return  (position.x < other.position.x + other.size.x ) &&
                  (position.x + size.x > other.position.x) &&
                  (position.y < other.position.y + other.size.y) &&
                  (position.y + size.y > other.position.y);
        }


        bool operator == (const rect& other) const
        {
            return position = other.position && size = other.size;
        }
        bool operator != (const rect& other) const
        {
            return !(*this == other);
        }
    };

    using int_rect = rect<int>;
    using float_rect = rect<float>;
    using uint_rect = rect<unsigned int>;
}
#endif
