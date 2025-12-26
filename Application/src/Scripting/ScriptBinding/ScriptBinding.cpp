#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Aero.hpp>
#include <Scripting/ScriptableEntity.hpp>


namespace ag
{
	void ScriptBinding::register_keyboard_polling()
	{
		auto& lua = ScriptManager::get_lua();

		sol::table keyboard = lua.create_named_table("Keyboard");

		lua.new_enum<ag::KeyCode>("Key",
			{
					{"Space", ag::Key::Space},
					{"Apostrophe", ag::Key::Apostrophe},
					{"Comma", ag::Key::Comma},
					{"Minus", ag::Key::Minus},
					{"Period", ag::Key::Period},
					{"Slash", ag::Key::Slash},

					{"D0", ag::Key::D0}, {"D1", ag::Key::D1}, {"D2", ag::Key::D2}, {"D3", ag::Key::D3},
					{"D4", ag::Key::D4}, {"D5", ag::Key::D5}, {"D6", ag::Key::D6}, {"D7", ag::Key::D7},
					{"D8", ag::Key::D8}, {"D9", ag::Key::D9},

					{"Semicolon", ag::Key::Semicolon},
					{"Equal", ag::Key::Equal},

					{"A", ag::Key::A}, {"B", ag::Key::B}, {"C", ag::Key::C}, {"D", ag::Key::D},
					{"E", ag::Key::E}, {"F", ag::Key::F}, {"G", ag::Key::G}, {"H", ag::Key::H},
					{"I", ag::Key::I}, {"J", ag::Key::J}, {"K", ag::Key::K}, {"L", ag::Key::L},
					{"M", ag::Key::M}, {"N", ag::Key::N}, {"O", ag::Key::O}, {"P", ag::Key::P},
					{"Q", ag::Key::Q}, {"R", ag::Key::R}, {"S", ag::Key::S}, {"T", ag::Key::T},
					{"U", ag::Key::U}, {"V", ag::Key::V}, {"W", ag::Key::W}, {"X", ag::Key::X},
					{"Y", ag::Key::Y}, {"Z", ag::Key::Z},

					{"LeftBracket", ag::Key::LeftBracket},
					{"Backslash", ag::Key::Backslash},
					{"RightBracket", ag::Key::RightBracket},
					{"GraveAccent", ag::Key::GraveAccent},

					{"World1", ag::Key::World1},
					{"World2", ag::Key::World2},

					// Function keys
					{"Escape", ag::Key::Escape}, {"Enter", ag::Key::Enter}, {"Tab", ag::Key::Tab},
					{"Backspace", ag::Key::Backspace}, {"Insert", ag::Key::Insert}, {"Delete", ag::Key::Delete},

					{"Right", ag::Key::Right}, {"Left", ag::Key::Left}, {"Down", ag::Key::Down}, {"Up", ag::Key::Up},

					{"PageUp", ag::Key::PageUp}, {"PageDown", ag::Key::PageDown}, {"Home", ag::Key::Home}, {"End", ag::Key::End},

					{"CapsLock", ag::Key::CapsLock}, {"ScrollLock", ag::Key::ScrollLock}, {"NumLock", ag::Key::NumLock},
					{"PrintScreen", ag::Key::PrintScreen}, {"Pause", ag::Key::Pause},

					{"F1", ag::Key::F1}, {"F2", ag::Key::F2}, {"F3", ag::Key::F3}, {"F4", ag::Key::F4},
					{"F5", ag::Key::F5}, {"F6", ag::Key::F6}, {"F7", ag::Key::F7}, {"F8", ag::Key::F8},
					{"F9", ag::Key::F9}, {"F10", ag::Key::F10}, {"F11", ag::Key::F11}, {"F12", ag::Key::F12},
					{"F13", ag::Key::F13}, {"F14", ag::Key::F14}, {"F15", ag::Key::F15}, {"F16", ag::Key::F16},
					{"F17", ag::Key::F17}, {"F18", ag::Key::F18}, {"F19", ag::Key::F19}, {"F20", ag::Key::F20},
					{"F21", ag::Key::F21}, {"F22", ag::Key::F22}, {"F23", ag::Key::F23}, {"F24", ag::Key::F24},
					{"F25", ag::Key::F25},

					// Keypad
					{"KP0", ag::Key::KP0}, {"KP1", ag::Key::KP1}, {"KP2", ag::Key::KP2}, {"KP3", ag::Key::KP3},
					{"KP4", ag::Key::KP4}, {"KP5", ag::Key::KP5}, {"KP6", ag::Key::KP6}, {"KP7", ag::Key::KP7},
					{"KP8", ag::Key::KP8}, {"KP9", ag::Key::KP9},

					{"KPDecimal", ag::Key::KPDecimal}, {"KPDivide", ag::Key::KPDivide}, {"KPMultiply", ag::Key::KPMultiply},
					{"KPSubtract", ag::Key::KPSubtract}, {"KPAdd", ag::Key::KPAdd}, {"KPEnter", ag::Key::KPEnter},
					{"KPEqual", ag::Key::KPEqual},

					// Modifiers
					{"LeftShift", ag::Key::LeftShift}, {"LeftControl", ag::Key::LeftControl}, {"LeftAlt", ag::Key::LeftAlt}, {"LeftSuper", ag::Key::LeftSuper},
					{"RightShift", ag::Key::RightShift}, {"RightControl", ag::Key::RightControl}, {"RightAlt", ag::Key::RightAlt}, {"RightSuper", ag::Key::RightSuper},
					{"Menu", ag::Key::Menu}
			});


		// Keyboard.is_key_down(Key.W)
		keyboard.set_function("is_key_down", [](ag::KeyCode key) -> bool {
			return ag::Keyboard::is_key_pressed(key);
			});

		// Keyboard.is_key_up(Key.W)
		keyboard.set_function("is_key_up", [](ag::KeyCode key) -> bool {
			return ag::Keyboard::is_key_released(key);
			});
	}

