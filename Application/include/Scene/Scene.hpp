#pragma once

#include <entt.hpp>
#include <Core/TimeStamp.hpp>
#include <string>
#include <cstdint>
#include <Events/Event.hpp>
#include "box2d/box2d.h"





namespace ag
{
	class Entity;
	class ScenePanel;
	class SaveScene;
	class EditorLayer;
	enum class NodeType;

	class Scene
	{
	public:
		Scene();
		~Scene();



		void on_update(TimeStamp ts);
		void destroy();
		void on_event(Event& event);

		Entity create_entity(const std::string& name, NodeType type, bool is_cloning = false);
		Entity duplicate_entity(Entity original, Entity parent);

		void set_name(const std::string& name) { m_name = name; }
		const std::string& get_name() const { return m_name; }

		void set_directory(const std::string& path) { m_directory = path; }
		const std::string& get_directory() const { return m_directory; }

		bool has_name() { return !m_name.empty(); }
		bool has_directory() { return !m_directory.empty(); }

		bool is_save_required() const { return m_save_required; }
		void set_save_required(bool required = true) { m_save_required = required; }

		AG_uint get_index() const { return m_next_index; }
		void set_next_index(AG_uint index) { m_next_index = index; }

		inline b2World& get_world() { return *m_world; }

		template <typename T>
		auto get_view()
		{
			return m_registry.view<T>();
		}

		void destroy_entity(Entity entity);


		static AG_ref<Scene> get_active_scene() { return s_active_scene; }
		static void set_active_scene(const AG_ref<Scene>& scene) { s_active_scene = scene; }

		static AG_ref<Scene> create(const std::string& name = "", const std::string& directory = "");

	private:
		void update_entity_recursive(Entity entity, TimeStamp ts);
		void update_entity(Entity entity, TimeStamp ts);

		void draw_entity_recursive(Entity entity);
		void draw_entity(Entity entity);



	private:
		entt::registry m_registry;
		AG_uint m_next_index = 0;
		AG_scope<b2World> m_world;
		std::string m_name = "";
		std::string m_directory = "";
		std::vector<Entity> m_to_delete_entity;
		bool m_save_required = false;

		inline static AG_ref<Scene> s_active_scene;




		friend class Entity;
		friend class ScenePanel;
		friend class SaveScene;
		friend class EditorLayer;
	};
}
