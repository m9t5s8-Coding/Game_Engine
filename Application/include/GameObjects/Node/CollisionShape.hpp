#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>


namespace ag
{
	enum class ShapeType
	{
		Rectangle,
		Circle
	};
	enum class BodyType
	{
		Dynamic = 0,
		Static = 1,
		Kinematic = 2
	};

	struct CollisionShape
	{
		struct CollisionShapeProps
		{
			ShapeType type = ShapeType::Rectangle;
			vec2u size = { 100, 100 };

			
			b2Body* body = nullptr;
			BodyType body_type = BodyType::Dynamic;

			int group_number = 1;
			bool collide_with[5] = { true, true, true, true, true };
			bool fixed_rotation = true;

			static json save(Entity entity)
			{
				json j;
				auto& props = entity.get_component<CollisionShapeProps>();
				Helper::save_json(j, "Size", props.size);
				Helper::save_json(j, "Shape Type", static_cast<int>(props.type));
				Helper::save_json(j, "Body Type", static_cast<int>(props.body_type));
				Helper::save_json(j, "Groud Number", props.group_number);
				Helper::save_json(j, "Rotation" ,props.fixed_rotation);

				j["Collide With"] = json::array();
				for (int i = 0; i < 5; i++)
					j["Collide With"].push_back(props.collide_with[i]);


				return j;
			}

			static void load(Entity entity, const json& j)
			{
				auto& props = entity.get_component<CollisionShapeProps>();
				Helper::load_json(j, "Size", props.size);
				Helper::load_json(j, "Shape Type", props.type);
				Helper::load_json(j, "Body Type", props.body_type);
				Helper::load_json(j, "Groud Number", props.group_number);
				Helper::load_json(j, "Rotation", props.fixed_rotation);

				if (j.contains("Collide With") && j["Collide With"].is_array())
				{
					for (int i = 0; i < 5 && i < j["Collide With"].size(); i++)
						props.collide_with[i] = j["Collide With"][i].get<bool>();
				}
			}
		};

		static void create_node(Entity entity)
		{

			auto& tag = entity.get_component<Tag>();
			entity.add_component<Transform>();

			entity.add_component<CollisionShapeProps>();
			
		}

		static void delete_node(Entity entity)
		{
			auto& props = entity.get_component<CollisionShapeProps>();
			if (props.body)
			{
				auto scene = Scene::get_active_scene();
				auto& world = scene->get_world();
				world.DestroyBody(props.body);
				props.body = nullptr;
			}
			ScriptComponent::destroy(entity);
			entity.delete_entity();
		}

		static void clone_node(Entity original, Entity clone)
		{
			clone.add_component<Transform>(original.get_component<Transform>());
			if (original.has_component<ScriptComponent>())
			{
				clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
			}
			auto& original_props = original.get_component<CollisionShapeProps>();
			CollisionShapeProps props;

			props.type = original_props.type;
			props.size = original_props.size;
			props.body_type = original_props.body_type;
			props.group_number = original_props.group_number;
			props.fixed_rotation = original_props.fixed_rotation;
			for (int i = 0; i < 5; i++)
			{
				props.collide_with[i] = original_props.collide_with[i];
			}

			clone.add_component<CollisionShapeProps>(props);
			if (!Engine::is_runtime())
				return;
			
		}

		static json save_json(Entity entity)
		{
			json j;
			j["Transform"] = Transform::save(entity);
			j["CollisionShapeProps"] = CollisionShapeProps::save(entity);

			if (entity.has_component<ScriptComponent>())
			{
				j["ScriptComponent"] = ScriptComponent::save_json(entity);
			}
			return j;
		}

		static void load_json(Entity entity, json j)
		{
			CollisionShapeProps::load(entity, j["CollisionShapeProps"]);
			Transform::load(entity, j["Transform"]);

			if (j.contains("ScriptComponent"))
			{
				ScriptComponent::load_json(entity, j["ScriptComponent"]);
			}

			if (Engine::is_runtime())
				create_body(entity);
		}

