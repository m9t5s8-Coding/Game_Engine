#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_log() {
  auto&      lua = ScriptManager::get_lua();
  sol::table log = lua.create_named_table("Log");

#ifdef AERO_SERVER

  log.set_function("info", [](const std::string& msg) { AERO_SERVER_INFO("{0}", msg); });

  log.set_function("warn", [](const std::string& msg) { AERO_SERVER_WARN("{0}", msg); });

  log.set_function("error", [](const std::string& msg) { AERO_SERVER_ERROR("{0}", msg); });

  log.set_function("trace", [](const std::string& msg) { AERO_SERVER_TRACE("{0}", msg); });

  log.set_function("fatal", [](const std::string& msg) { AERO_SERVER_FATAL("{0}", msg); });

  log.set_function("assert",
                   [](bool x, const std::string& msg) { AERO_SERVER_ASSERT(x, "{0}", msg); });

#else

  // Runtime clients (including Android) log through the shared client logger,
  // which is routed to logcat by Log::init_android().
  log.set_function("info", [](const std::string& msg) { AERO_INFO("{0}", msg); });
  log.set_function("warn", [](const std::string& msg) { AERO_WARN("{0}", msg); });
  log.set_function("error", [](const std::string& msg) { AERO_ERROR("{0}", msg); });
  log.set_function("trace", [](const std::string& msg) { AERO_TRACE("{0}", msg); });
  log.set_function("fatal", [](const std::string& msg) { AERO_FATAL("{0}", msg); });
  log.set_function("assert", [](bool x, const std::string& msg) {
    if (!x)
      AERO_FATAL("Assertion failed: {0}", msg);
  });

#endif
}

}  // namespace ag
