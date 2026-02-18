#pragma once

#include <nlohmann/json.hpp>
#include <Math/Math.hpp>
#include <algorithm>
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
  inline void load_json(const json& j, const std::string& key, T& value)
  {
    if (j.contains(key))
      value = j[key].get<T>();
  }

  template <typename T>
  inline void load_json(const json& j, const std::string& key, vec2<T>& value)
  {
    if (j.contains(key))
    {
      value.load(j[key]);
    }
  }

  template <typename T>
  inline void load_json(const json& j, vec2<T>& value)
  {
     value.load(j);
  }

  template <typename T>
  inline void load_json(const json& j, T& value)
  {
    value = j.get<T>();
  }

  template <typename T>
  inline void load_json(const json& j, rect<T>& value)
  {
    value.load(j);
  }

  template <typename T>
  inline void load_json(const json& j, const std::string& key, rect<T>& value)
  {
    if (j.contains(key))
    {
      value.load(j[key]);
    }
  }

  inline void load_json(const json& j, const std::string& key, Color& color)
  {
    if (j.contains(key))
    {
      color.load(j[key]);
    }
  }




  template <typename T>
  inline void save_json(json& j, const std::string& key, const T& value)
  {
    j[key] = value;
  }

  template <typename T>
  inline void save_json(json& j, const std::string& key, const vec2<T>& value)
  {
    j[key] = value.save();;
  }

  template <typename T>
  inline void save_json(json& j, const std::string& key, const rect<T>& value)
  {
    j[key] = value.save();;
  }

  inline void save_json(json& j, const std::string& key, const Color& color)
  {
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
    if(read_only)
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




}
