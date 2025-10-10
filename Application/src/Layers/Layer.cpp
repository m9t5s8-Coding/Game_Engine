#include <Layers/Layer.hpp>

namespace aero
{
  Layer::Layer(const std::string &name)
      : m_debug_name(name) {}

  Layer::~Layer() {}
}
