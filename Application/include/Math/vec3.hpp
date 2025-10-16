#pragma once

#include <utility>
#include <cmath>
#include <type_traits>
#include <glm/glm.hpp>

namespace ag
{
    template <typename T>
    struct vec3
    {
        T x;
        T y;
        T z;

        vec3(T x_ = T(), T y_ = T(), T z_ = T()) : x(x_), y(y_), z(z_) {}
        vec3(const T s_) : x(s_), y(s_), z(s_) {}

        template <typename U>
        vec3(const vec3<U> &v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)) {}

        glm::vec3 to_glm() const
        {
          return glm::vec3(x, y, z);
        }

        vec3 round() const
        {
            return {std::round(x), std::round(y), std::round(z)};
        }



        vec3 operator+(const vec3 &other) const
        {
            return vec3(x + other.x, y + other.y, z + other.z);
        }
        vec3 operator+=(const vec3 &other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        vec3 operator+(T scalar) const
        {
            return vec3(x + scalar, y + scalar, z + scalar);
        }

        vec3 operator-(const vec3 &other) const
        {
            return vec3(x - other.x, y - other.y, z - other.z);
        }
        vec3 operator-=(const vec3 &other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        vec3 operator-(T scalar) const
        {
            return vec3(x - scalar, y - scalar, z - scalar);
        }

        vec3 operator*(const vec3 &other) const
        {
            return vec3(x * other.x, y * other.y, z * other.z);
        }
        vec3 operator*=(const vec3 &other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        vec3 operator*(T scalar) const
        {
            return vec3(x * scalar, y * scalar, z * scalar);
        }

        vec3 operator/(const vec3 &other) const
        {
            return vec3(x / other.x, y / other.y, z / other.z);
        }
        vec3 operator/=(const vec3 &other)
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }
        vec3 operator/(T scalar) const
        {
            return vec3(x / scalar, y / scalar, z / scalar);
        }

        bool operator==(const vec3<T> &other) const
        {
            constexpr T epsilon = static_cast<T>(1.0E-4);
            if constexpr (std::is_floating_point_v<T>)
            {
                return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon && std::fabs(z - other.z) < epsilon;
            }
            else
            {
                return x == other.x && y == other.y && z == other.z;
            }
        }

        bool operator!=(const vec3<T> &other) const
        {
            return !(*this == other);
        }
    };

    using vec3i = vec3<int>;
    using vec3f = vec3<float>;
    using vec3u = vec3<unsigned int>;
}

