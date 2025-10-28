#include <EditorLayer.hpp>

namespace ag
{
  class EditorApp final : public Application
  {
  public:
    EditorApp()
    {
      push_layer(new EditorLayer());
    }
    ~EditorApp() override {};
    
  };

  Application* create_application()
  {
    return new EditorApp();
  }
}