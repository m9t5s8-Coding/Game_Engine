#pragma once

#include <Apch.hpp>
#include <imgui.h>
#include "box2d/box2d.h"

namespace ag
{
    template <typename T>
    struct vec2
    {
        T x;
        T y;

        vec2(T x_ = T(), T y_ = T()) : x(x_), y(y_) {}
        vec2(T s_) : x(s_), y(s_) {}

        vec2<T>(const ImVec2& v) : x(v.x), y(v.y) {}
        vec2<T>(const b2Vec2& v) : x(v.x), y(v.y) {}

        T length() const {
          if constexpr (std::is_floating_point_v<T>) {
            return std::sqrt(x * x + y * y);
          }
          else {
          
            static_assert(std::is_floating_point_v<T>,
              "length() is only meaningful for floating point types");
            return T(0);
          }
        }

        vec2<T>& normalize() {
          if constexpr (std::is_floating_point_v<T>) {
            T len = length();
            if (len > static_cast<T>(0)) {
              x /= len;
              y /= len;
            }
          }
          else {
            static_assert(std::is_floating_point_v<T>,
              "normalize() is only valid for floating point types");
          }
          return *this;
        }

        float average() const
        {
          return static_cast<float>((x + y) * 0.5);
        }

        vec2<T> normalized() const {
          vec2<T> result = *this;
          return result.normalize();
        }

        void ceil()
        {
          x = std::ceil(x);
          y = std::ceil(y);
        }
        void floor()
        {
          x = std::floor(x);
          y = std::floor(y);
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

        void print() const
        {
          AERO_CORE_INFO("({0}, {1})", x, y);
        }

        vec2 round() const
        {
            return {std::round(x), std::round(y)};
        }

        ImVec2 to_imvec2() const
        {
          return ImVec2(x, y);
        }
        
        void to_vec2(const ImVec2& other)
        {
          x = other.x;
          y = other.y;
        }

        template <typename U>
        vec2(const vec2<U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) 
        {
        }
     
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
          y *= scalar;
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
        vec2 operator/=(T scalar)
        {
          x /= scalar;
          y /= scalar;
          return *this;
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
          return x < other.x || y < other.y;
        }
        bool operator>(const vec2<T>& other) const
        {
          return x > other.x || y > other.y;
        }


        bool operator!=(const ImVec2& other) const
        {
          return !(x == static_cast<T>(other.x) && static_cast<int>(y == other.y));
        }
        vec2& operator=(const ImVec2& other)
        {
          x = other.x;
          y = other.y;
          return *this;
        }
    };

    template <typename T>
    struct vec2_hash
    {
      std::size_t operator()(const vec2<T>& v) const noexcept
      {
        std::hash<T> hasher;
        return hasher(v.x) ^ (hasher(v.y) << 1);
      }
    };



    using vec2i = vec2<int>;
    using vec2f = vec2<float>;
    using vec2u = vec2<uint32_t>;

}

