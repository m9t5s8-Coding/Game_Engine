#include <Core/EntryPoint.hpp>
#include <Core/Log.hpp>
#include <iostream>


//This is an entry point main program where I can create program by inheriting the application class
int main(int args, char **argv)
{
  aero::Log::init();
  const auto app = aero::create_application();
  app->run();
  delete app;

  return 0;
}
