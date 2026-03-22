// AndroidSink.hpp
#pragma once

#ifdef PLATFORM_ANDROID

  #include <android/log.h>
  #include <spdlog/sinks/base_sink.h>

template <typename Mutex>
class AndroidSink : public spdlog::sinks::base_sink<Mutex>
{
protected:
  void sink_it_(const spdlog::details::log_msg& msg) override
  {
    spdlog::memory_buf_t formatted;
    this->formatter_->format(msg, formatted);

    int priority = ANDROID_LOG_INFO;

    switch (msg.level)
    {
      case spdlog::level::trace:
      case spdlog::level::debug:
        priority = ANDROID_LOG_DEBUG;
        break;
      case spdlog::level::info:
        priority = ANDROID_LOG_INFO;
        break;
      case spdlog::level::warn:
        priority = ANDROID_LOG_WARN;
        break;
      case spdlog::level::err:
        priority = ANDROID_LOG_ERROR;
        break;
      case spdlog::level::critical:
        priority = ANDROID_LOG_FATAL;
        break;
      default:
        break;
    }

    __android_log_write(priority, "AERO", fmt::to_string(formatted).c_str());
  }

  void flush_() override
  {
  }
};

using AndroidSink_mt = AndroidSink<std::mutex>;

#endif
