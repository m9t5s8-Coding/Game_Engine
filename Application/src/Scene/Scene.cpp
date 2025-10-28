#include <Scene/Scene.hpp>

#include <Apch.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	Entity Scene::create_entity(const std::string& name, const NodeType type)
	{
		Entity entity = { m_registry.create(), this };
		entity.add_component<Tag>(name, m_next_index++, RenderLayer::MidGround, type);

		auto it = NodeFactory::create_map.find(type);
		if (it != NodeFactory::create_map.end())
			it->second(entity);

		return entity;
	}

	void Scene::on_update(TimeStamp ts)
	{
		m_registry.sort<Tag>([](const Tag& a, const Tag& b) {
			return a.index < b.index;
			});

		auto group = m_registry.group<Tag>();
		for (auto entityID : group)
		{
			Entity e{ entityID, this };

			auto it = NodeFactory::draw_map.find(e.get_component<Tag>().node_type);
			if (it != NodeFactory::draw_map.end())
				it->second(e, ts);
		}
	}


}
