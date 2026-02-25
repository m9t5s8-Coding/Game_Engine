#pragma once

namespace ag
{
class Audio_Device
{
public:
  virtual ~Audio_Device() {};

  virtual bool init()     = 0;
  virtual void shutdown() = 0;

  static Audio_Device* create();
};
}  // namespace ag
