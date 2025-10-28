#pragma once

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Renderer/View.hpp>
#include <Scene/Entity.hpp>
#include <string>
#include <UI/DrawUI.hpp>
#include <cstdint>

namespace ag
{
	static std::string default_path = "assets/textures/";


	enum class NodeType
	{
		Rectangle,
		Circle,
		Sprite,
		AnimatedSprite2D
	};

	enum class RenderLayer
	{
		BackGround,
		ForeGround,
		MidGround
	};

	// SceneComponents
	struct Tag
	{
		std::string tag;
		uint32_t index = 0;
		RenderLayer layer;
		NodeType node_type;

		static void show_properties(Entity entity)
		{
			auto& tag = entity.get_component<Tag>();
			UI::draw_string("Tag", tag.tag);
		}
	};
	struct Transform
	{
		vec2f position;
		vec2f scale = { 1, 1 };
		vec2u origin;
		float rotation = 0.0f;

		static void show_properties(Entity entity)
		{
			UI::draw_title("Transform");
			auto& transform = entity.get_component<Transform>();
			uint32_t id = entity.get_id();
			UI::draw_vec2("Position", transform.position, { 0, 0 });
			UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });
			UI::draw_vec2("Origin", transform.origin, { 0, 0 });
			UI::draw_value("Rotation", transform.rotation);
		}
	};

	struct Rectangle
	{
		vec2u size;
		Color fill_color;
	};

	struct Circle
	{
		vec2u size;
		Color fill_color;
	};

	struct Sprite
	{
		vec2u size;
		uint_rect texture_rect;
	};

}
