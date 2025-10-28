#include <Scene/Entity.hpp>

namespace ag
{
	Entity::Entity()
		:m_entity_handler(entt::null), m_scene(nullptr)
	{

	}

	Entity::Entity(entt::entity handle, Scene* scene)
		:m_entity_handler(handle), m_scene(scene)
	{

	}

	Entity::~Entity()
	{

	}

	
}