//#pragma once
//
//#include <Math/Math.hpp>
//#include <Scene/Entity.hpp>
//#include <Renderer/Renderer2D.hpp>
//#include <Scene/SceneComponent.hpp>
//#include <Core/Application.hpp>
//
//namespace ag
//{
//	struct TextureButton
//	{
//		struct TextureButtonProps
//		{
//			bool hovered = false;
//			bool pressed = false;
//			uint_rect normal_texture_rect;
//			uint_rect hover_texture_rect;
//			uint_rect active_texture_rect;
//			std::string texture_path = "default.png";
//			AG_ref<Texture2D> texture;
//			RenderMode mode = RenderMode::Screen;
//
//			static json save_json(Entity entity)
//			{
//				json j;
//
//				auto &props = entity.get_component<TextureButtonProps>();
//				Helper::save_json(j, "Normal Texture Rect", props.normal_texture_rect);
//				Helper::save_json(j, "Hover Texture Rect", props.hover_texture_rect);
//				Helper::save_json(j, "Active Texture Rect", props.active_texture_rect);
//				Helper::save_json(j, "Texture Path", props.texture_path);
//				Helper::save_json(j, "Mode", static_cast<int>(props.mode));
//
//				return j;
//			}
//			static void load_json(Entity entity, const json &j)
//			{
//
//				auto &props = entity.get_component<TextureButtonProps>();
//				Helper::load_json(j, "Normal Texture Rect", props.normal_texture_rect);
//				Helper::load_json(j, "Hover Texture Rect", props.hover_texture_rect);
//				Helper::load_json(j, "Active Texture Rect", props.active_texture_rect);
//				Helper::load_json(j, "Texture Path", props.texture_path);
//				{
//					auto project = Project::get_active_project();
//					std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + props.texture_path;
//					props.texture = Texture2D::create(texture_path);
//				}
//				Helper::load_json(j, "Mode", props.mode);
//			}
//		};
//
//		static void create_node(Entity entity)
//		{
//			TextureButtonProps props;
//			{
//				auto project = Project::get_active_project();
//				std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + props.texture_path;
//				props.texture = Texture2D::create(texture_path);
//			}
//			props.normal_texture_rect = uint_rect(0, 0, props.texture->get_size());
//			entity.add_component<TextureButtonProps>(props);
//			entity.add_component<Transform>();
//		}
//
//		static void delete_node(Entity entity)
//		{
//			entity.delete_entity();
//		}
//
//		static void clone_node(Entity original, Entity duplicate)
//		{
//			duplicate.add_component<TextureButtonProps>(original.get_component<TextureButtonProps>());
//			duplicate.add_component<Transform>(original.get_component<Transform>());
//		}
//
//		static json save_json(Entity entity)
//		{
//			json j;
//			j["TextureButtonProps"] = TextureButtonProps::save_json(entity);
//			j["Transform"] = Transform::save(entity);
//
//			if (entity.has_component<ScriptComponent>())
//			{
//				j["ScriptComponent"] = ScriptComponent::save_json(entity);
//			}
//			return j;
//		}
//
//		static void load_json(Entity entity, const json &j)
//		{
//			TextureButtonProps::load_json(entity, j["TextureButtonProps"]);
//			Transform::load(entity, j["Transform"]);
//			if (j.contains("ScriptComponent"))
//			{
//				ScriptComponent::load_json(entity, j["ScriptComponent"]);
//			}
//		}
//
//		static void show_properties(Entity entity)
//		{
//
//			Tag::show_properties(entity);
//			auto &props = entity.get_component<TextureButtonProps>();
//
//			{
//
//				{
//					UI::draw_string("Texture Path", props.texture_path);
//					if (ImGui::Button("Load Texture"))
//					{
//						std::string full_path = FileDialogs::open_file("Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0");
//						if (!full_path.empty())
//						{
//							auto project = Project::get_active_project();
//							Helper::normalize_path(full_path);
//
//							std::string project_dir = project->get_directory();
//							std::string assets_dir = project->get_assets_directory();
//
//							std::string base_path = project_dir + assets_dir + "/";
//
//							std::string relative_path = full_path;
//							if (relative_path.find(base_path) == 0)
//								relative_path = relative_path.substr(base_path.size());
//
//							Helper::normalize_path(relative_path);
//
//							props.texture_path = relative_path;
//
//							props.texture = Texture2D::create(full_path);
//
//							vec2u texture_size = props.texture->get_size();
//							props.normal_texture_rect = {0, 0, texture_size};
//						}
//					}
//				}
//			}
//
//			UI::draw_title("Normal");
//			UI::draw_vec2("Texture Position##normal", props.normal_texture_rect.position);
//			UI::draw_vec2("Texture Size##normal", props.normal_texture_rect.size, props.texture->get_size());
//
//			UI::draw_title("Hover");
//			UI::draw_vec2("Texture Position##hover", props.hover_texture_rect.position);
//			UI::draw_vec2("Texture Size##hover", props.hover_texture_rect.size, props.texture->get_size());
//
//			UI::draw_title("Active");
//			UI::draw_vec2("Texture Position##active", props.active_texture_rect.position);
//			UI::draw_vec2("Texture Size##active", props.active_texture_rect.size, props.texture->get_size());
//
//			UI::draw_title("Render Mode");
//			if (ImGui::RadioButton("Screen", props.mode == RenderMode::Screen))
//			{
//				props.mode = RenderMode::Screen;
//			}
//			if (ImGui::RadioButton("World", props.mode == RenderMode::World))
//			{
//				props.mode = RenderMode::World;
//			}
//			Transform::show_properties(entity);
//		}
//
//		static void update(Entity entity, TimeStamp ts)
//		{
//			if (!Engine::is_runtime())
//				return;
//
//			auto& props = entity.get_component<TextureButtonProps>();
//			auto& transform = entity.get_component<Transform>();
//
//			float_rect rect;
//
//			rect.position = transform.position - (vec2f(props.normal_texture_rect.size) * transform.scale) / 2;
//			rect.size = vec2f(props.normal_texture_rect.size) * transform.scale;
//
//			auto mouse_pos = ViewController::get_mouse_position();
//
//			if (rect.contains(mouse_pos))
//			{
//				props.hovered = true;
//				props.pressed = false;
//				if (Mouse::is_mouse_pressed(Button::ButtonLeft) && props.hovered)
//				{
//					props.hovered = false;
//					props.pressed = true;
//				}
//			}
//			else
//			{
//				props.hovered = false;
//				props.pressed = false;
//			}
//			ScriptComponent::update(entity, ts);
//		}
//
//		static void draw(Entity entity)
//		{
//			auto &is_visible = entity.get_component<Tag>().is_visible;
//
//			if (!is_visible)
//				return;
//
//			auto &props = entity.get_component<TextureButtonProps>();
//			const auto& transform = Transform::get_world_transform(entity);
//
//			Sprite sprite;
//
//			if (Engine::is_runtime())
//			{
//				sprite.mode = props.mode;
//			}
//			else
//			{
//				sprite.mode = RenderMode::World;
//			}
//
//			if (props.hovered)
//			{
//				sprite.texture_rect = props.hover_texture_rect;
//				sprite.size = props.hover_texture_rect.size;
//			}
//			else if (props.pressed)
//			{
//				sprite.texture_rect = props.active_texture_rect;
//				sprite.size = props.active_texture_rect.size;
//			}
//			else
//			{
//				sprite.texture_rect = props.normal_texture_rect;
//				sprite.size = props.normal_texture_rect.size;
//			}
//			Renderer2D::set_texture(props.texture);
//			//Renderer2D::draw_sprite(sprite, transform);
//		}
//
//		static bool is_hovered(Entity entity)
//		{
//			return entity.get_component<TextureButtonProps>().hovered;
//		}
//
//		static bool is_pressed(Entity entity)
//		{
//			return entity.get_component<TextureButtonProps>().pressed;
//		}
//
//};
//}
