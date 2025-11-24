#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <Core/Application.hpp>

namespace ag
{
	struct TileMapNode
	{
		struct Tile
		{
			vec2u texture_id;

			static json save(Tile tile)
			{
				json j;

				Helper::save_json(j, "Texture ID", tile.texture_id);

				return j;
			}

			static void load(Tile& tile, const json& j)
			{
				Helper::load_json(j, "Texture ID", tile.texture_id);
			}
		};

		struct TileTexture
		{
			vec2u texture_pos;
			uint_rect texture_rect;

			static json save(TileTexture texture)
			{
				json j;

				Helper::save_json(j, "Position", texture.texture_pos);
				Helper::save_json(j, "Texture Rect", texture.texture_rect);

				return j;
			}

			static void load(TileTexture& texture, const json& j)
			{
				Helper::load_json(j, "Position", texture.texture_pos);
				Helper::load_json(j, "Texture Rect", texture.texture_rect);
			}
		};


		struct TileMapProp
		{
			vec2f size = { 32, 32 };
			vec2f offset;
			AG_ref<Texture2D> texture;
			std::string texture_path = "default.png";

			std::unordered_map<vec2u, TileTexture, vec2_hash<AG_uint>> tile_textures;
			std::unordered_map<vec2i, Tile, vec2_hash<int>> tiles;

			Sprite ghost_sprite;
			bool display_ghost = false;
			vec2i ghost_sprite_position;

			static json save(Entity entity)
			{
				json j;

				const auto& props = entity.get_component<TileMapProp>();

				Helper::save_json(j, "Texture Path", props.texture_path);

				Helper::save_json(j, "Size", props.size);
				Helper::save_json(j, "Offset", props.offset);

				for (const auto& [position, tile] : props.tiles)
				{
					std::string key = std::to_string(position.x) + "," + std::to_string(position.y);
					j["Tiles"][key] = Tile::save(tile);
				}

				return j;
			}

			static void load(Entity entity, const json& j)
			{
				auto& props = entity.get_component<TileMapProp>();

				Helper::load_json(j, "Texture Path", props.texture_path);
				{
					auto project = Project::get_active_project();
					std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + props.texture_path;
					props.texture = Texture2D::create(texture_path);
				}

				Helper::load_json(j, "Size", props.size);
				Helper::load_json(j, "Offset", props.offset);

				if (j.contains("Tiles"))
				{
					auto& tiles = props.tiles;
					tiles.clear();

					for (auto& [key, tile_json] : j["Tiles"].items())
					{
						Tile tile;
						Tile::load(tile, tile_json);

						int x, y;
						sscanf(key.c_str(), "%d,%d", &x, &y);
						tiles[{x, y}] = tile;
					}
				}
			}
		};

		static void create_node(Entity entity)
		{
			TileMapProp props;
			{
				auto project = Project::get_active_project();
				std::string texture_path = project->get_directory() + project->get_assets_directory() + "/" + props.texture_path;
				props.texture = Texture2D::create(texture_path);
			}
			entity.add_component<TileMapProp>(props);
		}
		
		static void delete_node(Entity entity)
		{

		}
		
		static void clone_node(Entity original, Entity clone)
		{
			auto& src = original.get_component<TileMapProp>();

			clone.add_component<TileMapProp>(src);
		}
		
		static json save_json(Entity entity)
		{
			json j;
			j["TileMapProp"] = TileMapProp::save(entity);
			return j;
		}
		
		static void load_json(Entity entity, const json& j)
		{
			TileMapProp::load(entity, j["TileMapProp"]);
		}
		
		static void show_properties(Entity entity)
		{
				auto& props = entity.get_component<TileMapProp>();
				{
					Tag::show_properties(entity);
					{
						UI::draw_string("Texture Path", props.texture_path);

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

								props.texture_path = relative_path;

								props.texture = Texture2D::create(full_path);
							}
						}
					}
				}
		}
		
		static void update(Entity enitity, TimeStamp ts)
		{

		}
		
		static void draw(Entity entity, TimeStamp ts)
		{
			auto is_visible = entity.get_component<Tag>().is_visible;
			if (!is_visible)
				return;


			auto& props = entity.get_component<TileMapProp>();
			Transform trans;
			Sprite sprite;
			Renderer2D::set_texture(props.texture);
			for (const auto& [position, tile] : props.tiles)
			{
				sprite.size = props.size;

				auto tex_it = props.tile_textures.find(tile.texture_id);
				if (tex_it != props.tile_textures.end())
				{
					const TileTexture& tile_tex = tex_it->second;
					sprite.texture_rect = tile_tex.texture_rect;
				}

				trans.position = (position * props.size) + props.size / 2 + props.offset;

        Renderer2D::draw_sprite(sprite, trans);
			}
			if (props.display_ghost)
			{
				trans.position = (props.ghost_sprite_position * props.size) + props.size / 2 + props.offset;
				Renderer2D::draw_sprite(props.ghost_sprite, trans);
			}
		}
	};
}
