#pragma once
#include <Scripting/LuaEnv.hpp>
#include <Core/Log.hpp>

namespace ag
{
	class LuaFunc
	{
	public:
		LuaFunc() = default;
		LuaFunc(sol::function func) : m_func(func) {}

		void set_function(LuaEnv& env, const std::string& func_name)
		{
			auto& e = env.get();
			sol::object obj = e[func_name];

			if (obj.valid() && obj.get_type() == sol::type::function)
				m_func = obj.as<sol::function>();
			else
				m_func = sol::function();
		}

		template <typename... Args>
		void call(Args&&... args)
		{
			if (!m_func.valid())
				return;

			sol::protected_function_result result = m_func(std::forward<Args>(args)...);

			if (!result.valid())
			{
				sol::error err = result;
				AERO_CORE_INFO("[Lua Error] {0}", err.what());
			}
		}

		bool is_valid() const { return m_func.valid(); }

	private:
		sol::function m_func;
	};
}