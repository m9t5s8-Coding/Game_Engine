#pragma once

#include <entt.hpp>
#include <Core/TimeStamp.hpp>
#include <string>
#include <cstdint>



namespace ag
{
	class Entity;
  class ScenePanel;
  enum class NodeType;
	class Scene
	{
	public:
		Scene();
		~Scene();

		void on_update(TimeStamp ts);

		Entity create_entity(const std::string& name, NodeType type);



	private:
		entt::registry m_registry; // container
		uint32_t m_next_index = 0;

		friend class Entity;
    friend class ScenePanel;
	};
}
