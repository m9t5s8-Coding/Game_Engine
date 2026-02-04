#pragma once

namespace ag
{
	class AudioAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenAL = 1,
			OpenSL = 2,
		};
		inline static API get_api() { return s_API; }
	private:
		static API s_API;
	};
}