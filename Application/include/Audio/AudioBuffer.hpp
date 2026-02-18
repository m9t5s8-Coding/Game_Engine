#pragma once
#include <string>

namespace ag
{
class AudioBuffer
{
   public:
    virtual ~AudioBuffer() {};

    virtual AG_uint get_id() const = 0;
    virtual const std::string& get_path() const = 0;

    virtual void delete_buffers() = 0;

    static AG_ref<AudioBuffer> create(const std::string& path);
};
}  // namespace ag