		static void show_properties(Entity entity)
		{
			Tag::show_properties(entity);
			{
				auto& props = entity.get_component<CollisionShapeProps>();
				UI::draw_vec2("Size", props.size);

				const char* bodyTypeNames[] = { "Dynamic", "Static", "Kinematic" };

				ImGui::SliderInt("Group Number", &props.group_number, 1, 5);

				int currentIndex = static_cast<int>(props.body_type);

				ImGui::Text("Collides With:");
				for (int i = 0; i < 5; i++)
				{
					char label[16];
					sprintf(label, "Group %d", i + 1);
					ImGui::Checkbox(label, &props.collide_with[i]);
				}

				if (ImGui::BeginCombo("Body Type", bodyTypeNames[currentIndex]))
				{
					for (int i = 0; i < IM_ARRAYSIZE(bodyTypeNames); i++)
					{
						bool isSelected = (currentIndex == i);
						if (ImGui::Selectable(bodyTypeNames[i], isSelected))
						{
							props.body_type = static_cast<BodyType>(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();

					
				}
				ImGui::Checkbox("Fixed Rotation", &props.fixed_rotation);
			}
			Transform::show_properties(entity);
		}

		static void update(Entity entity, TimeStamp ts)
		{
			auto& tag = entity.get_component<Tag>();
			if (!tag.is_visible || !Engine::is_runtime())
				return;
			ScriptComponent::update(entity, ts);
			auto& props = entity.get_component<CollisionShapeProps>();
			if (!props.body)
				create_body(entity);


			b2Vec2 pos = props.body->GetPosition();
			float angle = props.body->GetAngle();
			vec2f position = vec2f(pos.x, pos.y);
			Math::meters_to_pixels(position);
			auto& transform = entity.get_component<Transform>();
			transform.position = position;
			transform.rotation = Math::to_degree(angle);
			Transform::get_local_transform(entity, transform);
		}

		static void draw(Entity entity)
		{
			auto is_visible = entity.get_component<Tag>().is_visible;
			if (!is_visible || Engine::is_runtime())
				return;

			auto& props = entity.get_component<CollisionShapeProps>();
			auto trans = Transform::get_world_transform(entity);
			if (props.type == ShapeType::Rectangle)
			{
				Rectangle rect;
				rect.size = props.size;
				rect.fill_color = Color(0, 255, 255, 40);
				rect.border_color = Color(255, 0, 255, 255);
				rect.border_thickness = -1.0f;
				Renderer2D::draw_rectangle(rect, trans);
			}
		}
		
		static void create_body(Entity entity)
		{
			auto& props = entity.get_component<CollisionShapeProps>();
			b2BodyDef body_def;
			switch (props.body_type)
			{
			case BodyType::Dynamic: body_def.type = b2_dynamicBody; break;
			case BodyType::Static: body_def.type = b2_staticBody; break;
			case BodyType::Kinematic: body_def.type = b2_kinematicBody; break;
			default: body_def.type = b2_dynamicBody; break;
			}

			Transform trans = Transform::get_world_transform(entity);
			Math::pixels_to_meters(trans.position);
			body_def.position.Set(trans.position.x, trans.position.y);
			body_def.angle = Math::to_radians(trans.rotation);
			body_def.fixedRotation = props.fixed_rotation;

			auto scene = Scene::get_active_scene();
			auto& world = scene->get_world();
			props.body = world.CreateBody(&body_def);

			vec2f size = props.size * trans.scale;
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

			fixture_def.friction = 0.3f;

			b2Fixture* fixture = props.body->CreateFixture(&fixture_def);

			if (fixture)
			{
				b2Filter filter;
				filter.categoryBits = 1 << (props.group_number - 1); // this entity's group
				filter.maskBits = 0;

				for (int i = 0; i < 5; i++)
				{
					if (props.collide_with[i])
						filter.maskBits |= 1 << i;
				}

				fixture->SetFilterData(filter);
			}

		}
	};
}
