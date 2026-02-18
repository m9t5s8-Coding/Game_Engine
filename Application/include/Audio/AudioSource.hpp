#pragma once

#include <Audio/AudioBuffer.hpp>
#include <Math/Math.hpp>

namespace ag
{
class AudioSource
{
   public:
    virtual ~AudioSource() {};

    virtual void set_buffer(const AG_ref<AudioBuffer>& buffer) const = 0;
    virtual void set_buffer(AG_uint buffer_id) const = 0;
    virtual void play() const = 0;
    virtual void pause() const = 0;
    virtual bool is_paused() const = 0;
    virtual void stop() const = 0;
    virtual bool is_playing() const = 0;

    virtual void set_loop(bool loop) = 0;
    virtual bool is_looping() const = 0;

    virtual void set_volume(float volume) const = 0;
    virtual float get_volume() const = 0;

    virtual void set_pitch(float pitch) const {}
    virtual float get_pitch() const { return 1.0f; }
    virtual void set_position(const vec2f& position) const {}

    static AG_scope<AudioSource> create();
};
}  // namespace ag
