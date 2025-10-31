#pragma once

#include <nlohmann/json.hpp>
#include <Math/Math.hpp>
#include <string>
using json = nlohmann::json;

namespace ag::Helper
{
  template <typename T>
  void load_json(const json& j, const std::string& key, T& value)
  {
    if (j.contains(key))
      value = j[key].get<T>();
  }

  template <typename T>
  void load_json(const json& j, const std::string& key, vec2<T>& value)
  {
    if (j.contains(key))
    {
      value.load(j[key]);
    }
  }

  template <typename T>
  void load_json(const json& j, const std::string& key, rect<T>& value)
  {
    if (j.contains(key))
    {
      value.load(j[key]);
    }
  }


  template <typename T>
  void save_json(json& j, const std::string& key, const T& value)
  {
    j[key] = value;
  }

  template <typename T>
  void save_json(json& j, const std::string& key, const vec2<T>& value)
  {
    j[key] = value.save();;
  }

  template <typename T>
  void save_json(json& j, const std::string& key, const rect<T>& value)
  {
    j[key] = value.save();;
  }


  inline std::string normalize_path(const std::string& path)
  {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
  }
  inline std::string denormalize_path(const std::string& path)
  {
    std::string result = path;
    std::replace(result.begin(), result.end(), '/', '\\');
    return result;
  }

}