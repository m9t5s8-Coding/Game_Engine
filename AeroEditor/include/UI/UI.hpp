#pragma once

#include <imgui.h>
#include <string>
#include <type_traits>
#include <Math/Math.hpp>

class Texture;
namespace ag::UI
{
	

	void draw_menu_bar();

	bool texture_selector(const AG_ref<Texture>& texture, uint_rect& texture_rect);
}
