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
		if (Engine::is_runtime())
		{
			m_world = AG_cscope<b2World>(b2Vec2(0.0f, 28.8f));
		}
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
	Entity Scene::duplicate_entity(Entity original, Entity parent)
	{
		auto& original_tag = original.get_component<Tag>();
		Entity duplicate = create_entity(original_tag.tag, original_tag.node_type, true);

		auto it = NodeFactory::clone_map.find(original_tag.node_type);
		if (it != NodeFactory::clone_map.end())
			it->second(original, duplicate);

		Tag::clone(original, duplicate, parent);



		return duplicate;
	}

	void Scene::destroy_entity(Entity entity)
	{
		m_to_delete_entity.push_back(entity);
	}

	//Needed an Parent Child Heirarchy Draw Order
	void Scene::on_update(TimeStamp ts)
	{
		if (Engine::is_runtime())
		{
			m_world->Step(ts.get_seconds(), 8, 3);
		}

		m_registry.sort<Tag>([](const Tag& a, const Tag& b)
			{ return a.index < b.index; });

		// Update Thread
		auto view = m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto& parent = e.get_component<Tag>().parent;
			if (parent.get_id() != INVALID_ENTITY)
				continue;

			update_entity_recursive(e, ts);
		}

		// Draw Thread
		for (auto entityID : view)
		{
			Entity e(entityID);
			auto& parent = e.get_component<Tag>().parent;
			if (parent.get_id() != INVALID_ENTITY)
				continue;
			draw_entity_recursive(e);
		}

		{
			while (!m_to_delete_entity.empty())
			{
				Entity entity = m_to_delete_entity.back();
				m_to_delete_entity.pop_back();

				auto& tag = entity.get_component<Tag>();

				// Remove From The Parent Entity
				if (tag.parent.get_id() != INVALID_ENTITY)
				{
					auto& parent_tag = tag.parent.get_component<Tag>();
					parent_tag.children.erase(
						std::remove(parent_tag.children.begin(), parent_tag.children.end(), entity), parent_tag.children.end());
				}

				for (auto& children : tag.children)
				{
					auto& child_tag = children.get_component<Tag>();
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
	}

	void Scene::destroy()
	{
		if (Engine::is_runtime())
		{

		}
		// auto view = m_registry.view<Tag>();
		// for (auto entityID : view)
		//{
		//	Entity e(entityID);
		//	destroy_entity(e);
		//
		// }
		//{
		//	while (!m_to_delete_entity.empty())
		//	{
		//		Entity entity = m_to_delete_entity.back();
		//		m_to_delete_entity.pop_back();

		//		auto& tag = entity.get_component<Tag>();

		//		//Remove From The Parent Entity
		//		if (tag.parent.get_id() != INVALID_ENTITY)
		//		{
		//			auto& parent_tag = tag.parent.get_component<Tag>();
		//			parent_tag.children.erase(
		//				std::remove(parent_tag.children.begin(), parent_tag.children.end(), entity), parent_tag.children.end()
		//			);
		//		}

		//		for (auto& children : tag.children)
		//		{
		//			auto& child_tag = children.get_component<Tag>();
		//			child_tag.parent = Entity{};
		//			destroy_entity(children);
		//		}

		//		tag.children.clear();

		//		auto it = NodeFactory::clear_map.find(tag.node_type);
		//		if (it != NodeFactory::clear_map.end())
		//			it->second(entity);
		//	}
		//	m_to_delete_entity.clear();
		//}
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

		// auto camera_entity = scene->create_entity("Camera", NodeType::Camera);
		return scene;
	}

	void Scene::update_entity(Entity entity, TimeStamp ts)
	{
		auto it = NodeFactory::update_map.find(entity.get_component<Tag>().node_type);
		if (it != NodeFactory::update_map.end())
			it->second(entity, ts);
	}
	void Scene::update_entity_recursive(Entity entity, TimeStamp ts)
	{
		update_entity(entity, ts);
		auto& tag = entity.get_component<Tag>();
		for (auto& child : tag.children)
		{
			update_entity_recursive(child, ts);
		}
	}
	void Scene::draw_entity(Entity entity)
	{
			auto it = NodeFactory::draw_map.find(entity.get_component<Tag>().node_type);
			if (it != NodeFactory::draw_map.end())
				it->second(entity);
	}
	void Scene::draw_entity_recursive(Entity entity)
	{
		draw_entity(entity);
		auto& tag = entity.get_component<Tag>();
		for (auto& child : tag.children)
		{
			draw_entity_recursive(child);
		}
	}
}
