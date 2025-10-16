#pragma once

#include <memory>

#define AERO_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace ag
{
  using AG_uint = unsigned int;

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

}
