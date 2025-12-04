#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	struct SpriteNode
	{
		struct SpriteProp
		{
			std::string texture_path = "default.png";
			AG_ref<Texture2D> texture;
			vec2u size;
			uint_rect texture_rect;

			bool flip_horizontal = false;
			bool flip_vertical = false;

			static json save(Entity entity)
			{
				json j;
				const auto& props = entity.get_component<SpriteProp>();

				j["Texture Path"] = props.texture_path;
				j["Size"] = props.size.save();
				j["Texture Rect"] = props.texture_rect.save();

				return j;
			}

			static void load(Entity entity,const json& j)
			{
				auto& props = entity.get_component<SpriteProp>();
				props.texture_path = j["Texture Path"].get<std::string>();
				{
					auto project = Project::get_active_project();
					std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" +  props.texture_path;
					props.texture = Texture2D::create(texture_path);
				}
				props.size.load(j["Size"]);
				props.texture_rect.load(j["Texture Rect"]);
			}

		};

		static void create_node(Entity entity)
		{
			entity.add_component<Transform>();
			SpriteProp sprite_;
			{
				auto project = Project::get_active_project();
				std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + sprite_.texture_path;
				sprite_.texture = Texture2D::create(texture_path);
			}
			sprite_.size = sprite_.texture->get_size();
			sprite_.texture_rect = uint_rect(0, 0, sprite_.size);
			entity.add_component<SpriteProp>(sprite_);

		}

		static void delete_node(Entity entity)
		{
			entity.delete_entity();
		}

		static void clone_node(Entity original, Entity clone)
		{
			clone.add_component<Transform>(original.get_component<Transform>());
			clone.add_component<SpriteProp>(original.get_component<SpriteProp>());

			if (original.has_component<ScriptComponent>())
			{
				clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
			}
		}

		static json save_json(Entity entity)
		{
			json j;
			j["SpriteProp"] = SpriteProp::save(entity);
			j["Transform"] = Transform::save(entity);

			if (entity.has_component<ScriptComponent>())
			{
				j["ScriptComponent"] = ScriptComponent::save_json(entity);
			}

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			SpriteProp::load(entity, j["SpriteProp"]);
			Transform::load(entity, j["Transform"]);

			if (j.contains("ScriptComponent"))
			{
				ScriptComponent::load_json(entity, j["ScriptComponent"]);
			}
		}

		static void show_properties(Entity entity)
		{
			auto& sprite = entity.get_component<SpriteProp>();
			{
				Tag::show_properties(entity);
				{
					UI::draw_string("Texture Path", sprite.texture_path);
					if (ImGui::Button("Load Texture"))
					{
						std::string full_path = FileDialogs::open_file("Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0");
						if (!full_path.empty())
						{
							auto project = Project::get_active_project();
							Helper::normalize_path(full_path);

							std::string project_dir = project->get_directory();
							std::string assets_dir = project->get_assets_directory();

							std::string base_path = project_dir + assets_dir + "/";

							std::string relative_path = full_path;
							if (relative_path.find(base_path) == 0)
								relative_path = relative_path.substr(base_path.size());

							Helper::normalize_path(relative_path);

							sprite.texture_path = relative_path;

							sprite.texture = Texture2D::create(full_path);

							vec2u texture_size = sprite.texture->get_size();
							sprite.size = texture_size;
							sprite.texture_rect = {0, 0, sprite.size};
						}
					}
				}
				{
					vec2f texture_size = sprite.texture->get_size();
					UI::draw_vec2("Size", sprite.size, sprite.texture_rect.size);
					UI::draw_vec2("Texture Position", sprite.texture_rect.position, vec2f(0, 0));
					UI::draw_vec2("Texture Size", sprite.texture_rect.size, texture_size);
				}
				Transform::show_properties(entity);


			}
		}

		static void update(Entity entity, TimeStamp ts)
		{
			ScriptComponent::update(entity, ts);
		}

		static void draw(Entity entity)
		{
			auto is_visible = entity.get_component<Tag>().is_visible;
			if (!is_visible)
				return;



			auto& transform = entity.get_component<Transform>();
			auto& s = entity.get_component<SpriteProp>();
			Sprite sprite;
			sprite.size = s.size;

			sprite.texture_rect = s.texture_rect;
			sprite.flip_horizontal = s.flip_horizontal;
			sprite.flip_vertical = s.flip_vertical;

			Renderer2D::set_texture(s.texture);
			Renderer2D::draw_sprite(sprite, transform);
		}
	};
}
