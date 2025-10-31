#include <Application/ProjectManagerApp.hpp>
#include <Application/EditorApp.hpp>

namespace ag
{
  Application* create_application()
  {
    AppSettings::load();

    AppSettings::Mode mode = AppSettings::get_mode();
    switch (mode)
    {
    case AppSettings::Mode::Editor:
    {
      return new EditorApp();
    }
    case AppSettings::Mode::ProjectManager:
    {
      return new ProjectManagerApp();
    }
    default:
    {
      return new ProjectManagerApp();
    }
    }
  }
}