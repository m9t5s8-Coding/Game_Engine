#pragma once

#include <utility>
#include <cmath>
#include <type_traits>

namespace aero
{
    template <typename T>
    struct vector_2
    {
        T x;
        T y;

        vector_2(T x_ = T(), T y_ = T()) : x(x_), y(y_) {}

        vector_2 round() const
        {
            return {std::round(x), std::round(y)};
        }

        template <typename U>
        vector_2(const vector_2<U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

        vector_2 operator+(const vector_2 &other) const
        {
            return vector_2(x + other.x, y + other.y);
        }
        vector_2 operator+=(const vector_2 &other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }
        vector_2 operator+(T scalar) const
        {
            return vector_2(x + scalar, y + scalar);
        }

        vector_2 operator-(const vector_2 &other) const
        {
            return vector_2(x - other.x, y - other.y);
        }
        vector_2 operator-=(const vector_2 &other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        vector_2 operator-(T scalar) const
        {
            return vector_2(x - scalar, y - scalar);
        }

        vector_2 operator*(const vector_2 &other) const
        {
            return vector_2(x * other.x, y * other.y);
        }
        vector_2 operator*=(const vector_2 &other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        vector_2 operator*(T scalar) const
        {
            return vector_2(x * scalar, y * scalar);
        }

        vector_2 operator/(const vector_2 &other) const
        {
            return vector_2(x / other.x, y / other.y);
        }
        vector_2 operator/=(const vector_2 &other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }
        vector_2 operator/(T scalar) const
        {
            return vector_2(x / scalar, y / scalar);
        }

        bool operator==(const vector_2<T> &other) const
        {
            constexpr T epsilon = static_cast<T>(1.0E-4);
            if constexpr (std::is_floating_point_v<T>)
            {
                return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon;
            }
            else
            {
                return x == other.x && y == other.y;
            }
        }

        bool operator!=(const vector_2<T> &other) const
        {
            return !(*this == other);
        }
    };

    using vector_2i = vector_2<int>;
    using vector_2f = vector_2<float>;
    using vector_2u = vector_2<unsigned int>;
}

