#include <Scene/Scene.hpp>

#include <Apch.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Scene/SceneComponent.hpp>
#include <Project/SaveScene.hpp>
#include <Project/Project.hpp>

namespace ag
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	Entity Scene::create_entity(const std::string& name, const NodeType type, bool is_cloning)
	{
		Entity entity(m_registry.create());
		entity.add_component<Tag>(name, m_next_index++, RenderLayer::MidGround, type);

		if (!is_cloning)
		{
			auto it = NodeFactory::create_map.find(type);
			if (it != NodeFactory::create_map.end())
				it->second(entity);
		}

		return entity;
	}
	Entity Scene::duplicate_entity(Entity original)
	{
		auto& original_tag = original.get_component<Tag>();
		Entity duplicate = create_entity(original_tag.tag, original_tag.node_type, true);

		auto it = NodeFactory::clone_map.find(original_tag.node_type);
		if (it != NodeFactory::clone_map.end())
			it->second(original, duplicate);

		return duplicate;
	}

	void Scene::on_update(TimeStamp ts)
	{
		m_registry.sort<Tag>([](const Tag& a, const Tag& b) {
			return a.index < b.index;
		});

		auto view = m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto it = NodeFactory::draw_map.find(e.get_component<Tag>().node_type);
			if (it != NodeFactory::draw_map.end())
				it->second(e, ts);
		}
	}

	AG_ref<Scene> Scene::create(const std::string& name, const std::string& directory)
	{
		auto scene = AG_cref<Scene>();

		scene->set_name(name);
		scene->set_directory(directory);


		return scene;
	}
}
