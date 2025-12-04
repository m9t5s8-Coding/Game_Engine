#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	struct ButtonNode
	{
		struct ButtonProps
		{
			bool hovered = false;
			bool pressed = false;

			Color normal_color;
			Color hover_color;
			Color active_color;

			Color normal_text_color;
			Color hover_text_color;
			Color active_text_color;
		};
		
	};


}