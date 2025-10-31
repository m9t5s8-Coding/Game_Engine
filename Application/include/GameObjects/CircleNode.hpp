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

			static json save(Entity entity)
			{
				json j;
				const auto& props = entity.get_component<CircleProp>();

				j["Size"] = props.size.save();
				j["Fill Color"] = props.fill_color.save();
				j["Border Color"] = props.border_color.save();
				j["Border Thickness"] = props.border_thickness;

				return j;
			}
		
			static void load(Entity entity,const json& j)
			{
				auto& props = entity.get_component<CircleProp>();
				props.size.load(j["Size"]);
				props.fill_color.load(j["Fill Color"]);
				props.border_color.load(j["Border Color"]);
				props.border_thickness = j["Border Thickness"].get<float>();
			}
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

		static void clone_node(Entity original, Entity clone)
		{
			clone.add_component<Transform>(original.get_component<Transform>());
			clone.add_component<CircleProp>(original.get_component<CircleProp>());
		}

		static json save(Entity entity)
		{
			json j;
			j["CircleProps"] = CircleProp::save(entity);
			j["Transform"] = Transform::save(entity);

			return j;
		}

		static void load(Entity entity, const json& j)
		{
			CircleProp::load(entity, j["CircleProps"]);
			Transform::load(entity, j["Transform"]);
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