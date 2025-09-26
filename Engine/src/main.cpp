#include <Application.hpp>

int main()
{
  Application app;
  app.Init(1280, 720, "First Window");
  app.Run();
  app.ShutDown();
  return 0;
}
