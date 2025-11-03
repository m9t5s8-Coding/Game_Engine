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
		}

		static json save(Entity entity)
		{
			json j;
			j["SpriteProp"] = SpriteProp::save(entity);
			j["Transform"] = Transform::save(entity);
			return j;
		}

		static void load(Entity entity, const json& j)
		{
			SpriteProp::load(entity, j["SpriteProp"]);
			Transform::load(entity, j["Transform"]);
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
						{
							auto project = Project::get_active_project();
							std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + sprite.texture_path;
							sprite.texture = Texture2D::create(texture_path);
						}
						sprite.size = sprite.texture->get_size();
						sprite.texture_rect = uint_rect(0, 0, sprite.size);
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

		static void draw(Entity entity, TimeStamp ts)
		{
			auto& transform = entity.get_component<Transform>();
			auto& s = entity.get_component<SpriteProp>();
			Sprite sprite;
			sprite.size = s.size;

			sprite.texture_rect = s.texture_rect;

			Renderer2D::set_texture(s.texture);
			Renderer2D::draw_sprite(sprite, transform);
		}
	};	
}
