#include <Core/EntryPoint.hpp>
#include <Core/Log.hpp>
#include <iostream>


//This is an entry point main program where I can create program by inheriting the application class
int main(int args, char **argv)
{
  ag::Log::init();
  const auto app = ag::create_application();
  app->run();
  delete app;

  return 0;
}
