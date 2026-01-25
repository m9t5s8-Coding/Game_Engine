#include<GameObjects/GameObjects.hpp>
#include <GameObjects/Components/Components.hpp>

namespace ag::NodeHelper
{
	NodeType get_nodetype(Entity entity)
	{
		if (entity.has_component<Tag_Component>())
		{
			return entity.get_component<Tag_Component>().node_type;
		}
		return NodeType::None;
	}
	
	AG_ref<Texture2D> create_texture(const std::string& path)
	{
		auto project = Project::get_active_project();
		auto& project_path = project->get_directory();
		auto& assets_path = project->get_assets_directory();
		std::string new_path = project_path + assets_path + path;
		auto texture = Texture2D::create(new_path);
		return texture;
	}
	
	AG_ref<Texture2D> load_texture(std::string& path)
	{
		auto project = Project::get_active_project();

		Helper::normalize_path(path);

		std::string project_dir = project->get_directory();
		std::string assets_dir = project->get_assets_directory();

		std::string base_path = project_dir + assets_dir;

		if (path.find(base_path) == 0)
			path = path.substr(base_path.size());

		Helper::normalize_path(path);

		return create_texture(path);
	}
	
	bool has_capability(Node_Capability a, Node_Capability b)
	{
		return ((AG_uint)a & (AG_uint)b) != 0;
	}
	
	Node_Capability get_node_capabilities(NodeType type)
	{
		switch (type)
		{
		case ag::NodeType::None:
			return Node_Capability::None;

		case ag::NodeType::Rectangle:
			return Node_Capability::RectShape |
				Node_Capability::Render2D |
				Node_Capability::Physics2D |
				Node_Capability::Transform |
				Node_Capability::UI;

		case ag::NodeType::Circle:
			return Node_Capability::CircleShape |
				Node_Capability::Render2D |
				Node_Capability::Physics2D |
				Node_Capability::Transform |
				Node_Capability::UI;

		case ag::NodeType::Sprite:
			return Node_Capability::Render2D |
				Node_Capability::Physics2D |
				Node_Capability::Texture2D |
				Node_Capability::Transform |
				Node_Capability::UI;

		case ag::NodeType::AnimatedSprite2D:
			return Node_Capability::Render2D |
				Node_Capability::Physics2D |
				Node_Capability::Texture2D |
				Node_Capability::Transform |
				Node_Capability::UI;

		case ag::NodeType::Camera:
			return Node_Capability::Camera;
		case ag::NodeType::TileMap:
			return Node_Capability::TileMap;

		case ag::NodeType::Scene2D:
		
		case ag::NodeType::Button:
		case ag::NodeType::TextureButton:
			return Node_Capability::None;

		case ag::NodeType::Text:
			return Node_Capability::Text |
				Node_Capability::Transform |
				Node_Capability::UI;

		case ag::NodeType::CollisionShape:
			return Node_Capability::Physics2D;
		default:
			break;
		}
	}
}