#include <Scene/Entity.hpp>

namespace ag
{
	Entity::Entity()
		:m_entity_handler(entt::null), m_scene(nullptr)
	{

	}

	Entity::Entity(const entt::entity handle)
		:m_entity_handler(handle)
	{
		m_scene = Scene::get_active_scene();
	}

	Entity::~Entity()
	{

	}

	
}