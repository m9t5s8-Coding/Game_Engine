#pragma once

#include <GameObjects/Components/Components.hpp>

namespace ag
{
	struct CircleNode
	{
		struct Circle_Component
		{
			vec2i size{ 100, 100 };
			Color color = Color::White;

			static json save_json(Entity entity);
			static void load_json(Entity entity, const json& j);
		};

		static void create_node(Entity entity);
		static void delete_node(Entity entity);
		static void clone_node(Entity original, Entity clone);
		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);
		static void update(Entity entity, TimeStamp ts);
		static void draw(Entity entity);
	};

}