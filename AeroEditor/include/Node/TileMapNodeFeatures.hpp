#pragma once

#include <Aero.hpp>

namespace ag
{
	class TileMapNodeFeatures
	{
	public:
		static void tilemap_editor();

		static void texture_selector_gui(const AG_ref<Texture2D>& texture, uint_rect& texture_rect);
	};
}