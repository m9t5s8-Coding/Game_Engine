#pragma once

#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define AERO_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace ag
{
  using AG_uint = uint32_t;

  template<typename T>
  using AG_ref = std::shared_ptr<T>;

  template<typename T>
  using AG_scope = std::unique_ptr<T>;

  template<typename T, typename...Args>
  constexpr AG_ref<T> AG_cref(Args&&...args)
  {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template<typename T, typename...Args>
  constexpr AG_scope<T> AG_cscope(Args&&...args)
  {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  enum class EngineMode
  {
    Editor, Runtime
  };
  class Engine
  {
  public:
    inline static bool is_runtime() { return s_mode == EngineMode::Runtime; }
    static void start_runtime() { s_mode = EngineMode::Runtime; }
    static void stop_runtime() { s_mode = EngineMode::Editor; }

  private:
    inline static EngineMode s_mode = EngineMode::Editor;
  };
}