	void ScriptBinding::register_mouse_polling()
	{
		auto& lua = ScriptManager::get_lua();

		sol::table mouse = lua.create_named_table("Mouse");

		lua.new_enum<ag::MouseCode>("Button",
			{
				{"Left", ag::Button::Button0},
				{"Right", ag::Button::Button1},
				{"Middle", ag::Button::Button2},
			});

		// Mouse.is_button_down(Button.Left)
		mouse.set_function("is_button_down", [](ag::MouseCode button) -> bool {
			return ag::Mouse::is_mouse_pressed(button);
			});

		// Mouse.is_button_up(Button.Left)
		mouse.set_function("is_button_up", [](ag::MouseCode button) -> bool {
			return ag::Mouse::is_mouse_released(button);
			});

		// Mouse.get_mouse_position()
		mouse.set_function("get_mouse_position", sol::overload(

			[]() -> ag::vec2f {
				return ag::Mouse::get_mouse_position();
			},

			[](ag::Entity& entity) -> vec2f {
				auto type = NodeHelper::get_nodetype(entity);

				if (type == NodeType::Camera)
				{
					const auto& props = entity.get_component<CameraComponent::CameraProps>();
					float_rect window_rect = { props.view_center - props.view_size / 2, props.view_size * props.zoom };
					vec2f screen_pos = ag::Mouse::get_mouse_position();
					vec2f window_size = Application::get().get_window().get_size();
					return Math::screen_to_world(screen_pos, window_rect, window_size);
				}
				return { 0, 0 };
			}
		));

		// Mouse.set_mouse_position(position)
		mouse.set_function("set_mouse_position", [](ag::vec2f position) {
			ag::Mouse::set_mouse_position(position);
			});

	}

