#include <Application/AppSettings.hpp>
#include <Application/ProjectManagerApp.hpp>
#include <Core/Log.hpp>

namespace ag {
ProjectManagerApp::ProjectManagerApp() { push_layer(new ProjectManagerLayer()); }

void ProjectManagerApp::on_create() {
#ifndef PLATFORM_ANDROID
  WindowProps props;
  props.Size  = vec2u(650, 720);
  props.Title = "Project Manager";

  const WindowState window_state = AppSettings::get_window_state();
  if (window_state.has_size)
    props.Size = {static_cast<AG_uint>(window_state.width),
                  static_cast<AG_uint>(window_state.height)};

  init(props);
  AppSettings::restore_window_state();
#endif
}

void ProjectManagerApp::on_destroy() {
#ifndef PLATFORM_ANDROID
  AppSettings::save_current_window_state();
#endif
}
}  // namespace ag
