#pragma once

namespace ag
{
	struct ScriptComponent
	{
		virtual void update(float dt) = 0;
	};
}