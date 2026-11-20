#include <Aero.hpp>

#include <Scripting/ScriptBinding/ScriptBinding.hpp>

#ifdef PLATFORM_ANDROID
  #include <Platform/Android/AndroidWindow.hpp>
#endif

namespace ag {
namespace {

bool is_android_platform() {
#ifdef PLATFORM_ANDROID
  return true;
#else
  return false;
#endif
}

std::string platform_name() {
#if defined(PLATFORM_ANDROID)
  return "Android";
#elif defined(PLATFORM_WEB) || defined(EMSCRIPTEN)
  return "Web";
#elif defined(PLATFORM_WINDOWS)
  return "Windows";
#elif defined(PLATFORM_LINUX)
  return "Linux";
#else
  return "Unknown";
#endif
}

vec2f screen_size() {
  const vec2u size = Application::get().get_window().get_size();
  return vec2f(size);
}

size_t touch_count() {
#ifdef PLATFORM_ANDROID
  return static_cast<AndroidWindow&>(Application::get().get_window()).get_touch_count();
#else
  return 0;
#endif
}

bool touch_is_down(int index) {
#ifdef PLATFORM_ANDROID
  if (index < 0)
    return false;
  auto& window = static_cast<AndroidWindow&>(Application::get().get_window());
  return window.get_touch_point(static_cast<size_t>(index)) != nullptr;
#else
  (void)index;
  return false;
#endif
}

int touch_id(int index) {
#ifdef PLATFORM_ANDROID
  if (index < 0)
    return -1;
  auto&                  window = static_cast<AndroidWindow&>(Application::get().get_window());
  const TouchPoint* point = window.get_touch_point(static_cast<size_t>(index));
  return point ? point->id : -1;
#else
  (void)index;
  return -1;
#endif
}

vec2f touch_position(int index) {
#ifdef PLATFORM_ANDROID
  if (index < 0)
    return vec2f(0.0f, 0.0f);
  auto&                  window = static_cast<AndroidWindow&>(Application::get().get_window());
  const TouchPoint* point = window.get_touch_point(static_cast<size_t>(index));
  return point ? point->position : vec2f(0.0f, 0.0f);
#else
  (void)index;
  return vec2f(0.0f, 0.0f);
#endif
}

float pinch_distance() {
#ifdef PLATFORM_ANDROID
  return static_cast<AndroidWindow&>(Application::get().get_window()).get_pinch_distance();
#else
  return 0.0f;
#endif
}

bool is_pinching() {
#ifdef PLATFORM_ANDROID
  return static_cast<AndroidWindow&>(Application::get().get_window()).is_pinching();
#else
  return false;
#endif
}

vec2f world_from_screen(const vec2f& screen_position) {
  const vec2f viewport_position = Mouse::window_to_viewport(screen_position);
  const auto  controller        = ViewController::get_main_controller();
  if (!controller)
    return viewport_position;

  return Mouse::get_mouse_world_position(viewport_position,
                                         controller->get_view(),
                                         Mouse::get_viewport().framebuffer_size);
}

}  // namespace

void ScriptBinding::register_platform() {
  auto& lua      = ScriptManager::get_lua();
  auto  platform = lua.create_named_table("Platform");

  platform.set_function("name", &platform_name);
  platform.set_function("is_android", &is_android_platform);
  platform.set_function("is_mobile", &is_android_platform);
  platform.set_function("is_active", []() { return Application::get().is_app_active(); });
  platform.set_function("is_paused", []() { return !Application::get().is_app_active(); });
  platform.set_function("get_screen_size", &screen_size);

  // Touch is exposed directly instead of forcing mobile scripts to pretend
  // every finger is a mouse. Mouse.* remains compatible with the primary touch.
  auto touch = lua.create_named_table("Touch");
  touch.set_function("count", &touch_count);
  touch.set_function("is_down", &touch_is_down);
  touch.set_function("get_id", &touch_id);
  touch.set_function("get_position", &touch_position);
  touch.set_function("get_viewport_position",
                     [](int index) { return Mouse::window_to_viewport(touch_position(index)); });
  touch.set_function("get_world_position",
                     [](int index) { return world_from_screen(touch_position(index)); });
  touch.set_function("get_pinch_distance", &pinch_distance);
  touch.set_function("is_pinching", &is_pinching);
}

}  // namespace ag
