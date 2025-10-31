#pragma once

#include <Apch.hpp>
#include <imgui.h>

namespace ag
{
    template <typename T>
    struct vec2
    {
        T x;
        T y;

        vec2(T x_ = T(), T y_ = T()) : x(x_), y(y_) {}
        vec2(T s_) : x(s_), y(s_) {}

        float length() const
        {
          return std::sqrt(x * x + y * y);
        }

        json save() const
        {
          return { x, y };
        }

        void load(const json& j)
        {
          x = j[0].get<T>();
          y = j[1].get<T>();
        }

        vec2 round() const
        {
            return {std::round(x), std::round(y)};
        }

        template <typename U>
        vec2(const vec2<U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

        // Addition
        vec2 operator+(const vec2 &other) const
        {
            return vec2(x + other.x, y + other.y);
        }
        vec2 operator+=(const vec2 &other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }
        vec2 operator +=(T scalar)
        {
          x += scalar;
          y += scalar;
          return *this;
        }
        vec2 operator+(T scalar) const
        {
            return vec2(x + scalar, y + scalar);
        }

        // Subtraction
        vec2 operator-(const vec2 &other) const
        {
            return vec2(x - other.x, y - other.y);
        }
        vec2 operator-=(const vec2 &other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        vec2 operator-=(T scalar)
        {
          x -= scalar;
          y -= scalar;
          return *this;
        }
        vec2 operator-(T scalar) const
        {
            return vec2(x - scalar, y - scalar);
        }

        // Multiply
        vec2 operator*(const vec2 &other) const
        {
            return vec2(x * other.x, y * other.y);
        }
        vec2 operator*=(const vec2 &other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        vec2 operator*=(T scalar)
        {
          x *= scalar;
          y*= scalar;
          return *this;
        }
        vec2 operator*(T scalar) const
        {
            return vec2(x * scalar, y * scalar);
        }

        // Division
        vec2 operator/(const vec2 &other) const
        {
            return vec2(x / other.x, y / other.y);
        }
        vec2 operator/=(const vec2 &other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }
        vec2 operator/(T scalar) const
        {
            return vec2(x / scalar, y / scalar);
        }


        bool operator==(const vec2<T> &other) const
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
        bool operator==(T scalar) const
        {
          return x == scalar || y == scalar;
        }

        bool operator!=(const vec2<T> &other) const
        {
            return !(*this == other);
        }


        bool operator<(const vec2<T> &other) const
        {
          if( x < other.x ) return true;
          if( x > other.x) return false;
          return y < other.y;
        }

        bool operator!=(const ImVec2& other) const
        {
          return !(x == static_cast<T>(other.x) && static_cast<int>(y == other.y));
        }
        vec2 operator=(const ImVec2& other)
        {
          return vec2(x = other.x, y = other.y);
        }
      };

    using vec2i = vec2<int>;
    using vec2f = vec2<float>;
    using vec2u = vec2<unsigned int>;
}

