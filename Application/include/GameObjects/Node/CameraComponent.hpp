#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <Helper.hpp>

namespace ag
{
	struct CameraComponent
	{
		struct CameraProps
		{
			vec2f view_size;
			vec2f view_center;

			float zoom = 1.0f;
			bool is_active = false;

			static json save(Entity entity)
			{
				const auto& props = entity.get_component<CameraProps>();
				json j;
				Helper::save_json(j, "View Size", props.view_size);
				Helper::save_json(j, "View Center", props.view_center);
				Helper::save_json(j, "Zoom", props.zoom);
				Helper::save_json(j, "IsActive", props.is_active);
				return j;
			}

			static void load(Entity entity, const json& j)
			{
				auto& props = entity.get_component<CameraProps>();
				Helper::load_json(j,"View Size", props.view_size);
				Helper::load_json(j,"View Center", props.view_center);
				Helper::load_json(j,"Zoom", props.zoom);
				Helper::load_json(j,"IsActive", props.is_active);
			}
		};

		static void create_node(Entity entity)
		{
			if (!entity.has_component<CameraProps>())
			{
				CameraProps props;
				props.view_size = vec2f(1280, 720);
				props.view_center = props.view_size / 2;
				entity.add_component<CameraProps>(props);
			}
		}

		static void delete_node(Entity entity)
		{
			entity.delete_entity();
		}

		static void clone_node(Entity original, Entity clone)
		{
			if (original.has_component<CameraProps>())
			{
				const auto& source = original.get_component<CameraProps>();
				clone.add_component<CameraProps>(source);
			}
		}

		static json save_json(Entity entity)
		{
			json j;
			j["CameraProps"] = CameraProps::save(entity);
			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			CameraProps::load(entity, j["CameraProps"]);
		}

		static void show_properties(Entity entity)
		{
      Tag::show_properties(entity);
			auto& props = entity.get_component<CameraProps>();
			UI::draw_vec2("View Size", props.view_size,{1280, 720});
			UI::draw_vec2("View Center", props.view_center, {640, 360});
			UI::draw_value("Zoom", props.zoom);
			ImGui::Checkbox("IsActive", &props.is_active);
		}

		static void draw(Entity entity, TimeStamp ts)
		{

		}
	};
}
