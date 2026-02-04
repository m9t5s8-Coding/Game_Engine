#include <Scene/Scene.hpp>

#include <Apch.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/Entity.hpp>
#include <GameObjects/NodeFactory.hpp>
#include <GameObjects/Components/Components.hpp>
#include <Project/SaveScene.hpp>
#include <Project/Project.hpp>

namespace ag
{
	static constexpr float GRAVITY = 9.8 * 3.333f;

	Scene::Scene()
	{
		if (Engine::is_runtime())
		{
			m_world = AG_cscope<b2World>(b2Vec2(0.0f, GRAVITY));
			m_contact_listener = std::make_unique<GroundContactListener>();
			m_world->SetContactListener(m_contact_listener.get());
		}
	}

	Scene::~Scene()
	{
	}

	Entity Scene::create_entity(const std::string& name, const NodeType type, bool is_cloning)
	{
		Entity entity(m_registry.create());

		Tag_Component tag;
		tag.name = name;
		tag.index = m_next_index++;
		tag.node_type = type;

		entity.add_component<Tag_Component>(tag);

		if (!is_cloning)
		{
			auto it = NodeFactory::create_map.find(type);
			if (it != NodeFactory::create_map.end())
				it->second(entity);
		}

		return entity;
	}
	Entity Scene::duplicate_entity(Entity original, Entity parent)
	{
		auto& original_tag = original.get_component<Tag_Component>();
		Entity duplicate = create_entity(original_tag.name, original_tag.node_type, true);

		auto it = NodeFactory::clone_map.find(original_tag.node_type);
		if (it != NodeFactory::clone_map.end())
			it->second(original, duplicate);

		Tag_Component::clone_entity(original, duplicate, parent);



		return duplicate;
	}

	void Scene::destroy_entity(Entity entity)
	{
		m_to_delete_entity.push_back(entity);
	}


	void Scene::on_update(TimeStamp ts)
	{
		if (Engine::is_runtime())
		{
			m_world->Step(ts.get_seconds(), 8, 3);
		}

		m_registry.sort<Tag_Component>([](const Tag_Component& a, const Tag_Component& b)
			{ return a.index < b.index; });

		// Update Thread
		auto view = m_registry.view<Tag_Component>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto& parent = e.get_component<Tag_Component>().parent;
			if (parent.get_id() != INVALID_ENTITY)
				continue;

			update_entity_recursive(e, ts);
		}

		// Draw Thread
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto& parent = e.get_component<Tag_Component>().parent;
			if (parent.get_id() != INVALID_ENTITY)
				continue;
			draw_entity_recursive(e);
		}
		clear_destroyed_entity();

	}

	void Scene::destroy()
	{
		auto view = m_registry.view<Tag_Component>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto& tag = e.get_component<Tag_Component>();
			if(tag.parent.get_id() == INVALID_ENTITY || !tag.parent)
				destroy_entity(e);
		}
		clear_destroyed_entity();
	}

	void Scene::clear_destroyed_entity()
	{
		while (!m_to_delete_entity.empty())
		{
			Entity entity = m_to_delete_entity.back();
			m_to_delete_entity.pop_back();

			auto& tag = entity.get_component<Tag_Component>();
			tag.name;
			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				auto& parent_tag = tag.parent.get_component<Tag_Component>();
				parent_tag.children.erase(
					std::remove(parent_tag.children.begin(), parent_tag.children.end(), entity), parent_tag.children.end());
			}

			for (auto& children : tag.children)
			{
				auto& child_tag = children.get_component<Tag_Component>();
				child_tag.parent = Entity{};
				destroy_entity(children);
			}

			tag.children.clear();

			auto it = NodeFactory::clear_map.find(tag.node_type);
			if (it != NodeFactory::clear_map.end())
				it->second(entity);
		}
		m_to_delete_entity.clear();
	}

	void Scene::on_event(Event& event)
	{
		auto view = m_registry.view<Script_Component>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			Script_Component::event(entity, event);
		}
	}

	AG_ref<Scene> Scene::create(const std::string& name, const std::string& directory)
	{
		auto scene = AG_cref<Scene>();

		scene->set_name(name);
		scene->set_directory(directory);

		// auto camera_entity = scene->create_entity("Camera", NodeType::Camera);
		return scene;
	}

	void Scene::update_entity(Entity entity, TimeStamp ts)
	{
		auto it = NodeFactory::update_map.find(entity.get_component<Tag_Component>().node_type);
		if (it != NodeFactory::update_map.end())
			it->second(entity, ts);
	}
	void Scene::update_entity_recursive(Entity entity, TimeStamp ts)
	{
		update_entity(entity, ts);
		auto& tag = entity.get_component<Tag_Component>();
		for (auto& child : tag.children)
		{
			update_entity_recursive(child, ts);
		}
	}
	void Scene::draw_entity(Entity entity)
	{
		auto it = NodeFactory::draw_map.find(entity.get_component<Tag_Component>().node_type);
		if (it != NodeFactory::draw_map.end())
			it->second(entity);
	}
	void Scene::draw_entity_recursive(Entity entity)
	{
		draw_entity(entity);
		auto& tag = entity.get_component<Tag_Component>();
		for (auto& child : tag.children)
		{
			draw_entity_recursive(child);
		}
	}
}
