#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

class Application
{
public:
  Application();
  ~Application();

  void Init(int width, int height, const char *title);
  void Run();
  void ShutDown();

private:
  struct Application_Impl;
  std::unique_ptr<Application_Impl> impl;
};

#endif // APPLICATION_H
