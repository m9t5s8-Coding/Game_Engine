#include <EntryPoint.hpp>
#include <Log.hpp>
#include <iostream>

int main(int args, char **argv)
{
  aero::Log::init();
  const auto app = aero::create_application();
  app->run();
  delete app;

  return 0;
}
