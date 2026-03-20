#include <Core/EntryPoint.hpp>
#include <Core/Log.hpp>
#ifndef AERO_SERVER
int main(int args, char** argv)
{
  ag::Log::init_with_console();
  const auto app = ag::create_application();
  app->run();
  delete app;

  return 0;
}
#endif
