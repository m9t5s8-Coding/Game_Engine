#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	struct CircleNode
	{
		struct CircleProp
		{
			vec2u size{ 50, 50 };
			Color fill_color = Color::White;

			float border_thickness = 0.0f;
			Color border_color = Color::White;
		};

		static void create_node(Entity entity)
		{
			entity.add_component<Transform>();
			entity.add_component<CircleProp>();
		}

		static void delete_node(Entity entity)
		{
			entity.delete_entity();
		}

		static void show_properties(Entity entity)
		{
			{
				Tag::show_properties(entity);
				{
					vec2u& size = entity.get_component<CircleProp>().size;
					UI::draw_vec2("Size", size);
					Color& color = entity.get_component<CircleProp>().fill_color;
					UI::draw_color("Fill Color", color);
				}
				Transform::show_properties(entity);
			}
		}

		static void draw(Entity entity, TimeStamp ts)
		{
			auto& transform = entity.get_component<Transform>();
			auto& c = entity.get_component<CircleProp>();
			Circle circle;
			circle.size = c.size;
			circle.fill_color = c.fill_color;
			Renderer2D::draw_circle(circle, transform);
		}

	};

}