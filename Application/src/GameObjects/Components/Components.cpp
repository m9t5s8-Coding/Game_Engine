#include<GameObjects/Components/Components.hpp>

#include <GameObjects/GameObjects.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{

	void Tag_Component::add_component(Entity entity)
	{

	}
	void Tag_Component::remove_component(Entity entity)
	{

	}
	json Tag_Component::save_json(Entity entity)
	{
		json j;
		auto& tag = entity.get_component<Tag_Component>();

		Helper::save_json(j, "Name", tag.name);
		Helper::save_json(j, "NodeType", static_cast<int>(tag.node_type));
		Helper::save_json(j, "Visible", tag.visible);
		Helper::load_json(j, "Locked", tag.locked);
		Helper::save_json(j, "ID", tag.index);

		if (tag.parent.get_id() != INVALID_ENTITY)
		{
			auto parent_index = tag.parent.get_component<Tag_Component>().index;
			Helper::save_json(j, "Parent", parent_index);
		}

		if (!tag.children.empty())
		{
			json children_json = json::array();
			for (auto& child : tag.children)
			{
				auto child_index = child.get_component<Tag_Component>().index;
				children_json.push_back(child_index);
			}
			j["Children"] = children_json;
		}
		return j;
	}
	void Tag_Component::load_json(Entity entity, const json& j)
	{
		auto& tag = entity.get_component<Tag_Component>();
		Helper::load_json(j, "Name", tag.name);
		Helper::load_json(j, "NodeType", tag.node_type);
		Helper::load_json(j, "ID", tag.index);
		Helper::load_json(j, "Visible", tag.visible);
		Helper::load_json(j, "Parent", tag.parent_id);

		if (j.contains("Children") && j["Children"].is_array())
		{
			auto& child_array = j["Children"];
			tag.children_id.clear();
			for (auto& id_json : child_array)
			{
				AG_uint child_id = id_json.get<AG_uint>();
				tag.children_id.push_back(child_id);
			}
		}

		index_map[tag.index] = entity;
	}
	void Tag_Component::load_children(Entity entity)
	{
		auto& tag = entity.get_component<Tag_Component>();
		if (tag.parent_id != INVALID_ENTITY)
		{
			if (index_map.contains(tag.parent_id))
			{
				tag.parent = index_map[tag.parent_id];
			}
		}

		for (auto& child_id : tag.children_id)
		{
			if (!index_map.contains(child_id))
				continue;

			Entity child = index_map[child_id];
			tag.children.push_back(child);
		}
	}
	void Tag_Component::clone_entity(Entity original, Entity duplicate, Entity parent)
	{
		const auto& original_tag = original.get_component<Tag_Component>();
		auto& duplicate_tag = duplicate.get_component<Tag_Component>();
		duplicate_tag.visible = original_tag.visible;
		if (parent.get_id() != INVALID_ENTITY)
		{
			duplicate_tag.parent = parent;
			auto& parent_tag = parent.get_component<Tag_Component>();
			parent_tag.children.push_back(duplicate);
		}
		duplicate_tag.children.clear();
		auto scene = Scene::get_active_scene();
		for (auto& children : original_tag.children)
		{
			scene->duplicate_entity(children, duplicate);
		}
	}
	bool Tag_Component::is_compatible(NodeType type)
	{
		return true;
	}
	const char* Tag_Component::get_name()
	{
		return "Tag";
	}


	void Transform_Component::add_component(Entity entity)
	{
		entity.add_component<Transform_Component>();
	}
	void Transform_Component::remove_component(Entity entity)
	{
		entity.remove_component<Transform_Component>();
	}
	json Transform_Component::save_json(Entity entity)
	{
		json j;
		const auto& props = entity.get_component<Transform_Component>();
		Helper::save_json(j, "Position", props.position);
		Helper::save_json(j, "Scale", props.scale);
		Helper::save_json(j, "Rotation", props.rotation);

		return j;
	}
	void Transform_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Transform_Component>())
			entity.add_component<Transform_Component>();

		auto& props = entity.get_component<Transform_Component>();
		Helper::load_json(j, "Position", props.position);
		Helper::load_json(j, "Scale", props.scale);
		Helper::load_json(j, "Rotation", props.rotation);
	}
	void Transform_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Transform_Component>())
		{
			clone.add_component<Transform_Component>(original.get_component<Transform_Component>());
		}
	}
	bool Transform_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Render2D);
	}
	const char* Transform_Component::get_name()
	{
		return "Transform";
	}
	Transform_Component Transform_Component::get_world_transform(Entity entity)
	{
		Transform_Component world_transform;
		if (!entity.has_component<Transform_Component>())
			return world_transform;

		world_transform = entity.get_component<Transform_Component>();
		auto& tag = entity.get_component<Tag_Component>();

		if (tag.parent.get_id() != INVALID_ENTITY)
		{
			Transform_Component parent_world = get_world_transform(tag.parent);

			world_transform.position = parent_world.position + world_transform.position;
			world_transform.scale = parent_world.scale * world_transform.scale;
			world_transform.rotation = parent_world.rotation + world_transform.rotation;
		}
		return world_transform;
	}
	void Transform_Component::get_local_transform(Entity entity, const Transform_Component& world_transform)
	{
		auto& transform = entity.get_component<Transform_Component>();
		auto& tag = entity.get_component<Tag_Component>();

		if (tag.parent.get_id() != INVALID_ENTITY)
		{
			Transform_Component parent_world = get_world_transform(tag.parent);
			transform.position = world_transform.position - parent_world.position;
			transform.scale = world_transform.scale / parent_world.scale;
			transform.rotation = world_transform.rotation - parent_world.rotation;
		}
	}



	void Script_Component::add_component(Entity entity)
	{
		entity.add_component<Script_Component>();
	}
	void Script_Component::remove_component(Entity entity)
	{
		auto& comp = entity.get_component<Script_Component>();
		comp.env.get().clear();
		comp.on_create = LuaFunc();
		comp.on_update = LuaFunc();
		comp.on_destroy = LuaFunc();
		comp.on_event = LuaFunc();

		entity.remove_component<Script_Component>();
	}
	json Script_Component::save_json(Entity entity)
	{
		json j;

		auto& comp = entity.get_component<Script_Component>();
		Helper::save_json(j, "Path", comp.path);

		return j;
	}
	void Script_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Script_Component>())
			entity.add_component<Script_Component>();

		auto& props = entity.get_component<Script_Component>();
		Helper::load_json(j, "Path", props.path);
	}
	void Script_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Script_Component>())
		{
			const auto& original_path = original.get_component<Script_Component>().path;
			Script_Component comps;
			comps.path = original_path;
			clone.add_component<Script_Component>(comps);
			load_scripts(clone);
		}
	}
	void Script_Component::create(Entity entity)
	{
		if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
			return;
		auto& comp = entity.get_component<Script_Component>();
		if (comp.on_create.is_valid())
		{
			comp.on_create.call();
		}
	}
	void Script_Component::update(Entity entity, TimeStamp ts)
	{
		if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
			return;

		auto& comp = entity.get_component<Script_Component>();

		if (comp.on_update.is_valid())
		{
			comp.on_update.call(ts.get_seconds());
		}
	}
	void Script_Component::destroy(Entity entity)
	{
		if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
			return;

		auto& comp = entity.get_component<Script_Component>();

		if (comp.on_destroy.is_valid())
		{
			comp.on_destroy.call();
		}

		comp.env.get().clear();
		comp.on_create = LuaFunc();
		comp.on_update = LuaFunc();
		comp.on_destroy = LuaFunc();
		comp.on_event = LuaFunc();
	}
	bool Script_Component::event(Entity entity, Event& e)
	{
		if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
			return false;

		auto& comp = entity.get_component<Script_Component>();
		if (!comp.on_event.is_valid())
			return false;

		comp.on_event.call(e);

		return false;
	}
	void Script_Component::load_scripts(Entity entity)
	{
		if (!Engine::is_runtime() || !entity.has_component<Script_Component>())
			return;

		auto& comp = entity.get_component<Script_Component>();

		auto project = Project::get_active_project();
		std::string full_path = project->get_directory() + project->get_scripts_directory() + comp.path;

		comp.env.get().set_function("get_entity", [entity]() -> Entity
			{ return entity; });

		comp.env.get().set_function("get_children", [](ag::Entity& e, const std::string& name) -> Entity
			{

				if (!e.has_component<Tag_Component>())
					return {};

				auto& tag = e.get_component<Tag_Component>();
				for (auto& child : tag.children)
				{
					auto& child_tag = child.get_component<Tag_Component>();
					if (child_tag.name == name)
					{
						return child;
					}
				}
				return {}; });

		ScriptManager::load_script(full_path, comp.env);
		comp.on_create.set_function(comp.env, "on_create");
		comp.on_update.set_function(comp.env, "on_update");
		comp.on_destroy.set_function(comp.env, "on_destroy");
		comp.on_event.set_function(comp.env, "on_event");
		create(entity);
	}
	bool Script_Component::is_compatible(NodeType type)
	{
		return true;
	}
	const char* Script_Component::get_name()
	{
		return "Script";
	}





	void Render2D_Component::add_component(Entity entity)
	{
		entity.add_component<Render2D_Component>();
	}
	void Render2D_Component::remove_component(Entity entity)
	{
		entity.remove_component<Render2D_Component>();
	}
	json Render2D_Component::save_json(Entity entity)
	{
		json j;
		auto& props = entity.get_component<Render2D_Component>();
		Helper::save_json(j, "Size", props.size);
		Helper::save_json(j, "Color", props.color);

		return j;
	}
	void Render2D_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Render2D_Component>())
			entity.add_component<Render2D_Component>();

		auto& props = entity.get_component<Render2D_Component>();
		Helper::load_json(j, "Size", props.size);
		Helper::load_json(j, "Color", props.color);
	}
	void Render2D_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Render2D_Component>())
		{
			clone.add_component<Render2D_Component>(original.get_component<Render2D_Component>());
		}
	}
	bool Render2D_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Render2D);
	}
	const char* Render2D_Component::get_name()
	{
		return "Render2D";
	}


	void Border_Component::add_component(Entity entity)
	{
		entity.add_component<Border_Component>();
	}
	void Border_Component::remove_component(Entity entity)
	{
		entity.remove_component<Border_Component>();
	}
	json Border_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<Border_Component>();
		Helper::save_json(j, "Thickness", props.thickness);
		Helper::save_json(j, "Color", props.color);

		return j;
	}
	void Border_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Border_Component>())
			entity.add_component<Border_Component>();

		auto& props = entity.get_component<Border_Component>();
		Helper::load_json(j, "Thickness", props.thickness);
		Helper::load_json(j, "Color", props.color);
	}
	void Border_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Border_Component>())
		{
			clone.add_component<Border_Component>(original.get_component<Border_Component>());
		}
	}
	bool Border_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::RectShape) ||
			NodeHelper::has_capability(caps, Node_Capability::CircleShape);
	}
	const char* Border_Component::get_name()
	{
		return "Border";
	}


	void Corner_Component::add_component(Entity entity)
	{
		entity.add_component<Corner_Component>();
	}
	void Corner_Component::remove_component(Entity entity)
	{
		entity.remove_component<Corner_Component>();
	}
	json Corner_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<Corner_Component>();
		Helper::save_json(j, "Corner", props.corner);
		Helper::save_json(j, "Uniform", props.uniform);

		return j;
	}
	void Corner_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Corner_Component>())
			entity.add_component<Corner_Component>();

		auto& props = entity.get_component<Corner_Component>();
		Helper::load_json(j, "Corner", props.corner);
		Helper::load_json(j, "Uniform", props.uniform);
	}
	void Corner_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Corner_Component>())
		{
			clone.add_component<Corner_Component>(original.get_component<Corner_Component>());
		}
	}
	bool Corner_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		// AERO_CORE_INFO("Node Type:{0}", static_cast<int>(type));
		return NodeHelper::has_capability(caps, Node_Capability::RectShape);
	}
	const char* Corner_Component::get_name()
	{
		return "Corner";
	}


	void UI_Component::add_component(Entity entity)
	{
		entity.add_component<UI_Component>();
	}
	void UI_Component::remove_component(Entity entity)
	{
		entity.remove_component<UI_Component>();
	}
	json UI_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<UI_Component>();
		Helper::save_json(j, "Mode", static_cast<int>(props.mode));

		return j;
	}
	void UI_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<UI_Component>())
			entity.add_component<UI_Component>();

		auto& props = entity.get_component<UI_Component>();
		Helper::load_json(j, "Mode", props.mode);
	}
	void UI_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<UI_Component>())
		{
			clone.add_component<UI_Component>(original.get_component<UI_Component>());
		}
	}
	bool UI_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::UI);
	}
	const char* UI_Component::get_name()
	{
		return "UI";
	}



	void Texture_Component::add_component(Entity entity)
	{
		entity.add_component<Texture_Component>();
	}
	void Texture_Component::remove_component(Entity entity)
	{
		entity.remove_component<Texture_Component>();
	}
	json Texture_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<Texture_Component>();
		Helper::save_json(j, "Path", props.path);

		return j;
	}
	void Texture_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Texture_Component>())
			entity.add_component<Texture_Component>();

		auto& props = entity.get_component<Texture_Component>();
		Helper::load_json(j, "Path", props.path);

		props.texture = NodeHelper::load_texture(props.path);
	}
	void Texture_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Texture_Component>())
		{
			clone.add_component<Texture_Component>(original.get_component<Texture_Component>());
		}
	}
	bool Texture_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
	}
	const char* Texture_Component::get_name()
	{
		return "Texture";
	}




	void TextureRect_Component::add_component(Entity entity)
	{
		entity.add_component<TextureRect_Component>();
	}
	void TextureRect_Component::remove_component(Entity entity)
	{
		entity.remove_component<TextureRect_Component>();
	}
	json TextureRect_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<TextureRect_Component>();
		Helper::save_json(j, "Rect", props.rect);

		return j;
	}
	void TextureRect_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<TextureRect_Component>())
			entity.add_component<TextureRect_Component>();

		auto& props = entity.get_component<TextureRect_Component>();
		Helper::load_json(j, "Rect", props.rect);
	}
	void TextureRect_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<TextureRect_Component>())
		{
			clone.add_component<TextureRect_Component>(original.get_component<TextureRect_Component>());
		}
	}
	bool TextureRect_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
	}
	const char* TextureRect_Component::get_name()
	{
		return "TextureRect";
	}




	void TextureFlip_Component::add_component(Entity entity)
	{
		entity.add_component<TextureFlip_Component>();
	}
	void TextureFlip_Component::remove_component(Entity entity)
	{
		entity.remove_component<TextureFlip_Component>();
	}
	json TextureFlip_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<TextureFlip_Component>();
		Helper::save_json(j, "Horizontal", props.horizontal);
		Helper::save_json(j, "Vertical", props.vertical);

		return j;
	}
	void TextureFlip_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<TextureFlip_Component>())
			entity.add_component<TextureFlip_Component>();

		auto& props = entity.get_component<TextureFlip_Component>();
		Helper::load_json(j, "Horizontal", props.horizontal);
		Helper::load_json(j, "Vertical", props.vertical);
	}
	void TextureFlip_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<TextureFlip_Component>())
		{
			clone.add_component<TextureFlip_Component>(original.get_component<TextureFlip_Component>());
		}
	}
	bool TextureFlip_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
	}
	const char* TextureFlip_Component::get_name()
	{
		return "TextureFlip";
	}




	void Camera_Component::add_component(Entity entity)
	{
		entity.add_component<Camera_Component>();
	}
	void Camera_Component::remove_component(Entity entity)
	{
		entity.remove_component<Camera_Component>();
	}
	json Camera_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<Camera_Component>();
		Helper::save_json(j, "Size", props.size);
		Helper::save_json(j, "Center", props.center);

		return j;
	}
	void Camera_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Camera_Component>())
			entity.add_component<Camera_Component>();

		auto& props = entity.get_component<Camera_Component>();
		Helper::load_json(j, "Size", props.size);
		Helper::load_json(j, "Center", props.center);
	}
	void Camera_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Camera_Component>())
		{
			clone.add_component<Camera_Component>(original.get_component<Camera_Component>());
		}
	}
	bool Camera_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Camera);
	}
	const char* Camera_Component::get_name()
	{
		return "Camera";
	}




	void Window_Component::add_component(Entity entity)
	{
		entity.add_component<Window_Component>();
	}
	void Window_Component::remove_component(Entity entity)
	{
		entity.remove_component<Window_Component>();
	}
	json Window_Component::save_json(Entity entity)
	{
		json j;

		auto& props = entity.get_component<Window_Component>();
		Helper::save_json(j, "Size", props.size);

		return j;
	}
	void Window_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Window_Component>())
			entity.add_component<Window_Component>();

		auto& props = entity.get_component<Window_Component>();
		Helper::load_json(j, "Size", props.size);
	}
	void Window_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Window_Component>())
		{
			clone.add_component<Window_Component>(original.get_component<Window_Component>());
		}
	}
	bool Window_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Camera);
	}
	const char* Window_Component::get_name()
	{
		return "Window";
	}





	json Animation::save_json(const Animation& animation)
	{
		json j;
		Helper::save_json(j, "Name", animation.name);
		Helper::save_json(j, "FPS", animation.fps);
		Helper::save_json(j, "PingPong", animation.ping_pong);
		Helper::save_json(j, "Loop", animation.loop);
		j["Frames"] = json::array();
		for (const auto& frame : animation.frames)
		{
			json frame_json;
			Helper::save_json(frame_json, "Rect", frame.frame_rect);
			j["Frames"].push_back(frame_json);
		}
		return j;
	}
	Animation Animation::load_json(const json& j)
	{
		Animation animation;
		Helper::load_json(j, "Name", animation.name);
		Helper::load_json(j, "FPS", animation.fps);
		Helper::load_json(j, "PingPong", animation.ping_pong);
		Helper::load_json(j, "Loop", animation.loop);
		if (j.contains("Frames") && j["Frames"].is_array())
		{
			for (const json& frame_json : j["Frames"])
			{
				Frame frame;
				Helper::load_json(frame_json, "Rect", frame.frame_rect);
				animation.frames.push_back(frame);
			}
		}
		return animation;
	}



	void Animation_Component::add_component(Entity entity)
	{
		entity.add_component<Animation_Component>();
	}
	void Animation_Component::remove_component(Entity entity)
	{
		entity.remove_component<Animation_Component>();
	}
	json Animation_Component::save_json(Entity entity)
	{
		json j;
		auto& props = entity.get_component<Animation_Component>();
		Helper::save_json(j, "Current", props.current_animation);
		Helper::save_json(j, "Playing", props.playing);

		j["Animations"] = json::object();
		for (const auto& [name, animation] : props.animations)
		{
			j["Animations"][name] = Animation::save_json(animation);
		}
		return j;
	}
	void Animation_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Animation_Component>())
			entity.add_component<Animation_Component>();

		auto& props = entity.get_component<Animation_Component>();
		Helper::load_json(j, "Current", props.current_animation);
		Helper::load_json(j, "Playing", props.playing);

		if (j.contains("Animations") && j["Animations"].is_object())
		{
			props.animations.clear();

			for (const auto& [name, animation_json] : j["Animations"].items())
			{
				Animation animation = Animation::load_json(animation_json);
				props.animations[name] = animation;
			}
		}
	}
	void Animation_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Animation_Component>())
		{
			clone.add_component<Animation_Component>(original.get_component<Animation_Component>());
		}
	}
	void Animation_Component::update(Entity entity, TimeStamp ts)
	{
		if (!entity.has_component<Animation_Component>())
			return;
		float dt = ts.get_seconds();

		auto& props = entity.get_component<Animation_Component>();

		if (!entity.has_component<Texture_Component>())
			return;

		if (props.current_animation.empty())
		{
			auto& texture = entity.get_component<Texture_Component>();
			if (texture.texture)
			{
				props.rect = { 0, 0, texture.texture->get_size() };
			}
			return;
		}



		auto it = props.animations.find(props.current_animation);
		if (props.animations.find(props.current_animation) == props.animations.end())
		{
			auto& texture = entity.get_component<Texture_Component>();
			if (texture.texture)
			{
				props.rect = { 0, 0, texture.texture->get_size() };
			}
			return;
		}

		Animation& anim = it->second;

		if (!props.playing)
		{
			props.rect = anim.frames[props.current_frame].frame_rect;
			return;
		}

		if (anim.frames.empty())
		{
			auto& texture = entity.get_component<Texture_Component>();
			props.rect = { 0, 0, texture.texture->get_size() };
			return;
		}

		if (props.current_frame < 0 || props.current_frame >= anim.frames.size())
			props.current_frame = 0;

		Frame& current_frame = anim.frames[props.current_frame];

		props.timer += dt;

		float duration = 1.0f / anim.fps;
		if (props.timer >= duration)
		{
			props.timer -= duration;
			props.current_frame++;
			if (props.current_frame >= anim.frames.size())
			{
				if (anim.loop)
				{
					props.current_frame = 0;
					props.current_animation_completed = true;
				}
				else
				{
					props.current_frame = anim.frames.size() - 1;
					props.playing = false;
					props.current_animation_completed = true;
				}
			}
		}

		if (props.current_animation_completed && anim.loop)
		{
			props.current_animation_completed = false;
		}

		props.rect = anim.frames[props.current_frame].frame_rect;
	}
	const char* Animation_Component::get_name()
	{
		return "Animations";
	}
	bool Animation_Component::play_animation(Entity entity, const std::string& name)
	{
		if (!entity.has_component<Animation_Component>())
			return false;

		auto& anim = entity.get_component<Animation_Component>();

		if (anim.animations.find(name) == anim.animations.end())
			return false;

		if (anim.current_animation == name)
			return false;

		anim.current_animation = name;
		anim.current_frame = 0;
		anim.timer = 0.0f;
		anim.current_animation_completed = false;
		return true;

	}




	json Tile_Defination::save_json(const Tile_Defination& def)
	{
		json j;
		Helper::save_json(j, "Rect", def.texture_rect);
		Helper::save_json(j, "Solid", def.is_solid);

		return j;
	}
	void Tile_Defination::load_json(Tile_Defination& def, const json& j)
	{
		Helper::load_json(j, "Rect", def.texture_rect);
		Helper::load_json(j, "Solid", def.is_solid);
	}


	json Tile::save_json(const Tile& tile)
	{
		json j;
		Helper::save_json(j, "ID", tile.tile_id);
		Helper::save_json(j, "SetID", tile.set_id);
		Helper::save_json(j, "AutoTile", tile.use_autotile);
		return j;
	}

	Tile Tile::load_json(const json& j)
	{
		Tile tile;
		Helper::load_json(j, "ID", tile.tile_id);
		Helper::load_json(j, "SetID", tile.set_id);
		Helper::load_json(j, "AutoTile", tile.use_autotile);

		return tile;
	}


	void Tile_Component::add_component(Entity entity)
	{
		entity.add_component<Tile_Component>();
	}
	void Tile_Component::remove_component(Entity entity)
	{
		entity.remove_component<Tile_Component>();
	}
	json Tile_Component::save_json(Entity entity)
	{
		json j;
		auto& tileset = entity.get_component<Tile_Component>();
		Helper::save_json(j, "Size", tileset.size);
		Helper::save_json(j, "Offset", tileset.offset);

		return j;
	}
	void Tile_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<Tile_Component>())
			entity.add_component<Tile_Component>();

		auto& tileset = entity.get_component<Tile_Component>();
		Helper::load_json(j, "Size", tileset.size);
		Helper::load_json(j, "Offset", tileset.offset);
	}
	void Tile_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Tile_Component>())
		{
			clone.add_component<Tile_Component>(original.get_component<Tile_Component>());
		}
	}
	const char* Tile_Component::get_name()
	{
		return "Tile";
	}


	void TileSet_Component::add_component(Entity entity)
	{
		entity.add_component<TileSet_Component>();
	}
	void TileSet_Component::remove_component(Entity entity)
	{
		entity.remove_component<TileSet_Component>();
	}
	json TileSet_Component::save_json(Entity entity)
	{
		json j;
		auto& tileset = entity.get_component<TileSet_Component>();
		if (!tileset.tile_definitions.empty())
			Helper::save_json(j, "Registered", tileset.is_tile_registered);

		Helper::save_json(j, "Size", tileset.tile_size);

		for (const auto& [id, def] : tileset.tile_definitions)
		{
			std::string key = std::to_string(id.x) + "," + std::to_string(id.y);
			j["Definations"][key] = Tile_Defination::save_json(def);
		}

		for (const auto& [grid, tile] : tileset.placed_tiles)
		{
			std::string key = std::to_string(grid.x) + "," + std::to_string(grid.y);
			j["Grid"][key] = Tile::save_json(tile);
		}

		return j;
	}
	void TileSet_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<TileSet_Component>())
			entity.add_component<TileSet_Component>();

		auto& tileset = entity.get_component<TileSet_Component>();

		Helper::load_json(j, "Size", tileset.tile_size);

		if (j.contains("Definations"))
		{
			for (auto& [key, def_json] : j["Definations"].items())
			{
				Tile_Defination def;
				Tile_Defination::load_json(def, def_json);

				vec2u id;
				sscanf(key.c_str(), "%u,%u", &id.x, &id.y);
				tileset.tile_definitions[id] = def;
			}

		}

		if (j.contains("Grid"))
		{
			for (auto& [key, id_json] : j["Grid"].items())
			{
				Tile tile = Tile::load_json(id_json);
				vec2u pos;
				sscanf(key.c_str(), "%u,%u", &pos.x, &pos.y);
				tileset.placed_tiles[pos] = tile;
			}
		}

		Helper::load_json(j, "Registered", tileset.is_tile_registered);
		if (!tileset.tile_definitions.empty())
			tileset.tile_changed = true;
	}
	void TileSet_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<TileSet_Component>())
		{
			clone.add_component<TileSet_Component>(original.get_component<TileSet_Component>());
		}
	}
	void TileSet_Component::update(Entity entity)
	{
		if (!entity.has_component<TileSet_Component>() || !entity.has_component<Tile_Component>() || !Engine::is_runtime())
			return;

		create_body(entity);
	}
	void TileSet_Component::draw(Entity entity)
	{
		if (!entity.has_component<TileSet_Component>() || !entity.has_component<Tile_Component>())
			return;

		auto& tile_set = entity.get_component<TileSet_Component>();
		auto& props = entity.get_component<Tile_Component>();
		Transform_Component trans;
		int entity_id = (int)(entity.get_id());
		Sprite sprite;
		sprite.size = props.size;

		for (const auto& [position, tile] : tile_set.placed_tiles)
		{

			auto tex_it = tile_set.tile_definitions.find(tile.tile_id);
			if (tex_it == tile_set.tile_definitions.end())
			{
				continue;
			}
			const Tile_Defination& def = tex_it->second;
			sprite.texture_rect = def.texture_rect;

			trans.position = (position * props.size) + props.size / 2 + props.offset;

			Renderer2D::draw_sprite(sprite, trans);
		}
	}
	const char* TileSet_Component::get_name()
	{
		return "TileSet";
	}
	bool TileSet_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::TileMap);
	}
	void TileSet_Component::create_body(Entity entity)
	{
		auto& props = entity.get_component<TileSet_Component>();
		if (props.body)
			return;

		auto& tile = entity.get_component<Tile_Component>();


		b2BodyDef body_def;
		body_def.type = b2_staticBody;
		body_def.position.Set(0.0f, 0.0f);

		auto scene = Scene::get_active_scene();
		auto& world = scene->get_world();
		props.body = world.CreateBody(&body_def);

		for (const auto& [pos, tile_detail] : props.placed_tiles)
		{
			auto tex_it = props.tile_definitions.find(tile_detail.tile_id);
			if (tex_it == props.tile_definitions.end())
			{
				continue;
			}
			const Tile_Defination& def = tex_it->second;
			if (def.is_solid)
			{
				vec2f position = (pos * tile.size) + tile.size / 2 + tile.offset;
				Math::pixels_to_meters(position);
				b2PolygonShape shape;
				vec2f size = tile.size;
				Math::pixels_to_meters(size);
				shape.SetAsBox(size.x / 2, size.y / 2, { position.x, position.y }, 0.0f);

				b2FixtureDef fixture_def;
				fixture_def.shape = &shape;
				fixture_def.density = 0.0f;
				fixture_def.friction = 0.5f;
				fixture_def.restitution = 0.1f;

				props.body->CreateFixture(&fixture_def);
			}
		}
	}



	json Auto_Tiles::save_json(const Auto_Tiles& tiles)
	{
		json j;

		for (const auto& [mask, id] : tiles.tile_bitmask)
		{
			std::string key = std::to_string(mask);
			Helper::save_json(j, key, id);
		}
		Helper::save_json(j, "SetID", tiles.set_id);
		return j;
	}
	Auto_Tiles Auto_Tiles::load_json(const json& j)
	{
		Auto_Tiles tiles;

		for (auto& [key_str, value] : j.items())
		{
			if (key_str == "SetID")
				continue;

			uint16_t mask = static_cast<uint16_t>(std::stoi(key_str));

			vec2u tile_id;
			Helper::load_json(j, key_str, tile_id);
			tiles.tile_bitmask[mask] = tile_id;
		}
		Helper::load_json(j, "SetID", tiles.set_id);
		return tiles;
	}
	void Auto_Tiles::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<Auto_Tiles>())
		{
			clone.add_component<Auto_Tiles>(original.get_component<Auto_Tiles>());
		}
	}
	const char* Auto_Tiles::get_name()
	{
		return "AutoTile";
	}



	void AutoTiling_Component::add_component(Entity entity)
	{
		entity.add_component<AutoTiling_Component>();
	}
	void AutoTiling_Component::remove_component(Entity entity)
	{
		auto& props = entity.get_component<AutoTiling_Component>();
		props.auto_tiles.clear();
		entity.remove_component<AutoTiling_Component>();
	}
	json AutoTiling_Component::save_json(Entity entity)
	{
		json j;
		const auto& props = entity.get_component<AutoTiling_Component>();

		j["AutoTiles"] = json::object();
		for (const auto& [key, auto_tile] : props.auto_tiles)
		{
			j["AutoTiles"][key] = Auto_Tiles::save_json(auto_tile);
		}

		return j;
	}
	void AutoTiling_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<AutoTiling_Component>())
			entity.add_component<AutoTiling_Component>();

		auto& props = entity.get_component<AutoTiling_Component>();

		if (j.contains("AutoTiles") && j["AutoTiles"].is_object())
		{
			props.auto_tiles.clear();

			for (const auto& [name, autotile_json] : j["AutoTiles"].items())
			{
				Auto_Tiles auto_tile = Auto_Tiles::load_json(autotile_json);
				props.auto_tiles[name] = auto_tile;
				props.next_id = std::max(auto_tile.set_id, props.next_id);
			}
		}
	}
	void AutoTiling_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<AutoTiling_Component>())
		{
			clone.add_component<AutoTiling_Component>(original.get_component<AutoTiling_Component>());
		}
	}
	const char* AutoTiling_Component::get_name()
	{
		return "AutoTileSet";
	}
	bool AutoTiling_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::TileMap);
	}



	void CollisionShape_Component::add_component(Entity entity)
	{
		CollisionShape_Component shape;
		if (entity.has_component<TextureRect_Component>())
		{
			auto& comps = entity.get_component<TextureRect_Component>();
			shape.size = comps.rect.size;
		}
		else if (entity.has_component<Render2D_Component>())
		{
			auto& props = entity.get_component<Render2D_Component>();
			shape.size = props.size;
		}
		entity.add_component<CollisionShape_Component>(shape);
	}
	void CollisionShape_Component::remove_component(Entity entity)
	{
		entity.remove_component<CollisionShape_Component>();
	}
	json CollisionShape_Component::save_json(Entity entity)
	{
		json j;

		const auto& shape = entity.get_component<CollisionShape_Component>();

		Helper::save_json(j, "ShapeType", (int)shape.shape_type);
		Helper::save_json(j, "Size", shape.size);
		Helper::save_json(j, "Radius", shape.radius);
		Helper::save_json(j, "Group", shape.group);
		j["Collide"] = json::array();
		for (int i = 0; i < 5; i++)
			j["Collide"].push_back(shape.collide_with[i]);

		return j;
	}
	void CollisionShape_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<CollisionShape_Component>())
			entity.add_component<CollisionShape_Component>();

		auto& shape = entity.get_component<CollisionShape_Component>();

		Helper::load_json(j, "ShapeType", shape.shape_type);
		Helper::load_json(j, "Size", shape.size);
		Helper::load_json(j, "Radius", shape.radius);
		Helper::load_json(j, "Group", shape.group);
		if (j.contains("Collide") && j["Collide"].is_array())
		{
			for (int i = 0; i < 5 && i < j["Collide"].size(); i++)
				shape.collide_with[i] = j["Collide"][i].get<bool>();
		}

	}
	void CollisionShape_Component::clone_entity(Entity original, Entity clone)
	{
		if (original.has_component<CollisionShape_Component>())
			clone.add_component<CollisionShape_Component>(original.get_component<CollisionShape_Component>());
	}
	const char* CollisionShape_Component::get_name()
	{
		return "CollisionShape";
	}
	bool CollisionShape_Component::is_compatible(NodeType type)
	{
		return true;
	}

	void PhysicsBody_Component::add_component(Entity entity)
	{
		entity.add_component<PhysicsBody_Component>();
		{
			CollisionShape_Component::add_component(entity);
		}
	}
	void PhysicsBody_Component::remove_component(Entity entity)
	{
		if (entity.has_component<PhysicsBody_Component>())
		{
			auto& props = entity.get_component<PhysicsBody_Component>();
			if (props.body)
			{
				auto scene = Scene::get_active_scene();
				auto& world = scene->get_world();
				world.DestroyBody(props.body);
				props.body = nullptr;
			}
			entity.remove_component<PhysicsBody_Component>();
		}
		entity.remove_component<CollisionShape_Component>();
	}
	json PhysicsBody_Component::save_json(Entity entity)
	{
		json j;
		auto& props = entity.get_component<PhysicsBody_Component>();
		Helper::save_json(j, "Body", static_cast<int>(props.body_type));
		Helper::save_json(j, "Rotation", props.rotation);

		NodeHelper::save_component<CollisionShape_Component>(entity, j);

		return j;
	}
	void PhysicsBody_Component::load_json(Entity entity, const json& j)
	{
		if (!entity.has_component<PhysicsBody_Component>())
			entity.add_component<PhysicsBody_Component>();

		auto& props = entity.get_component<PhysicsBody_Component>();
		Helper::load_json(j, "Body", props.body_type);
		Helper::load_json(j, "Rotation", props.rotation);

		NodeHelper::load_component<CollisionShape_Component>(entity, j);
	}
	void PhysicsBody_Component::clone_entity(Entity original, Entity clone)
	{
		if (!original.has_component<PhysicsBody_Component>())
			return;
		const auto& props = original.get_component<PhysicsBody_Component>();
		PhysicsBody_Component comps;
		comps.body_type = props.body_type;
		comps.rotation = props.rotation;

		clone.add_component<PhysicsBody_Component>(comps);

		CollisionShape_Component::clone_entity(original, clone);
	}
	void PhysicsBody_Component::delete_entity(Entity entity)
	{
		if (!entity.has_component<PhysicsBody_Component>())
			return;

		auto& props = entity.get_component<PhysicsBody_Component>();
		if (props.body)
		{
			auto scene = Scene::get_active_scene();
			auto& world = scene->get_world();
			world.DestroyBody(props.body);
			props.body = nullptr;
		}
	}
	void PhysicsBody_Component::update_entity(Entity entity)
	{
		if (!entity.has_component<PhysicsBody_Component>() || !entity.has_component<Transform_Component>() || !Engine::is_runtime())
			return;

		auto& props = entity.get_component<PhysicsBody_Component>();
		if (!props.body)
			create_body(entity);

		float angle = props.body->GetAngle();
		vec2f position = props.body->GetPosition();
		Math::meters_to_pixels(position);
		auto& transform = entity.get_component<Transform_Component>();
		transform.position = position;
		transform.rotation = Math::to_degree(angle);
		Transform_Component::get_local_transform(entity, transform);
	}
	const char* PhysicsBody_Component::get_name()
	{
		return "PhysicsBody";
	}
	bool PhysicsBody_Component::is_compatible(NodeType type)
	{
		auto caps = NodeHelper::get_node_capabilities(type);
		return NodeHelper::has_capability(caps, Node_Capability::Physics2D);
	}
	void PhysicsBody_Component::create_body(Entity entity)
	{
		if (!entity.has_component<PhysicsBody_Component>() || !entity.has_component<Transform_Component>() || !entity.has_component<CollisionShape_Component>())
			return;

		auto& props = entity.get_component<PhysicsBody_Component>();
		auto& shapes = entity.get_component<CollisionShape_Component>();
		if (props.body)
			return;

		b2BodyDef body_def;
		switch (props.body_type)
		{
		case BodyType::Dynamic: body_def.type = b2_dynamicBody; break;
		case BodyType::Static: body_def.type = b2_staticBody; break;
		case BodyType::Kinematic: body_def.type = b2_kinematicBody; break;
		default: body_def.type = b2_dynamicBody; break;
		}

		Transform_Component trans = Transform_Component::get_world_transform(entity);

		Math::pixels_to_meters(trans.position);
		body_def.position.Set(trans.position.x, trans.position.y);
		body_def.angle = Math::to_radians(trans.rotation);
		body_def.fixedRotation = !props.rotation;

		auto scene = Scene::get_active_scene();
		auto& world = scene->get_world();
		props.body = world.CreateBody(&body_def);



		vec2f size = shapes.size * trans.scale;
		Math::pixels_to_meters(size);
		b2PolygonShape shape_box;
		shape_box.SetAsBox(size.x / 2, size.y / 2);

		b2FixtureDef fixture_def;
		fixture_def.shape = &shape_box;
		fixture_def.isSensor = false;

		if (props.body_type == BodyType::Dynamic)
			fixture_def.density = 1.0f;
		else
			fixture_def.density = 0.0f;

		fixture_def.friction = 0.0f;

		b2Fixture* fixture = props.body->CreateFixture(&fixture_def);

		if (fixture)
		{
			b2Filter filter;
			filter.categoryBits = 1 << (shapes.group - 1);
			filter.maskBits = 0;

			for (int i = 0; i < 5; i++)
			{
				if (shapes.collide_with[i])
					filter.maskBits |= 1 << i;
			}

			fixture->SetFilterData(filter);
		}


		props.body->GetUserData().pointer = (uintptr_t)entity.get_id();

	}

	vec2f Text::calc_text_size(const Text& text, const vec2f& s)
	{
		vec2f scale;
		scale = s * (text.font_size / TextLoader::font.em_size);
		vec2f size = { 0, 0 };
		float line_height = (TextLoader::font.ascender - TextLoader::font.descender) * TextLoader::font.em_size * scale.y;
		size.y = line_height;
		float width = 0.0f;

		for (char c : text.text)
		{
			if (c == '\n')
			{
				size.x = std::max(size.x, width);
				width = 0.0f;
				size.y += line_height;
				continue;
			}
			auto it = TextLoader::font.glyphs.find(c);
			if (it != TextLoader::font.glyphs.end())
			{
				width += it->second.advance * scale.x;
			}
		}
		size.x = std::max(size.x, width);
		return size;
	}
	vec2f Text::center_text(const Text& text, const Transform_Component& transform)
	{
		vec2f size = calc_text_size(text, transform.scale);
		return transform.position - (size * 0.5f);
	}


}