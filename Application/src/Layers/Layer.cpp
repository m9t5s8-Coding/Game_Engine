#include <Layers/Layer.hpp>

namespace ag
{
  Layer::Layer(const std::string &name)
      : m_debug_name(name) {}

  Layer::~Layer() {}
}
