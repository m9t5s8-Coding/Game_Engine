#pragma once

#include <cmath>
#include <type_traits>

namespace ag
{
  template <typename T>
  struct vec4
  {
    T x, y, z, w;


    vec4(T x_ = T(), T y_ = T(), T z_ = T(), T w_ = T())
        : x(x_), y(y_), z(z_), w(w_) {}

    vec4(T scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}

    vec4 round() const
    {
      return {std::round(x), std::round(y), std::round(z), std::round(w)};
    }

    template <typename U>
    vec4(const vec4<U> &v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)),
          z(static_cast<T>(v.z)),
          w(static_cast<T>(v.w)) {}

    // Arithmetic operators
    vec4 operator+(const vec4 &other) const
    {
      return {x + other.x, y + other.y, z + other.z, w + other.w};
    }
    vec4 operator-(const vec4 &other) const
    {
      return {x - other.x, y - other.y, z - other.z, w - other.w};
    }
    vec4 operator*(const vec4 &other) const
    {
      return {x * other.x, y * other.y, z * other.z, w * other.w};
    }
    vec4 operator/(const vec4 &other) const
    {
      return {x / other.x, y / other.y, z / other.z, w / other.w};
    }

    vec4 &operator+=(const vec4 &other)
    {
      x += other.x;
      y += other.y;
      z += other.z;
      w += other.w;
      return *this;
    }
    vec4 &operator-=(const vec4 &other)
    {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      w -= other.w;
      return *this;
    }
    vec4 &operator*=(const vec4 &other)
    {
      x *= other.x;
      y *= other.y;
      z *= other.z;
      w *= other.w;
      return *this;
    }
    vec4 &operator/=(const vec4 &other)
    {
      x /= other.x;
      y /= other.y;
      z /= other.z;
      w /= other.w;
      return *this;
    }

    // Scalar operators
    vec4 operator+(T scalar) const
    {
      return {x + scalar, y + scalar, z + scalar, w + scalar};
    }
    vec4 operator-(T scalar) const
    {
      return {x - scalar, y - scalar, z - scalar, w - scalar};
    }
    vec4 operator*(T scalar) const
    {
      return {x * scalar, y * scalar, z * scalar, w * scalar};
    }
    vec4 operator/(T scalar) const
    {
      return {x / scalar, y / scalar, z / scalar, w / scalar};
    }

    vec4 &operator+=(T scalar)
    {
      x += scalar;
      y += scalar;
      z += scalar;
      w += scalar;
      return *this;
    }
    vec4 &operator-=(T scalar)
    {
      x -= scalar;
      y -= scalar;
      z -= scalar;
      w -= scalar;
      return *this;
    }
    vec4 &operator*=(T scalar)
    {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      w *= scalar;
      return *this;
    }
    vec4 &operator/=(T scalar)
    {
      x /= scalar;
      y /= scalar;
      z /= scalar;
      w /= scalar;
      return *this;
    }

    // Comparison
    bool operator==(const vec4 &other) const
    {
      if constexpr (std::is_floating_point_v<T>)
      {
        constexpr T eps = static_cast<T>(1.0E-4);
        return std::fabs(x - other.x) < eps &&
               std::fabs(y - other.y) < eps &&
               std::fabs(z - other.z) < eps &&
               std::fabs(w - other.w) < eps;
      }
      else
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const vec4 &other) const { return !(*this == other); }
  };

  using vec4f = vec4<float>;
  using vec4i = vec4<int>;
  using vec4u = vec4<unsigned int>;
}
