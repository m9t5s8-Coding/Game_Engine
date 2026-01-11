#include <Project/SaveScene.hpp>
#include <Apch.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/Components/Components.hpp>
#include <Helper.hpp>

namespace fs = std::filesystem;

namespace ag
{
	void SaveScene::save_scene(AG_ref<Scene>& scene, const std::string& path)
	{
		json j;
		Helper::save_json(j["Scene"], "Name", scene->get_name());
		Helper::save_json(j["Scene"], "Path", scene->get_directory());

		Scene::set_active_scene(scene);

		j["Scene"]["Entities"] = json::array();


		auto view = scene->m_registry.view<Tag_Component>();
		if (!view.empty())
		{
			for (auto entityID : view)
			{
				if (!scene->m_registry.valid(entityID))
					continue;

				Entity e(entityID);

				if (!e.has_component<Tag_Component>())
					continue;

				const auto& tag = e.get_component<Tag_Component>();

				json entityjson = Tag_Component::save_json(e);
				if (!entityjson.is_object())
				{
					entityjson = json::object();
				}

				auto it = NodeFactory::save_map.find(tag.node_type);
				if (it != NodeFactory::save_map.end())
				{
					json nodejson = it->second(e);

					if (!nodejson.is_object())
					{
						nodejson = json::object();
					}

					entityjson.update(nodejson);
				}
				j["Scene"]["Entities"].push_back(entityjson);
			}
		}

		Helper::makefile_read_only(path, false);
		std::ofstream file(path, std::ios::trunc);
		if (!file.is_open())
		{
			AERO_CORE_INFO("Failed to Open File for Saving: {}", path);
			return;
		}

		file << j.dump(4);
		file.close();
		AERO_CORE_INFO("Scene saved successfully to {}", path);
		scene->set_save_required(false);
		Helper::makefile_read_only(path);
	}


	AG_ref<Scene> SaveScene::load_scene(const std::string& path)
	{
		AG_ref<Scene> scene = Scene::create();

		json j;
		Helper::makefile_read_only(path, false);
		std::ifstream file(path);

		if (!file.is_open())
		{
			AERO_CORE_ERROR("Failed to Open File!");
			return Scene::create("default");
		}

		file >> j;
		file.close();
		Helper::makefile_read_only(path);

		std::string scene_name, scene_path;
		Helper::load_json(j["Scene"], "Name", scene_name);
		Helper::load_json(j["Scene"], "Path", scene_path);
		scene->set_name(scene_name);
		scene->set_directory(scene_path);
		Scene::set_active_scene(scene);
		std::unordered_map<AG_uint, Entity> id_map;

		for (auto& entityjson : j["Scene"]["Entities"])
		{
			NodeType type;
			std::string tag;
			{
				int node;
				Helper::load_json(entityjson, "NodeType", node);
				Helper::load_json(entityjson, "Tag_Component", tag);

				type = static_cast<NodeType>(node);
			}
			Entity e = scene->create_entity(tag, type);

			Tag_Component::load_json(e, entityjson);
			{
				auto& tag = e.get_component<Tag_Component>();
				scene->set_next_index(std::max(scene->get_index(), tag.index + 1));
				id_map[tag.index] = e;
			}	
		}

		{
			auto view = scene->m_registry.view<Tag_Component>();
			for (auto entityID : view)
			{
				Entity e(entityID);
				Tag_Component::load_children(e);
			}
		}

		for (auto& entityjson : j["Scene"]["Entities"])
		{
			AG_uint id;
			Helper::load_json(entityjson, "ID", id);
			Entity e = id_map[id];
			int node;
			Helper::load_json(entityjson, "NodeType", node);
			NodeType type = static_cast<NodeType>(node);

			auto it = NodeFactory::load_map.find(type);
			if (it != NodeFactory::load_map.end())
			{
				it->second(e, entityjson);
			}
		}

		if (Engine::is_runtime())
		{
			auto view = scene->m_registry.view<Script_Component>();
			for (auto entityID : view)
			{
				Entity e(entityID);
				Script_Component::load_scripts(e);
			}
		}
		id_map.clear();
		index_map.clear();
		AERO_CORE_INFO("Scene Loaded Successfully: {0}", scene->get_name());
		return scene;
	}
}

