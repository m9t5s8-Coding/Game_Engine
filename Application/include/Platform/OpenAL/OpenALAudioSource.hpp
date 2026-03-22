#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)

  #pragma once

  #include <Audio/AudioSource.hpp>
  #include <Math/Math.hpp>

namespace ag
{
class OpenALAudioSource : public AudioSource
{
public:
  OpenALAudioSource();
  virtual ~OpenALAudioSource();

  virtual void set_buffer(const AG_ref<AudioBuffer>& buffer) const override;
  virtual void set_buffer(AG_uint buffer_id) const override;
  virtual void play() const override;
  virtual void pause() const override;
  virtual bool is_paused() const override;
  virtual void stop() const override;
  virtual bool is_playing() const override;

  virtual void set_loop(bool loop) override;
  virtual bool is_looping() const override
  {
    return m_looping;
  };
  virtual void  set_volume(float volume) const override;
  virtual float get_volume() const override;

  virtual void  set_pitch(float pitch) const override;
  virtual float get_pitch() const override;
  virtual void  set_position(const vec2f& position) const override;

private:
  AG_uint m_ID;
  bool    m_looping = false;
};
}  // namespace ag
#endif
