#pragma once

#include <entt.hpp>
#include <Scene/Scene.hpp>
#include <cstdint>

namespace ag
{
	class Entity
	{
	public:
		Entity(entt::entity handle);
		Entity(const Entity& other) = default;
		Entity();

		~Entity();

		template <typename T, typename... Args>
		T& add_component(Args&&... args)
		{
			AERO_ASSERT(!has_component<T>(), "Entity already has component!");
			return m_scene->m_registry.emplace<T>(m_entity_handler, std::forward<Args>(args)...);
		}

		template <typename T>
		T& get_component()
		{
			return m_scene->m_registry.get<T>(m_entity_handler);
		}

		template <typename T>
		bool has_component()
		{
			return m_scene->m_registry.all_of<T>(m_entity_handler);
		}

		template <typename T>
		void remove_component()
		{
			m_scene->m_registry.remove<T>(m_entity_handler);
		}

		// TODO
		void delete_entity() { m_scene->m_registry.destroy(m_entity_handler); }
		//void delete_entity() { m_entity_handler = entt::null; }



		uint32_t get_id() const { return static_cast<uint32_t>(m_entity_handler); }

		operator bool() const { return m_entity_handler != entt::null; }
		operator uint32_t() const { return static_cast<uint32_t>(m_entity_handler); }

		bool operator==(const Entity& other) const { return m_entity_handler == other.m_entity_handler && m_scene == other.m_scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
	private:
		entt::entity m_entity_handler;
		AG_ref<Scene> m_scene;
	};
}
