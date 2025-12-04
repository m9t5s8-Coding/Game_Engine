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

		Tag tag;
		tag.tag = name;
		tag.index = m_next_index++;
		tag.node_type = type;

		entity.add_component<Tag>(tag);

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

		Tag::clone(original, duplicate);

		auto it = NodeFactory::clone_map.find(original_tag.node_type);
		if (it != NodeFactory::clone_map.end())
			it->second(original, duplicate);

		return duplicate;
	}

	void Scene::destroy_entity(Entity entity)
	{
		m_to_delete_entity.push_back(entity);
	}

	void Scene::on_update(TimeStamp ts)
	{
		m_registry.sort<Tag>([](const Tag& a, const Tag& b) {
			return a.index < b.index;
		});


		// Update Thread
		auto view = m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto it = NodeFactory::update_map.find(e.get_component<Tag>().node_type);
			if (it != NodeFactory::update_map.end())
				it->second(e, ts);

		}

		// Draw Thread
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto it = NodeFactory::draw_map.find(e.get_component<Tag>().node_type);
			if (it != NodeFactory::draw_map.end())
				it->second(e);

		}

		{
			for (auto& entity : m_to_delete_entity)
			{
				auto& tag = entity.get_component<Tag>().tag;
				entity.delete_entity();
			}
			m_to_delete_entity.clear();
		}
	}

	void Scene::destroy()
	{
		auto view = m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto it = NodeFactory::clear_map.find(e.get_component<Tag>().node_type);
			if (it != NodeFactory::clear_map.end())
				it->second(e);
		}
	}

	void Scene::on_event(Event& event)
	{
		auto view = m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			if (entity.has_component<ScriptComponent>())
			{
				ScriptComponent::event(entity, event);
			}
		}
	}

	AG_ref<Scene> Scene::create(const std::string& name, const std::string& directory)
	{
		auto scene = AG_cref<Scene>();

		scene->set_name(name);
		scene->set_directory(directory);

		//auto camera_entity = scene->create_entity("Camera", NodeType::Camera);
		return scene;
	}
}
