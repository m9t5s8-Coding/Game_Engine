
#include "Application/AppSettings.hpp"

#include <Application/EditorApp.hpp>
#include <Application/ProjectManagerApp.hpp>
#include <Core/Log.hpp>

namespace ag {

Application* create_application() {
  AppSettings::load();

  AppSettings::Mode mode = AppSettings::get_mode();
  switch (mode) {
    case AppSettings::Mode::Editor: return new EditorApp();
    case AppSettings::Mode::ProjectManager:
    default: return new ProjectManagerApp();
  }
}
}  // namespace ag
