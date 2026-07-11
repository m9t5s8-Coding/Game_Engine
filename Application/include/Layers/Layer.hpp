#pragma once

#include <Apch.hpp>
#include <Core/TimeStamp.hpp>
#include <Events/Event.hpp>

namespace ag {
class Layer {
public:
  Layer(const std::string& name = "Layer");
  virtual ~Layer();

  virtual void on_attach() {}
  virtual void on_detach() {}
  virtual void on_update(TimeStamp ts) {}
  virtual void on_event(Event& event) {}
  virtual void on_imgui_render() {}

  inline const std::string& get_name() const { return m_debug_name; }

private:
  std::string m_debug_name;
};
}  // namespace ag