	void ScriptBinding::register_events()
	{
		auto& lua = ScriptManager::get_lua();

		lua.new_enum<ag::Event_Type>("EventType",
			{
					{"Key_Pressed", ag::Event_Type::Key_Pressed},
					{"Key_Released", ag::Event_Type::Key_Released},
					{"Button_Pressed", ag::Event_Type::Mouse_Button_Pressed},
					{"Button_Released", ag::Event_Type::Mouse_Button_Released},
					{"Mouse_Moved", ag::Event_Type::Mouse_Moved},
					{"Mouse_Scrolled", ag::Event_Type::Mouse_Scrolled}
			});

		lua.new_usertype<ag::Event>("Event",
			"get_type", &ag::Event::get_event_type
		);


		// Key Events
		{
			lua.set_function("get_key", [&lua](ag::Event& event) -> sol::object {
				auto type = event.get_event_type();
				if (type == ag::Event_Type::Key_Pressed) {
					return sol::make_object(lua, static_cast<ag::KeyPressedEvent&>(event).get_key_code());
				}
				else if (type == ag::Event_Type::Key_Released) {
					return sol::make_object(lua, static_cast<ag::KeyReleasedEvent&>(event).get_key_code());
				}
				else if (type == ag::Event_Type::Key_Typed) {
					return sol::make_object(lua, static_cast<ag::KeyTypedEvent&>(event).get_key_code());
				}
				return sol::nil;
				});

			lua.set_function("is_key_event", [](ag::Event& event) -> bool {
				auto type = event.get_event_type();
				return type == ag::Event_Type::Key_Pressed ||
					type == ag::Event_Type::Key_Released ||
					type == ag::Event_Type::Key_Typed;
				});
		}


		//Mouse Events
		{

			//Button Pressed and Released
			lua.set_function("get_button", [&lua](ag::Event& event) -> sol::object {
				auto type = event.get_event_type();
				if (type == ag::Event_Type::Mouse_Button_Pressed) {
					return sol::make_object(lua, static_cast<ag::MouseButtonPressedEvent&>(event).get_mouse_button());
				}
				else if (type == ag::Event_Type::Mouse_Button_Released) {
					return sol::make_object(lua, static_cast<ag::MouseButtonReleasedEvent&>(event).get_mouse_button());
				}
				return sol::nil;
				});

			lua.set_function("is_button_event", [](ag::Event& event) -> bool {
				auto type = event.get_event_type();
				return type == ag::Event_Type::Mouse_Button_Pressed ||
					type == ag::Event_Type::Mouse_Button_Released ||
					type == ag::Event_Type::Key_Typed;
				});



			//Scroll Event
			lua.set_function("get_scroll", [&lua](ag::Event& event) -> sol::object {
				auto type = event.get_event_type();
				if (type == ag::Event_Type::Mouse_Scrolled) {
					auto& e = static_cast<MouseScrolledEvent&>(event);
					return sol::make_object(lua, std::make_pair(e.get_offsetX(), e.get_offsetY()));
				}
				return sol::nil;
				});

			lua.set_function("get_scroll_x", [&lua](ag::Event& event) -> sol::object {
				auto type = event.get_event_type();
				if (type == ag::Event_Type::Mouse_Scrolled) {
					auto& e = static_cast<MouseScrolledEvent&>(event);
					return sol::make_object(lua, e.get_offsetX());
				}
				return sol::nil;
				});

			lua.set_function("get_scroll_y", [&lua](ag::Event& event) -> sol::object {
				auto type = event.get_event_type();
				if (type == ag::Event_Type::Mouse_Scrolled) {
					auto& e = static_cast<MouseScrolledEvent&>(event);
					return sol::make_object(lua, e.get_offsetY());
				}
				return sol::nil;
				});

			lua.set_function("is_scroll_event", [](ag::Event& event) -> bool {
				auto type = event.get_event_type();

				return type == ag::Event_Type::Mouse_Scrolled;
				});
		}




	}

