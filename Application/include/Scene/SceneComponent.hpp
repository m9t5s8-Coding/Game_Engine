#pragma once

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Renderer/View.hpp>
#include <Scene/Entity.hpp>
#include <Project/Project.hpp>
#include <string>
#include <cstdint>
#include <Project/FileDialogs.hpp>
#include <Helper.hpp>
#include <Scripting/LuaEnv.hpp>
#include <Scripting/LuaFunc.hpp>
#include <Scripting/ScriptableEntity.hpp>
#include <Core/Core.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>
#include <Renderer/Text.hpp>
#include <Renderer/Texture.hpp>

namespace ag
{
	static std::string default_path = "assets/textures/";
	inline std::unordered_map<AG_uint, Entity> index_map;

	enum class RenderMode
	{
		Screen = 0,
		World = 1
	};

	enum class NodeType
	{
		None = -1,
		Rectangle = 0,
		Circle = 1,
		Sprite = 2,
		AnimatedSprite2D = 3,
		Camera = 4,
		TileMap = 5,
		Scene2D = 6,
		Text = 7,
		Button = 8,
		TextureButton = 9,
		CollisionShape = 10,
		Audio = 11,
	};

	enum class Node_Capability : uint32_t
	{
		None = 0,
		Render2D = 1 << 0,
		Physics2D = 1 << 1,
		Scriptable = 1 << 2,
		UI = 1 << 3,
		RectShape = 1 << 4,
		CircleShape = 1 << 5,
		Texture2D = 1 << 6,
		Transform = 1 << 7,
		Camera = 1 << 8,
		TileMap = 1 << 9,
		Text = 1 << 10,
	};

	enum class RenderLayer
	{
		BackGround,
		ForeGround,
		MidGround
	};

	struct Rectangle
	{
		vec2f size;
		Color fill_color = Color::White;
		float border_thickness = 0.0f;
		float corner_radius = 0.0f;
		Color border_color;
		RenderMode mode = RenderMode::World;
	};

	struct Circle
	{
		vec2f size;
		Color fill_color = Color::White;
		float border_thickness = 0.0f;
		Color border_color;
		RenderMode mode = RenderMode::World;
	};

	struct Sprite
	{
		vec2f size;
		uint_rect texture_rect;
		Color fill_color = Color::White;
		RenderMode mode = RenderMode::World;
		bool flip_horizontal = false;
		bool flip_vertical = false;
	};

	
}
