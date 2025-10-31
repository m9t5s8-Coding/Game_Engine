#include <Sandbox2D.hpp>
#include <AppSettings.hpp>

class Sandbox final : public ag::Application
{
public:
  Sandbox()
  {
    push_layer(new Sandbox2D());
  }
  ~Sandbox() override {};

  virtual void on_create() override
  {
    get().get_window().set_size({ 650, 720 });
    get().get_window().center_window();
    //get().get_window().show_decoration(false);
  }

  virtual void on_destroy() override
  {

  }
};

class Editor : public ag::Application
{
public:
  Editor()
  {

  }
  ~Editor() override {}

  virtual void on_create() override
  {
    get().get_window().set_size({ 1280, 720 });
    get().get_window().center_window();
  }
  virtual void on_destroy() override
  {
    json j;
    std::ifstream in_file(AppSettings::get_settings_path());
    if (in_file.is_open())
      in_file >> j;

    in_file.close();

    j["Mode"] = "ProjectManager";

    std::ofstream out_file(AppSettings::get_settings_path());
    out_file << j.dump(4);
    out_file.close();
  }
};

ag::Application *ag::create_application()
{
  AppSettings::load();

  AppSettings::Mode mode = AppSettings::get_mode();
  switch (mode)
  {
    case AppSettings::Mode::Editor:
    {
      return new Editor();
    }
    case AppSettings::Mode::ProjectManager:
    {
      return new Sandbox();
    }
    default:
    {
      return new Sandbox();
    }
  }
}
