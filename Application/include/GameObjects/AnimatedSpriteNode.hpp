#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	struct AnimatedSprite2DNode
	{
		struct Animation
		{
			std::string name;
			unsigned int start_frame = 0;
			unsigned int end_frame = 0;
			float duration = 0;
			bool loop = true;

			static json save(const Animation& anim)
			{
				json j;

				j["Animation Name"] = anim.name;
				j["Start Frame"] = anim.start_frame;
				j["End Frame"] = anim.end_frame;
				j["Duration"] = anim.duration;
				j["Loop"] = anim.loop;

				return j;
			}
		
			static void load(Animation& anim,const json& j)
			{
				anim.name = j["Animation Name"].get<std::string>();
				anim.start_frame = j["Start Frame"].get<int>();
				anim.end_frame = j["End Frame"].get<int>();
				anim.duration = j["Duration"].get<float>();
				anim.loop = j["Loop"].get<bool>();
			}
		};

		struct AnimatedSpriteProps
		{
			std::string texture_path = "default.png";
			AG_ref<Texture2D> texture;
			std::unordered_map<std::string, Animation> animations;
			vec2u frame_grid = { 1, 1 };
			uint_rect texture_rect;
			std::string current_animation = "";
			int current_frame = 0;
			bool playing = true;
			float timer = 0.0f;
			static json save(Entity entity)
			{
				const auto& props = entity.get_component<AnimatedSpriteProps>();

				json j;
				j["Texture Path"] = props.texture_path;
				j["Frame Grid"] = { props.frame_grid.x, props.frame_grid.y };
				j["Frame Grid"] = props.frame_grid.save();
				j["Texture Rect"] = props.texture_rect.save();
				j["Current Animation"] = props.current_animation;
				for (auto& [name, anim] : props.animations)
				{
					j["Animations"][name] = Animation::save(anim);
				}
				return j;
			}

			static void load(Entity entity, const json& j)
			{
				auto& props = entity.get_component<AnimatedSpriteProps>();
				props.texture_path = j["Texture Path"].get<std::string>();
				props.texture = Texture2D::create(default_path + props.texture_path);

				props.frame_grid.load(j["Frame Grid"]);
				props.texture_rect.load(j["Texture Rect"]);

				props.current_animation = j["Current Animation"].get<std::string>();

				if (j.contains("Animations"))
				{
					auto& anims = props.animations;
					anims.clear();

					for (auto& [name, animJson] : j["Animations"].items())
					{
						Animation anim;
						Animation::load(anim, animJson);
						anims[name] = anim;
					}
				}
			}

		};

		static void create_node(Entity entity)
		{
			entity.add_component<Transform>();

			AnimatedSpriteProps props;
			props.texture = Texture2D::create(default_path + props.texture_path);
			props.texture_rect = { 0, 0, props.texture->get_size() };

			entity.add_component<AnimatedSpriteProps>(props);
		}

		static void delete_node(Entity entity)
		{
			entity.delete_entity();
		}

		static void clone_node(Entity original, Entity clone)
		{
			clone.add_component<Transform>(original.get_component<Transform>());
			clone.add_component<AnimatedSpriteProps>(original.get_component<AnimatedSpriteProps>());
		}

		static json save(Entity entity)
		{
			json j;
			j["AnimationSprite2DProps"] = AnimatedSpriteProps::save(entity);
			j["Transform"] = Transform::save(entity);

			return j;
		}

		static void load(Entity entity,const json& j)
		{
			
			Transform::load(entity, j["Transform"]);
			AnimatedSpriteProps::load(entity, j["AnimationSprite2DProps"]);
		}

		static void show_properties(Entity entity)
		{
			auto& sprite = entity.get_component<AnimatedSpriteProps>();
			{
				Tag::show_properties(entity);
				{
					UI::draw_string("Texture Path", sprite.texture_path);
					if (ImGui::Button("Load Texture"))
					{
						sprite.texture = Texture2D::create(default_path + sprite.texture_path);
						{
							vec2u texture_size = sprite.texture->get_size();
							sprite.texture_rect.size = texture_size / sprite.frame_grid;
						}
					}
				}
				{
					UI::draw_title("Animations");
					if (UI::draw_vec2("Frame Grid", sprite.frame_grid, { 1, 1 }))
					{
						vec2u texture_size = sprite.texture->get_size();
						sprite.texture_rect.size = texture_size / sprite.frame_grid;
					}
					{
						if (ImGui::Button("Add Animation"))
						{
								Animation new_anim;
								new_anim.name = "Animation";
								sprite.animations[new_anim.name] = new_anim;
						}
					}

					for (auto it = sprite.animations.begin(); it != sprite.animations.end(); )
					{
						auto& [key, anim] = *it;

						ImGui::PushID(key.c_str());
						bool open = ImGui::TreeNode(key.c_str());

						ImGui::SameLine();
						if (ImGui::SmallButton("Delete"))
						{
							if (sprite.current_animation == key)
								sprite.current_animation = "";
							it = sprite.animations.erase(it);
							ImGui::PopID();
							continue;
						}

						if (open)
						{
							// Name editing — but since keys can't be changed directly,
							// we handle rename by adding a new key and removing the old one.

							UI::draw_string("Name", anim.name);
							ImGui::SameLine();
							if (ImGui::Button("Change") && !anim.name.empty())
							{
								sprite.animations[anim.name] = anim;
								if (sprite.current_animation == key)
								{
									sprite.current_animation = anim.name;
									
								}
								sprite.animations.erase(it++);
								ImGui::TreePop();
								ImGui::PopID();
								continue;
							}
							ImGui::Checkbox("Loop", &anim.loop);
							UI::draw_value("Start Frame", anim.start_frame);
							UI::draw_value("End Frame", anim.end_frame);
							UI::draw_value("Duration", anim.duration);

							ImGui::TreePop();
						}

						ImGui::PopID();
						++it;
					}


					if (ImGui::BeginCombo("Current Animation", sprite.current_animation.c_str()))
					{
						for (auto& [name, _] : sprite.animations)
						{
							bool selected = (sprite.current_animation == name);
							if (ImGui::Selectable(name.c_str(), selected))
							{
								sprite.current_animation = name;
								sprite.current_frame = 0;
								sprite.timer = 0.0f;
							}
							if (selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
				Transform::show_properties(entity);
			}
		}

		static void draw(Entity entity, TimeStamp ts)
		{
			auto& transform = entity.get_component<Transform>();
			auto& s = entity.get_component<AnimatedSpriteProps>();
			

			if (!s.current_animation.empty())
			{
				auto it = s.animations.find(s.current_animation);
				if (it == s.animations.end()) return;
				Animation& anim = it->second;

				float deltatime = ts.get_seconds();
				s.timer += deltatime;

				int frame_count = anim.end_frame - anim.start_frame + 1;

				if (frame_count <= 0) frame_count = 1;

				if (s.timer >= anim.duration && anim.duration != 0.0f)
				{
					s.timer -=  anim.duration;
					s.current_frame++;
					

					if (s.current_frame >= frame_count)
					{
						if (anim.loop)
						{
							s.current_frame = 0;
						}
						else
						{
							s.current_frame = frame_count - 1;
						}
					}
				}

				int frame_index = anim.start_frame + s.current_frame;
				s.texture_rect.position.x = (frame_index % s.frame_grid.x) * s.texture_rect.size.x;
				s.texture_rect.position.y = (frame_index / s.frame_grid.x) * s.texture_rect.size.y;
			}
			Renderer2D::set_texture(s.texture);
			Sprite sprite;
			sprite.texture_rect = s.texture_rect;
			sprite.size = s.texture_rect.size;
			Renderer2D::draw_sprite(sprite, transform);
		}
	};
}
