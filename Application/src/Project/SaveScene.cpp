#include <Project/SaveScene.hpp>
#include <Apch.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Scene/SceneComponent.hpp>

namespace fs = std::filesystem;

namespace ag
{
	void SaveScene::save_scene(const AG_ref<Scene>& scene, const std::string& path)
	{
		json j;
		j["Entities"] = json::array();

		auto group = scene->m_registry.group<Tag>();
		for (auto entityID : group)
		{
			Entity e{ entityID, scene.get() };
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
			j["Entities"].push_back(entityjson);
		}
		std::ofstream file(path);
		if (!file.is_open())
		{
			AERO_CORE_INFO("Failed to Open File for Saving: {}", path);
			return;
		}

		file << j.dump(4);
		file.close();
		AERO_CORE_INFO("Scene saved successfully to {}", path);
	}
	AG_ref<Scene> SaveScene::load_scene(const std::string& path)
	{
		AG_ref<Scene> scene = std::make_shared<Scene>();

		json j;
		std::ifstream file(path);

		if (!file.is_open())
		{
			AERO_CORE_ERROR("Failed to Open File!");
			return nullptr;
		}

		file >> j;
		file.close();

		for (auto& entityjson : j["Entities"])
		{
			NodeType type = static_cast<NodeType>(entityjson["NodeType"].get<int>());
			Entity e = scene->create_entity(entityjson["Tag"].get<std::string>(), type);

			auto it = NodeFactory::load_map.find(type);
			if (it != NodeFactory::load_map.end())
			{
				
				it->second(e, entityjson);
			}
		}

		AERO_CORE_INFO("Scene Loaded Successfully!");
		return scene;
	}
}