	void ScriptBinding::register_color()
	{
		auto& lua = ScriptManager::get_lua();

		lua.new_usertype<ag::Color>("Color",
			sol::constructors< ag::Color(), ag::Color(uint8_t, uint8_t, uint8_t), ag::Color(uint8_t, uint8_t, uint8_t, uint8_t) >(),
			"r", &ag::Color::r,
			"g", &ag::Color::g,
			"b", &ag::Color::b,
			"a", &ag::Color::a,

			sol::meta_function::equal_to, &ag::Color::operator==
		);

		lua["Color"]["Transparent"] = ag::Color::Transparent;
		lua["Color"]["White"] = ag::Color::White;
		lua["Color"]["Black"] = ag::Color::Black;
		lua["Color"]["Red"] = ag::Color::Red;
		lua["Color"]["Green"] = ag::Color::Green;
		lua["Color"]["Blue"] = ag::Color::Blue;
		lua["Color"]["Yellow"] = ag::Color::Yellow;
		lua["Color"]["Cyan"] = ag::Color::Cyan;
		lua["Color"]["Magenta"] = ag::Color::Magenta;
		lua["Color"]["Orange"] = ag::Color::Orange;
		lua["Color"]["Purple"] = ag::Color::Purple;
		lua["Color"]["Pink"] = ag::Color::Pink;
		lua["Color"]["Brown"] = ag::Color::Brown;
		lua["Color"]["Gray"] = ag::Color::Gray;
		lua["Color"]["LightGray"] = ag::Color::LightGray;
		lua["Color"]["DarkGray"] = ag::Color::DarkGray;
		lua["Color"]["Lime"] = ag::Color::Lime;
		lua["Color"]["Navy"] = ag::Color::Navy;
		lua["Color"]["Teal"] = ag::Color::Teal;
		lua["Color"]["Olive"] = ag::Color::Olive;
		lua["Color"]["Maroon"] = ag::Color::Maroon;

	}


