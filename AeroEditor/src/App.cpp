
#include "Application/AppSettings.hpp"

#include <Application/EditorApp.hpp>
#include <Application/ProjectManagerApp.hpp>
#include <Core/Application.hpp>
#include <Core/Log.hpp>
#include <Project/Project.hpp>

namespace ag {

Application* create_application() {
  AppSettings::load();

  // Project selection is argument driven. `AeroEditor <folder>` and
  // `AeroEditor <file.aeroproj>` both open the editor directly; anything else
  // opens the project manager where every launch path supplies the same
  // argument.
  const std::string project_argument = Application::get_project_path_argument();
  if (!project_argument.empty())
  {
    auto project = Project::load_project(project_argument);
    if (project && project->m_project_loaded)
    {
      AERO_CORE_INFO("Loading project from command line: {0}",
                     project->get_project_file_directory());
      return new EditorApp(project->get_project_file_directory());
    }

    AERO_CORE_ERROR("Command line project could not be loaded: {0}", project_argument);
  }

  return new ProjectManagerApp();
}
}  // namespace ag
