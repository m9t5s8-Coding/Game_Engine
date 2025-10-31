#pragma once

#include <entt.hpp>
#include <Core/TimeStamp.hpp>
#include <string>
#include <cstdint>



namespace ag
{
	class Entity;
  class ScenePanel;
	class SaveScene;
  enum class NodeType;
	class Scene
	{
	public:
		Scene();
		~Scene();



		void on_update(TimeStamp ts);

		Entity create_entity(const std::string& name, NodeType type, bool is_cloning = false);
		Entity duplicate_entity(Entity original);

		void set_name(const std::string& name) { m_name = name; }
		const std::string get_name() const { return m_name; }

		void set_directory(const std::string& path) { m_directory = path; }
		const std::string get_directory() const { return m_directory; }

		bool has_name() { return m_name.empty(); }
		bool has_directory() { return m_directory.empty(); }



	private:
		entt::registry m_registry;
		uint32_t m_next_index = 0;

		std::string m_name = "";
		std::string m_directory = "";

		

		friend class Entity;
    friend class ScenePanel;
		friend class SaveScene;
	};
}
