#pragma once

#include <algorithm>
#include <Math/Math.hpp>
#include <nlohmann/json.hpp>
#include <Renderer/Color.hpp>
#include <string>

#ifdef PLATFROM_WINDOWS
  #include <Windows.h>

#elif defined(PLATFORM_LINUX)
  #include <sys/stat.h>
#endif

using json = nlohmann::json;

namespace ag::Helper
{
template <typename T>
inline void load_json(const json& j, const std::string& key, T& value, const T& default_value)
{
  if (j.contains(key))
  {
    value = j[key].get<T>();
    return;
  }
  value = default_value;
}

template <typename T>
inline void
load_json(const json& j, const std::string& key, vec2<T>& value, const vec2<T>& default_value)
{
  if (j.contains(key))
  {
    value.load(j[key]);
    return;
  }
  value = default_value;
}

template <typename T>
inline void load_json(const json& j, vec2<T>& value, const vec2<T>& default_value)
{
  if (j)
  {
    value.load(j);
    return;
  }
  value = default_value;
}

template <typename T>
inline void load_json(const json& j, T& value, const T& default_value)
{
  if (j)
  {
    value = j.get<T>();
    return;
  }
  value = default_value;
}

template <typename T>
inline void load_json(const json& j, rect<T>& value, const rect<T>& default_value)
{
  if (j)
  {
    value.load(j);
    return;
  }
  value = default_value;
}

template <typename T>
inline void
load_json(const json& j, const std::string& key, rect<T>& value, const rect<T>& default_value)
{
  if (j.contains(key))
  {
    value.load(j[key]);
    return;
  }
  value = default_value;
}

inline void
load_json(const json& j, const std::string& key, Color& color, const Color& default_value)
{
  if (j.contains(key))
  {
    color.load(j[key]);
    return;
  }
  color = default_value;
}

template <typename T>
inline void save_json(json& j, const std::string& key, const T& value, const T& default_value)
{
  if (value != default_value)
    j[key] = value;
}

template <typename T>
inline void
save_json(json& j, const std::string& key, const vec2<T>& value, const vec2<T>& default_value)
{
  if (value != default_value)
    j[key] = value.save();
}

template <typename T>
inline void
save_json(json& j, const std::string& key, const rect<T>& value, const rect<T>& default_value)
{
  if (value != default_value)
    j[key] = value.save();
}

inline void
save_json(json& j, const std::string& key, const Color& color, const Color& default_value)
{
  if (color != default_value)
    j[key] = color.save();
}

inline void normalize_path(std::string& path)
{
  std::replace(path.begin(), path.end(), '\\', '/');
}

inline std::string denormalize_path(const std::string& path)
{
  std::string result = path;
  std::replace(result.begin(), result.end(), '/', '\\');
  return result;
}

inline void makefile_read_only(const std::string& path, bool read_only = true)
{
#ifdef PLATFORM_WINDOWS
  if (read_only)
    SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_READONLY);
  else
    SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_NORMAL);
#elif defined(PLATFORM_LINUX)
  if (read_only)
    chmod(path.c_str(), S_IRUSR | S_IRGRP | S_IROTH);
  else
    chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
}

}  // namespace ag::Helper
