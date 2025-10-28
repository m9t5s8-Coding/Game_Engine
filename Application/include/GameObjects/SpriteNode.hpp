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

			Sprite sprite;
		};

		static void create_node(Entity entity)
		{
			entity.add_component<Transform>();
			SpriteProp sprite_;
			sprite_.texture = Texture2D::create(default_path + sprite_.texture_path);
			sprite_.size = sprite_.texture->get_size();
			sprite_.texture_rect = uint_rect(0, 0, sprite_.size);
			entity.add_component<SpriteProp>(sprite_);
		}

		static void delete_node(Entity entity)
		{
			entity.delete_entity();
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
						sprite.texture = Texture2D::create(default_path + sprite.texture_path);
						sprite.size = sprite.texture->get_size();
						sprite.texture_rect = uint_rect(0, 0, sprite.size);
						sprite.sprite.size = sprite.size;
						sprite.sprite.texture_rect = sprite.texture_rect;
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

			s.sprite.size = s.size;

			s.sprite.texture_rect = s.texture_rect;

			Renderer2D::set_texture(s.texture);
			Renderer2D::draw_sprite(s.sprite, transform);
		}
	};	
}
