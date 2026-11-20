
#ifndef PLATFORM_ANDROID
  #include <Core/EntryPoint.hpp>
  #include <Core/Log.hpp>
  #ifndef AERO_SERVER
int main(int args, char** argv) {
    #ifdef AERO_EDITOR
  ag::Log::init_with_imGui();
    #else
  ag::Log::init_simple();
    #endif

  ag::Application::set_command_line(args, argv);
  const auto app = ag::create_application();
  app->run();
  delete app;

  return 0;
}
  #endif
#endif
