#pragma once

#include <Scripting/ScriptBinding/Signals.hpp>
#include <unordered_map>

namespace ag
{
class Signal_Manager
{
   private:
    static Signal_Manager* s_instance;
    std::unordered_map<AG_uint, AG_ref<Signals>> m_signals;
    std::unordered_map<std::string, AG_uint> m_name_to_id;

   public:
    Signal_Manager() { s_instance = this; }
    ~Signal_Manager() { s_instance = nullptr; }
    static Signal_Manager& get() { return *s_instance; }
    static AG_ref<Signals> get_signals(const std::string& name);
    static AG_ref<Signals> get_signals(AG_uint ids);
    static AG_uint get_id(const std::string& name);
    static void bind_signals();

    static AG_uint create_id(const std::string& name);
};
}  // namespace ag