	//TODO overload with scalar and vec2
	void ScriptBinding::register_vec2()
	{
		auto& lua = ScriptManager::get_lua();

		lua.new_usertype<ag::vec2f>("vec2f",
			sol::call_constructor, sol::factories([](float x, float y) { return ag::vec2f(x, y); }),
			"x", &ag::vec2f::x,
			"y", &ag::vec2f::y,

			"__add", sol::overload(
				sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator+),
				sol::resolve<ag::vec2f(const float) const>(&ag::vec2f::operator+)
			),
			"__sub", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator-),
			"__mul", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator*),
			"__div", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator/),
			"__eq", sol::resolve<bool(const ag::vec2f&) const>(&ag::vec2f::operator==),
			"__tostring", [](const ag::vec2f& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
		);

		lua.new_usertype<ag::vec2i>("vec2i",
			sol::call_constructor, sol::factories([](int x, int y) { return ag::vec2i(x, y); }),
			"x", &ag::vec2i::x,
			"y", &ag::vec2i::y,
			"__add", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator+),
			"__sub", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator-),
			"__mul", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator*),
			"__div", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator/),
			"__eq", sol::resolve<bool(const ag::vec2i&) const>(&ag::vec2i::operator==),
			"__tostring", [](const ag::vec2i& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
		);

		lua.new_usertype<ag::vec2u>("vec2u",
			sol::call_constructor, sol::factories([](ag::AG_uint x, int y) { return ag::vec2u(x, y); }),
			"x", &ag::vec2u::x,
			"y", &ag::vec2u::y,
			"__add", sol::resolve<ag::vec2u(const ag::vec2u&) const>(&ag::vec2u::operator+),
			"__sub", sol::resolve<ag::vec2u(const ag::vec2u&) const>(&ag::vec2u::operator-),
			"__mul", sol::resolve<ag::vec2u(const ag::vec2u&) const>(&ag::vec2u::operator*),
			"__div", sol::resolve<ag::vec2u(const ag::vec2u&) const>(&ag::vec2u::operator/),
			"__eq", sol::resolve<bool(const ag::vec2u&) const>(&ag::vec2u::operator==),
			"__tostring", [](const ag::vec2u& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
		);
	}


	void ScriptBinding::register_node()
	{
		auto& lua = ScriptManager::get_lua();

		// Tag Component
		{
			// Visibility
			lua.set_function("is_visible", [](ag::Entity& entity) -> bool {
				return NodeHelper::get_comp_value(entity, &Tag::is_visible, true);
				});

			lua.set_function("set_visible", [](ag::Entity& entity, const bool visible) {
				NodeHelper::set_comp_value(entity, &Tag::is_visible, visible);
				});
		}

		// Transform Component
		{
			// Position
			lua.set_function("get_position", [](ag::Entity& entity) -> vec2f
				{
					return NodeHelper::get_comp_value(entity, &Transform::position, vec2f(0, 0));
				});

			lua.set_function("set_position", [](ag::Entity& entity, const vec2f& position)
				{
					NodeHelper::set_comp_value(entity, &Transform::position, position);
				});

			lua.set_function("move", [](ag::Entity& entity, const vec2f& delta) {
				auto position = NodeHelper::get_comp_value(entity, &Transform::position, vec2f(0, 0));
				position += delta;
				NodeHelper::set_comp_value(entity, &Transform::position, position);
				});


			// Scale
			lua.set_function("get_scale", [](ag::Entity& entity) -> vec2f {
				return NodeHelper::get_comp_value(entity, &Transform::scale, vec2f(1, 1));
				});

			lua.set_function("set_scale", [](ag::Entity& entity, const vec2f& scale) {
				NodeHelper::set_comp_value(entity, &Transform::scale, scale);
				});


			//Rotation
			lua.set_function("get_rotation", [](ag::Entity& entity) -> float
				{
					return NodeHelper::get_comp_value(entity, &Transform::rotation, 0.0f);
				});

			lua.set_function("set_rotation", [](ag::Entity& entity, const float rotation)
				{
					NodeHelper::set_comp_value(entity, &Transform::rotation, rotation);
				});


		}

		// Rectangle Node and Circle Node
		{
			// Fill Color
			lua.set_function("get_fill_color", [](ag::Entity& entity) -> Color {
				auto node_type = NodeHelper::get_nodetype(entity);
				switch (node_type)
				{
				case ag::NodeType::Rectangle:
					return NodeHelper::get_comp_value(entity, &RectangleNode::Rectangle_Component::color, Color::Transparent);

				case ag::NodeType::Circle:
					return NodeHelper::get_comp_value(entity, &CircleNode::Circle_Component::color, Color::Transparent);

				default:
					return ag::Color::Transparent;
				}
				});

			lua.set_function("set_fill_color", [](ag::Entity& entity, const ag::Color& color) {
				auto node_type = entity.get_component<Tag>().node_type;
				switch (node_type)
				{
				case ag::NodeType::Rectangle:
				{
					NodeHelper::set_comp_value(entity, &RectangleNode::Rectangle_Component::color, color);
					return;
				}
				case ag::NodeType::Circle:
				{
					NodeHelper::set_comp_value(entity, &CircleNode::Circle_Component::color, color);
					return;
				}
				default:
				{
					return;
				}
				}
				});


			// Border Color
			lua.set_function("get_border_color", [](ag::Entity& entity) -> Color {
				auto node_type = NodeHelper::get_nodetype(entity);

				return NodeHelper::get_comp_value(entity, &Border_Component::color, Color::Transparent);

				});

			lua.set_function("set_border_color", [](ag::Entity& entity, const ag::Color& color) {
				auto node_type = NodeHelper::get_nodetype(entity);
				NodeHelper::set_comp_value(entity, &Border_Component::color, color);
				});


			// Border Color
			lua.set_function("get_border_thickness", [](ag::Entity& entity) -> float {
				return NodeHelper::get_comp_value(entity, &Border_Component::thickness, 0.0f);
				});

			lua.set_function("set_border_thickness", [](ag::Entity& entity, const float& thickness) {
				NodeHelper::set_comp_value(entity, &Border_Component::thickness, thickness);
				});

			// Sprite Node Rectangle and Circle
			{
				lua.set_function("get_size", [](ag::Entity& entity) -> vec2f {
					auto type = NodeHelper::get_nodetype(entity);

					if (type == NodeType::Rectangle)
					{
						return NodeHelper::get_comp_value(entity, &RectangleNode::Rectangle_Component::size, { 0, 0 });
					}
					else if (type == NodeType::Circle)
					{
						return NodeHelper::get_comp_value(entity, &CircleNode::Circle_Component::size, { 0, 0 });
					}
					else if (type == NodeType::Sprite)
					{
						return NodeHelper::get_comp_value(entity, &SpriteNode::SpriteProp::size, { 0, 0 });
					}
					else if (type == NodeType::AnimatedSprite2D)
					{
						auto texture_rect = NodeHelper::get_comp_value(entity, &AnimatedSprite2DNode::AnimatedSpriteProps::texture_rect, { 0, 0, 0, 0 });
						return texture_rect.size;
					}
					return { 0, 0 };
					});
			}

			//Animated Sprite 2D and Sprite Node
			{

				lua.set_function("play_animation", [](ag::Entity& entity, const std::string& animation_name) -> bool {

					auto type = NodeHelper::get_nodetype(entity);
					if (type == NodeType::AnimatedSprite2D)
					{
						return AnimatedSprite2DNode::play_animation(entity, animation_name);
					}
					return false;
					});

				lua.set_function("flip_vertical", [](ag::Entity& entity, const bool vertical) {
					auto type = NodeHelper::get_nodetype(entity);
					switch (type)
					{
					case ag::NodeType::Sprite:
						NodeHelper::set_comp_value(entity, &SpriteNode::SpriteProp::flip_vertical, vertical);
						return;

					case ag::NodeType::AnimatedSprite2D:
						NodeHelper::set_comp_value(entity, &AnimatedSprite2DNode::AnimatedSpriteProps::flip_vertical, vertical);
						return;

					default:
						return;
					}
					});

				lua.set_function("flip_horizontal", [](ag::Entity& entity, const bool horizontal) {
					auto type = NodeHelper::get_nodetype(entity);
					switch (type)
					{
					case ag::NodeType::Sprite:
						NodeHelper::set_comp_value(entity, &SpriteNode::SpriteProp::flip_horizontal, horizontal);
						return;

					case ag::NodeType::AnimatedSprite2D:
						NodeHelper::set_comp_value(entity, &AnimatedSprite2DNode::AnimatedSpriteProps::flip_horizontal, horizontal);
						return;

					default:
						return;
					}
					});

			}



			lua.set_function("duplicate_entity", [](ag::Entity& entity) -> ag::Entity {
				auto parent = entity.get_component<Tag>().parent;
				auto e = Scene::get_active_scene()->duplicate_entity(entity, parent);
				return e;
				});

			lua.set_function("delete_entity", [](ag::Entity& entity) {
				const auto& scene = Scene::get_active_scene();
				scene->destroy_entity(entity);
				});

			lua.set_function("set_velocity", [](ag::Entity& entity, const vec2f& velocity) {
				auto& tag = entity.get_component<Tag>();
				if (tag.node_type != NodeType::CollisionShape)
					return;
				auto& body = entity.get_component<CollisionShape::CollisionShapeProps>().body;
				if (!body)
					return;

				vec2f v = velocity;
				Math::pixels_to_meters(v);
				body->SetLinearVelocity(b2Vec2(v.x, v.y));
				});

			lua.set_function("set_velocity_x", [](ag::Entity& entity, float velocity) {
				auto& tag = entity.get_component<Tag>();
				if (tag.node_type != NodeType::CollisionShape)
					return;
				auto& body = entity.get_component<CollisionShape::CollisionShapeProps>().body;
				if (!body)
					return;

				float v = velocity;
				Math::pixels_to_meters(v);
				body->SetLinearVelocity(b2Vec2(v, body->GetLinearVelocity().y));
				});


			lua.set_function("set_awake", [](ag::Entity& entity, bool awake) {
				auto type = NodeHelper::get_nodetype(entity);
				if (type == NodeType::CollisionShape)
				{
					auto& body = entity.get_component<CollisionShape::CollisionShapeProps>().body;
					if (!body)
						return;
					body->SetAwake(awake);
				}
				});

			lua.set_function("reload_scene", []() {
				auto scene_path = Scene::get_active_scene()->get_directory();
				auto project = Project::get_active_project();
				std::string path = project->get_directory() + project->get_scene_directory() + scene_path;

				auto scene = SaveScene::load_scene(path);
				Scene::set_active_scene(scene);

				});

			lua.set_function("set_text", [](ag::Entity& entity, const std::string& value) {
				auto type = NodeHelper::get_nodetype(entity);
				if (type == NodeType::TextNode)
				{
					auto& text = entity.get_component< TextNode::TextProp>().text;
					text.text = value;
					AERO_CORE_INFO("Text:{0}", text.text);
				}
				});

		}
	}

}
