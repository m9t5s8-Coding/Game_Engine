#include <Project/SaveScene.hpp>
#include <Apch.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Scene/SceneComponent.hpp>
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


		auto view = scene->m_registry.view<Tag>();
		if (!view.empty())
		{
			for (auto entityID : view)
			{
				if (!scene->m_registry.valid(entityID))
					continue;

				Entity e(entityID);

				if (!e.has_component<Tag>())
					continue;

				const auto& tag = e.get_component<Tag>();

				json entityjson;
				entityjson["NodeType"] = static_cast<int>(tag.node_type);
				entityjson["Tag"] = tag.tag;

				auto it = NodeFactory::save_map.find(tag.node_type);
				if (it != NodeFactory::save_map.end())
				{
					json nodeJson = it->second(e);
					entityjson.update(nodeJson);
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

		for (auto& entityjson : j["Scene"]["Entities"])
		{
			NodeType type;
			std::string tag;
			{
				int node;
				Helper::load_json(entityjson, "NodeType", node);
				Helper::load_json(entityjson, "Tag", tag);

				type = static_cast<NodeType>(node);
			}
			

			Entity e = scene->create_entity(tag, type);

			auto it = NodeFactory::load_map.find(type);
			if (it != NodeFactory::load_map.end())
			{
				it->second(e, entityjson);
			}
		}

		AERO_CORE_INFO("Scene Loaded Successfully: {0}", scene->get_name());
		return scene;
	}
}

