#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Aero.hpp>

namespace ag
{
	namespace detail
	{
		template<typename Component, typename MemberType>
		MemberType safe_get_comp_value(Entity& entity, MemberType Component::* member, const MemberType& default_value)
		{
			return NodeHelper::get_comp_value(entity, member, default_value);
		}

		template<typename Component, typename MemberType>
		void safe_set_comp_value(Entity& entity, MemberType Component::* member, const MemberType& value)
		{
			NodeHelper::set_comp_value(entity, member, value);
		}

		template<typename Component>
		bool has_component(Entity& entity)
		{
			return entity.has_component<Component>();
		}
	}

	void ScriptBinding::register_keyboard_polling()
	{
		auto& lua = ScriptManager::get_lua();
		sol::table keyboard = lua.create_named_table("Keyboard");

		// Register key codes enum
		lua.new_enum<KeyCode>("Key", {
			// Printable keys
			{"Space", Key::Space},
			{"Apostrophe", Key::Apostrophe},
			{"Comma", Key::Comma},
			{"Minus", Key::Minus},
			{"Period", Key::Period},
			{"Slash", Key::Slash},

			// Number keys
			{"D0", Key::D0}, {"D1", Key::D1}, {"D2", Key::D2}, {"D3", Key::D3},
			{"D4", Key::D4}, {"D5", Key::D5}, {"D6", Key::D6}, {"D7", Key::D7},
			{"D8", Key::D8}, {"D9", Key::D9},

			// Special characters
			{"Semicolon", Key::Semicolon},
			{"Equal", Key::Equal},

			// Letter keys
			{"A", Key::A}, {"B", Key::B}, {"C", Key::C}, {"D", Key::D},
			{"E", Key::E}, {"F", Key::F}, {"G", Key::G}, {"H", Key::H},
			{"I", Key::I}, {"J", Key::J}, {"K", Key::K}, {"L", Key::L},
			{"M", Key::M}, {"N", Key::N}, {"O", Key::O}, {"P", Key::P},
			{"Q", Key::Q}, {"R", Key::R}, {"S", Key::S}, {"T", Key::T},
			{"U", Key::U}, {"V", Key::V}, {"W", Key::W}, {"X", Key::X},
			{"Y", Key::Y}, {"Z", Key::Z},

			// Brackets and special
			{"LeftBracket", Key::LeftBracket},
			{"Backslash", Key::Backslash},
			{"RightBracket", Key::RightBracket},
			{"GraveAccent", Key::GraveAccent},
			{"World1", Key::World1},
			{"World2", Key::World2},

			// Function keys
			{"Escape", Key::Escape}, {"Enter", Key::Enter}, {"Tab", Key::Tab},
			{"Backspace", Key::Backspace}, {"Insert", Key::Insert}, {"Delete", Key::Delete},

			// Arrow keys
			{"Right", Key::Right}, {"Left", Key::Left},
			{"Down", Key::Down}, {"Up", Key::Up},

			// Navigation keys
			{"PageUp", Key::PageUp}, {"PageDown", Key::PageDown},
			{"Home", Key::Home}, {"End", Key::End},

			// Lock keys
			{"CapsLock", Key::CapsLock}, {"ScrollLock", Key::ScrollLock},
			{"NumLock", Key::NumLock}, {"PrintScreen", Key::PrintScreen},
			{"Pause", Key::Pause},

			// Function keys F1-F25
			{"F1", Key::F1}, {"F2", Key::F2}, {"F3", Key::F3}, {"F4", Key::F4},
			{"F5", Key::F5}, {"F6", Key::F6}, {"F7", Key::F7}, {"F8", Key::F8},
			{"F9", Key::F9}, {"F10", Key::F10}, {"F11", Key::F11}, {"F12", Key::F12},
			{"F13", Key::F13}, {"F14", Key::F14}, {"F15", Key::F15}, {"F16", Key::F16},
			{"F17", Key::F17}, {"F18", Key::F18}, {"F19", Key::F19}, {"F20", Key::F20},
			{"F21", Key::F21}, {"F22", Key::F22}, {"F23", Key::F23}, {"F24", Key::F24},
			{"F25", Key::F25},

			// Keypad
			{"KP0", Key::KP0}, {"KP1", Key::KP1}, {"KP2", Key::KP2}, {"KP3", Key::KP3},
			{"KP4", Key::KP4}, {"KP5", Key::KP5}, {"KP6", Key::KP6}, {"KP7", Key::KP7},
			{"KP8", Key::KP8}, {"KP9", Key::KP9},

			// Keypad operations
			{"KPDecimal", Key::KPDecimal}, {"KPDivide", Key::KPDivide},
			{"KPMultiply", Key::KPMultiply}, {"KPSubtract", Key::KPSubtract},
			{"KPAdd", Key::KPAdd}, {"KPEnter", Key::KPEnter}, {"KPEqual", Key::KPEqual},

			// Modifiers
			{"LeftShift", Key::LeftShift}, {"LeftControl", Key::LeftControl},
			{"LeftAlt", Key::LeftAlt}, {"LeftSuper", Key::LeftSuper},
			{"RightShift", Key::RightShift}, {"RightControl", Key::RightControl},
			{"RightAlt", Key::RightAlt}, {"RightSuper", Key::RightSuper},
			{"Menu", Key::Menu}
			});

		keyboard.set_function("is_key_down", [](KeyCode key) -> bool {
			return Keyboard::is_key_pressed(key);
			});

		keyboard.set_function("is_key_up", [](KeyCode key) -> bool {
			return Keyboard::is_key_released(key);
			});
	}

	void ScriptBinding::register_mouse_polling()
	{
		auto& lua = ScriptManager::get_lua();
		sol::table mouse = lua.create_named_table("Mouse");

		// Register mouse button enum
		lua.new_enum<MouseCode>("Button", {
			{"Left", Button::Button0},
			{"Right", Button::Button1},
			{"Middle", Button::Button2}
			});

		// Mouse button state functions
		mouse.set_function("is_button_down", [](MouseCode button) -> bool {
			return Mouse::is_mouse_pressed(button);
			});

		mouse.set_function("is_button_up", [](MouseCode button) -> bool {
			return Mouse::is_mouse_released(button);
			});

		mouse.set_function("get_mouse_position", sol::overload(
			[]() -> vec2f {
				return Mouse::get_mouse_position();
			},

			[](Entity& entity) -> vec2f {
				auto type = NodeHelper::get_nodetype(entity);

				if (type == NodeType::Camera)
				{
					const auto& props = entity.get_component<Camera_Component>();
					float_rect window_rect = { props.center - props.size / 2, props.size };
					vec2f screen_pos = Mouse::get_mouse_position();
					vec2f window_size = Application::get().get_window().get_size();
					return Math::screen_to_world(screen_pos, window_rect, window_size);
				}
				return vec2f(0, 0);
			}
		));

		mouse.set_function("set_mouse_position", [](const vec2f& position) {
			Mouse::set_mouse_position(position);
			});
	}

	void ScriptBinding::register_events()
	{
		auto& lua = ScriptManager::get_lua();

		// Register event types enum
		lua.new_enum<Event_Type>("EventType", {
			{"Key_Pressed", Event_Type::Key_Pressed},
			{"Key_Released", Event_Type::Key_Released},
			{"Button_Pressed", Event_Type::Mouse_Button_Pressed},
			{"Button_Released", Event_Type::Mouse_Button_Released},
			{"Mouse_Moved", Event_Type::Mouse_Moved},
			{"Mouse_Scrolled", Event_Type::Mouse_Scrolled}
			});

		// Register base Event type
		lua.new_usertype<Event>("Event",
			"get_type", &Event::get_event_type
		);



		// KEYBOARD EVENT FUNCTIONS

		lua.set_function("get_key", [&lua](Event& event) -> sol::object {
			auto type = event.get_event_type();

			switch (type) {
			case Event_Type::Key_Pressed:
				return sol::make_object(lua, static_cast<KeyPressedEvent&>(event).get_key_code());
			case Event_Type::Key_Released:
				return sol::make_object(lua, static_cast<KeyReleasedEvent&>(event).get_key_code());
			case Event_Type::Key_Typed:
				return sol::make_object(lua, static_cast<KeyTypedEvent&>(event).get_key_code());
			default:
				return sol::nil;
			}
			});

		lua.set_function("is_key_event", [](Event& event) -> bool {
			auto type = event.get_event_type();
			return type == Event_Type::Key_Pressed ||
				type == Event_Type::Key_Released ||
				type == Event_Type::Key_Typed;
			});

		// MOUSE BUTTON EVENT FUNCTIONS

		lua.set_function("get_button", [&lua](Event& event) -> sol::object {
			auto type = event.get_event_type();

			switch (type) {
			case Event_Type::Mouse_Button_Pressed:
				return sol::make_object(lua, static_cast<MouseButtonPressedEvent&>(event).get_mouse_button());
			case Event_Type::Mouse_Button_Released:
				return sol::make_object(lua, static_cast<MouseButtonReleasedEvent&>(event).get_mouse_button());
			default:
				return sol::nil;
			}
			});

		lua.set_function("is_button_event", [](Event& event) -> bool {
			auto type = event.get_event_type();
			return type == Event_Type::Mouse_Button_Pressed ||
				type == Event_Type::Mouse_Button_Released;
			});

		// MOUSE SCROLL EVENT FUNCTIONS

		lua.set_function("get_scroll", [&lua](Event& event) -> sol::object {
			if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
				auto& e = static_cast<MouseScrolledEvent&>(event);
				return sol::make_object(lua, std::make_pair(e.get_offsetX(), e.get_offsetY()));
			}
			return sol::nil;
			});

		lua.set_function("get_scroll_x", [&lua](Event& event) -> sol::object {
			if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
				return sol::make_object(lua, static_cast<MouseScrolledEvent&>(event).get_offsetX());
			}
			return sol::nil;
			});

		lua.set_function("get_scroll_y", [&lua](Event& event) -> sol::object {
			if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
				return sol::make_object(lua, static_cast<MouseScrolledEvent&>(event).get_offsetY());
			}
			return sol::nil;
			});

		lua.set_function("is_scroll_event", [](Event& event) -> bool {
			return event.get_event_type() == Event_Type::Mouse_Scrolled;
			});
	}

	void ScriptBinding::register_color()
	{
		auto& lua = ScriptManager::get_lua();

		lua.new_usertype<Color>("Color",
			sol::constructors<
			Color(),
			Color(uint8_t, uint8_t, uint8_t),
			Color(uint8_t, uint8_t, uint8_t, uint8_t)
			>(),
			"r", &Color::r,
			"g", &Color::g,
			"b", &Color::b,
			"a", &Color::a,
			sol::meta_function::equal_to, &Color::operator==,
			sol::meta_function::to_string, [](const Color& c) {
				return "Color(" + std::to_string(c.r) + ", " +
					std::to_string(c.g) + ", " +
					std::to_string(c.b) + ", " +
					std::to_string(c.a) + ")";
			}
		);

		auto color_table = lua["Color"];
		color_table["Transparent"] = Color::Transparent;
		color_table["White"] = Color::White;
		color_table["Black"] = Color::Black;
		color_table["Red"] = Color::Red;
		color_table["Green"] = Color::Green;
		color_table["Blue"] = Color::Blue;
		color_table["Yellow"] = Color::Yellow;
		color_table["Cyan"] = Color::Cyan;
		color_table["Magenta"] = Color::Magenta;
		color_table["Orange"] = Color::Orange;
		color_table["Purple"] = Color::Purple;
		color_table["Pink"] = Color::Pink;
		color_table["Brown"] = Color::Brown;
		color_table["Gray"] = Color::Gray;
		color_table["LightGray"] = Color::LightGray;
		color_table["DarkGray"] = Color::DarkGray;
		color_table["Lime"] = Color::Lime;
		color_table["Navy"] = Color::Navy;
		color_table["Teal"] = Color::Teal;
		color_table["Olive"] = Color::Olive;
		color_table["Maroon"] = Color::Maroon;
	}

	void ScriptBinding::register_vec2()
	{
		auto& lua = ScriptManager::get_lua();

		// vec2f (float vector)
		lua.new_usertype<vec2f>("vec2f",
			sol::call_constructor, sol::factories([](float x, float y) {
				return vec2f(x, y);
				}),
			"x", &vec2f::x,
			"y", &vec2f::y,

			sol::meta_function::addition, sol::overload(
				sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator+),
				sol::resolve<vec2f(const float) const>(&vec2f::operator+)
			),
			sol::meta_function::subtraction, sol::overload(
				sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator-),
				sol::resolve<vec2f(const float) const>(&vec2f::operator-)
			),
			sol::meta_function::multiplication, sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator*),
			sol::meta_function::division, sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator/),
			sol::meta_function::equal_to, sol::resolve<bool(const vec2f&) const>(&vec2f::operator==),
			sol::meta_function::to_string, [](const vec2f& v) {
				return "vec2f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
			},

			"length", [](const vec2f& v) { return std::sqrt(v.x * v.x + v.y * v.y); },
			"normalized", [](const vec2f& v) {
				float len = std::sqrt(v.x * v.x + v.y * v.y);
				return len > 0 ? vec2f(v.x / len, v.y / len) : vec2f(0, 0);
			},
			"dot", [](const vec2f& v1, const vec2f& v2) {
				return v1.x * v2.x + v1.y * v2.y;
			}
		);

		// vec2i (integer vector)
		lua.new_usertype<vec2i>("vec2i",
			sol::call_constructor, sol::factories([](int x, int y) {
				return vec2i(x, y);
				}),
			"x", &vec2i::x,
			"y", &vec2i::y,

			sol::meta_function::addition, sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator+),
			sol::meta_function::subtraction, sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator-),
			sol::meta_function::multiplication, sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator*),
			sol::meta_function::division, sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator/),
			sol::meta_function::equal_to, sol::resolve<bool(const vec2i&) const>(&vec2i::operator==),
			sol::meta_function::to_string, [](const vec2i& v) {
				return "vec2i(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
			}
		);

		// vec2u (unsigned integer vector)
		lua.new_usertype<vec2u>("vec2u",
			sol::call_constructor, sol::factories([](AG_uint x, AG_uint y) {
				return vec2u(x, y);
				}),
			"x", &vec2u::x,
			"y", &vec2u::y,

			sol::meta_function::addition, sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator+),
			sol::meta_function::subtraction, sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator-),
			sol::meta_function::multiplication, sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator*),
			sol::meta_function::division, sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator/),
			sol::meta_function::equal_to, sol::resolve<bool(const vec2u&) const>(&vec2u::operator==),
			sol::meta_function::to_string, [](const vec2u& v) {
				return "vec2u(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
			}
		);
	}

	void ScriptBinding::register_node()
	{
		auto& lua = ScriptManager::get_lua();





		// TAG COMPONENT FUNCTIONS

		lua.set_function("is_visible", [](Entity& entity) -> bool {
			return detail::safe_get_comp_value(entity, &Tag_Component::visible, true);
			});

		lua.set_function("set_visible", [](Entity& entity, bool visible) {
			detail::safe_set_comp_value(entity, &Tag_Component::visible, visible);
			});




		// TRANSFORM COMPONENT FUNCTIONS

		// Position
		lua.set_function("get_position", [](Entity& entity) -> vec2f {
			return detail::safe_get_comp_value(entity, &Transform_Component::position, vec2f(0, 0));
			});

		lua.set_function("set_position", [](Entity& entity, const vec2f& position) {
			detail::safe_set_comp_value(entity, &Transform_Component::position, position);
			});

		lua.set_function("move", [](Entity& entity, const vec2f& delta) {
			auto position = detail::safe_get_comp_value(entity, &Transform_Component::position, vec2f(0, 0));
			position += delta;
			detail::safe_set_comp_value(entity, &Transform_Component::position, position);
			});

		// Scale
		lua.set_function("get_scale", [](Entity& entity) -> vec2f {
			return detail::safe_get_comp_value(entity, &Transform_Component::scale, vec2f(1, 1));
			});

		lua.set_function("set_scale", [](Entity& entity, const vec2f& scale) {
			detail::safe_set_comp_value(entity, &Transform_Component::scale, scale);
			});

		// Rotation
		lua.set_function("get_rotation", [](Entity& entity) -> float {
			return detail::safe_get_comp_value(entity, &Transform_Component::rotation, 0.0f);
			});

		lua.set_function("set_rotation", [](Entity& entity, float rotation) {
			detail::safe_set_comp_value(entity, &Transform_Component::rotation, rotation);
			});

		lua.set_function("rotate", [](Entity& entity, float delta) {
			auto rotation = detail::safe_get_comp_value(entity, &Transform_Component::rotation, 0.0f);
			rotation += delta;
			detail::safe_set_comp_value(entity, &Transform_Component::rotation, rotation);
			});




		// Fill color
		lua.set_function("get_fill_color", [](Entity& entity) -> Color {
			return detail::safe_get_comp_value(entity, &Render2D_Component::color, Color::Transparent);
			});

		lua.set_function("set_fill_color", [](Entity& entity, const Color& color) {
			detail::safe_set_comp_value(entity, &Render2D_Component::color, color);
			});

		// Size
		lua.set_function("get_size", [](Entity& entity) -> vec2f {
			auto size = detail::safe_get_comp_value(entity, &Render2D_Component::size, vec2u(0, 0));
			return vec2f(size);
			});

		lua.set_function("set_size", [](Entity& entity, const vec2f& size) {
			detail::safe_set_comp_value(entity, &Render2D_Component::size, vec2u(size));
			});


		lua.set_function("get_border_color", [](Entity& entity) -> Color {
			return detail::safe_get_comp_value(entity, &Border_Component::color, Color::Transparent);
			});

		lua.set_function("set_border_color", [](Entity& entity, const Color& color) {
			detail::safe_set_comp_value(entity, &Border_Component::color, color);
			});

		lua.set_function("get_border_thickness", [](Entity& entity) -> float {
			return detail::safe_get_comp_value(entity, &Border_Component::thickness, 0.0f);
			});

		lua.set_function("set_border_thickness", [](Entity& entity, float thickness) {
			detail::safe_set_comp_value(entity, &Border_Component::thickness, thickness);
			});


		lua.set_function("play_animation", [](Entity& entity, const std::string& name) -> bool {
			return Animation_Component::play_animation(entity, name, false);
			});

		lua.set_function("play_animation_restart", [](Entity& entity, const std::string& name) -> bool {
			return Animation_Component::play_animation(entity, name, true);
			});

		lua.set_function("is_animation_complete", [](Entity& entity) -> bool {
			if (detail::has_component<Animation_Component>(entity)) {
				return entity.get_component<Animation_Component>().current_animation_completed;
			}
			return false;
			});

		lua.set_function("get_current_animation", [](Entity& entity) -> std::string {
			if (detail::has_component<Animation_Component>(entity)) {
				return entity.get_component<Animation_Component>().current_animation;
			}
			return "";
			});



		lua.set_function("flip_vertical", [](Entity& entity, bool vertical) {
			detail::safe_set_comp_value(entity, &TextureFlip_Component::vertical, vertical);
			});

		lua.set_function("flip_horizontal", [](Entity& entity, bool horizontal) {
			detail::safe_set_comp_value(entity, &TextureFlip_Component::horizontal, horizontal);
			});



		lua.set_function("duplicate_entity", [](Entity& entity) -> Entity {
			auto& parent = entity.get_component<Tag_Component>().parent;
			return Scene::get_active_scene()->duplicate_entity(entity, parent);
			});

		lua.set_function("delete_entity", [](Entity& entity) {
			Scene::get_active_scene()->destroy_entity(entity);
			});





		// TEXT COMPONENT FUNCTIONS

		lua.set_function("set_text", [](Entity& entity, const std::string& value) {
			detail::safe_set_comp_value(entity, &Text_Component::text, value);
			});

		lua.set_function("get_text", [](Entity& entity) -> std::string {
			return detail::safe_get_comp_value(entity, &Text_Component::text, std::string(""));
			});



		// TWEEN COMPONENT FUNCTIONS

		lua.set_function("play_tween", [](Entity& entity) -> bool {
			return Tween_Component::play_tween(entity);
			});

		lua.set_function("set_tween_start", [](Entity& entity, const vec2f& position) {
			detail::safe_set_comp_value(entity, &Tween_Component::start_position, position);
			});

		lua.set_function("set_tween_end", [](Entity& entity, const vec2f& position) {
			detail::safe_set_comp_value(entity, &Tween_Component::end_position, position);
			});

		lua.set_function("set_tween_duration", [](Entity& entity, float duration) {
			detail::safe_set_comp_value(entity, &Tween_Component::duration, duration);
			});

		lua.set_function("is_tween_completed", [](Entity& entity) -> bool {
			auto state = detail::safe_get_comp_value(
				entity,
				&Tween_Component::state,
				Tween_Component::State::STOPPED
			);
			return state == Tween_Component::State::COMPLETED;
			});



		// UTILITY FUNCTIONS

		lua.set_function("is_hovered", [](Entity& entity) -> bool {
			auto state = detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
			return state & Button_State::Hovered;
			});

		lua.set_function("is_pressed", [](Entity& entity) -> bool {
			auto state = detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
			return state & Button_State::Pressed;
			});

		lua.set_function("is_disabled", [](Entity& entity) -> bool {
			auto state = detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
			return state & Button_State::Disabled;
			});

		lua.set_function("disable_button", [](Entity& entity, bool disabled)
			{
				if (!entity.has_component<ButtonState_Component>())
					return;

				auto& state = entity.get_component<ButtonState_Component>().button_state;
				if (disabled)
					state |= Button_State::Disabled;
				else
					state &= ~Button_State::Disabled;
			});




		lua.set_function("reload_scene", []() {
			auto scene_path = Scene::get_active_scene()->get_directory();
			auto project = Project::get_active_project();
			std::string path = project->get_directory() +
				project->get_scene_directory() +
				scene_path;

			auto scene = SaveScene::load_scene(path);
			Scene::set_active_scene(scene);
			});






	}

	void ScriptBinding::register_audio_functions()
	{
		auto& lua = ScriptManager::get_lua();

		lua.set_function("play_audio", [](Entity& entity) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->play();
			}
			});

		lua.set_function("pause_audio", [](Entity& entity) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->pause();
			}
			});

		lua.set_function("stop_audio", [](Entity& entity) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->stop();
			}
			});


		lua.set_function("is_playing_audio", [](Entity& entity) -> bool {
			if (detail::has_component<Audio_Component>(entity)) {
				return entity.get_component<Audio_Component>().source->is_playing();
			}
			return false;
			});

		lua.set_function("is_paused_audio", [](Entity& entity) -> bool {
			if (detail::has_component<Audio_Component>(entity)) {
				return entity.get_component<Audio_Component>().source->is_paused();
			}
			return false;
			});

		lua.set_function("is_loop_audio", [](Entity& entity) -> bool {
			if (detail::has_component<Audio_Component>(entity)) {
				return entity.get_component<Audio_Component>().source->is_looping();
			}
			return false;
			});


		lua.set_function("set_loop_audio", [](Entity& entity, bool loop) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->set_loop(loop);
			}
			});

		lua.set_function("set_pitch_audio", [](Entity& entity, float pitch) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->set_pitch(pitch);
			}
			});

		lua.set_function("get_pitch_audio", [](Entity& entity) -> float {
			if (detail::has_component<Audio_Component>(entity)) {
				return entity.get_component<Audio_Component>().source->get_pitch();
			}
			return 1.0f;
			});

		lua.set_function("set_volume_audio", [](Entity& entity, float volume) {
			if (detail::has_component<Audio_Component>(entity)) {
				entity.get_component<Audio_Component>().source->set_volume(volume);
			}
			});

		lua.set_function("get_volume_audio", [](Entity& entity) -> float {
			if (detail::has_component<Audio_Component>(entity)) {
				return entity.get_component<Audio_Component>().source->get_volume();
			}
			return 1.0f;
			});
	}

	void ScriptBinding::register_physics()
	{
		auto& lua = ScriptManager::get_lua();

		lua.set_function("set_velocity", [](Entity& entity, const vec2f& velocity) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;

			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;

			vec2f v = velocity;
			Math::pixels_to_meters(v);
			body->SetLinearVelocity(b2Vec2(v.x, v.y));
			});

		lua.set_function("apply_impulse", [](Entity& entity, const vec2f& impulse)
			{
				if (!detail::has_component<PhysicsBody_Component>(entity)) return;

				auto& body = entity.get_component<PhysicsBody_Component>().body;
				if (!body) return;

				vec2f i = impulse;
				Math::pixels_to_meters(i);
				body->ApplyLinearImpulseToCenter(i.to_b2vec2(), true);
			});

		lua.set_function("set_velocity_x", [](Entity& entity, float velocity) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;

			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;

			float v = Math::pixels_to_meters(velocity);
			body->SetLinearVelocity(b2Vec2(v, body->GetLinearVelocity().y));
			});

		lua.set_function("set_velocity_y", [](Entity& entity, float velocity) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;

			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;

			float v = Math::pixels_to_meters(velocity);
			body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, v));
			});


		lua.set_function("apply_impulse_x", [](Entity& entity, float impulse)
			{
				if (!detail::has_component<PhysicsBody_Component>(entity)) return;

				auto& body = entity.get_component<PhysicsBody_Component>().body;
				if (!body) return;

				float v = Math::pixels_to_meters(impulse);
				body->ApplyLinearImpulseToCenter(b2Vec2(v, 0), true);
			});

		lua.set_function("apply_impulse_y", [](Entity& entity, float impulse)
			{
				if (!detail::has_component<PhysicsBody_Component>(entity)) return;

				auto& body = entity.get_component<PhysicsBody_Component>().body;
				if (!body) return;

				float v = Math::pixels_to_meters(impulse);
				body->ApplyLinearImpulseToCenter(b2Vec2(0, v), true);
			});


		lua.set_function("get_velocity", [](Entity& entity) -> vec2f {
			if (!detail::has_component<PhysicsBody_Component>(entity))
			{
				return vec2f(0.0f, 0.0f);
			}

			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return vec2f(0.0f, 0.0f);

			vec2f velocity = body->GetLinearVelocity();
			Math::meters_to_pixels(velocity);
			return velocity;
			});

		lua.set_function("set_world_gravity", [](const vec2f& gravity)
			{
				auto& world = Scene::get_active_scene()->get_world();
				world.SetGravity(gravity.to_b2vec2());
			});

		lua.set_function("set_world_gravity_scale", [](float scale)
			{
				auto& world = Scene::get_active_scene()->get_world();
				vec2f gravity = world.GetGravity();
				gravity *= scale;
				world.SetGravity(gravity.to_b2vec2());
			});

		lua.set_function("set_gravity_scale", [](Entity& entity, float scale)
			{
				if (entity.has_component<PhysicsBody_Component>())
				{
					auto body = entity.get_component<PhysicsBody_Component>().body;
					if (!body)
						return;

					body->SetGravityScale(scale);
				}
			});

		lua.set_function("set_awake", [](Entity& entity, bool awake) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;

			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (body) {
				body->SetAwake(awake);
			}
			});

		lua.set_function("is_on_ground", [](Entity& entity) -> bool
			{
				auto scene = Scene::get_active_scene();
				return scene->get_contact_listener()->is_grounded(entity.get_id());
			});

		lua.set_function("is_collided", [](Entity& entity1, Entity& entity2) -> bool
			{
				auto scene = Scene::get_active_scene();
				return scene->get_contact_listener()->is_collided(entity1.get_id(), entity2.get_id());
			});


		lua.set_function("is_collided", [](Entity& entity1) -> bool
			{
				auto scene = Scene::get_active_scene();
				return scene->get_contact_listener()->is_collided(entity1.get_id());
			});



		lua.set_function("get_mass", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetMass();
			});

		lua.set_function("get_inertia", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetInertia();
			});

		lua.set_function("get_center_of_mass", [](Entity& entity) -> vec2f
			{
				if (!detail::has_component<PhysicsBody_Component>(entity)) return vec2f(0, 0);
				auto& body = entity.get_component<PhysicsBody_Component>().body;
				if (!body) return vec2f(0, 0);
				vec2f com = body->GetWorldCenter();
				Math::meters_to_pixels(com);
				return com;
			});


		lua.set_function("get_local_center_of_mass", [](Entity& entity) -> vec2f
			{
				if (!detail::has_component<PhysicsBody_Component>(entity)) return vec2f(0, 0);
				auto& body = entity.get_component<PhysicsBody_Component>().body;
				if (!body) return vec2f(0, 0);
				vec2f com = body->GetLocalCenter();
				Math::meters_to_pixels(com);
				return com;
			});


		lua.set_function("get_linear_damping", [](Entity& entity) -> float
			{
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetLinearDamping();
			});

		lua.set_function("set_linear_damping", [](Entity& entity, float damping) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetLinearDamping(damping);
			});

		lua.set_function("get_angular_damping", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetAngularDamping();
			});

		lua.set_function("set_angular_damping", [](Entity& entity, float damping) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetAngularDamping(damping);
			});

		lua.set_function("get_angular_velocity", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetAngularVelocity();
			});

		lua.set_function("set_angular_velocity", [](Entity& entity, float velocity) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetAngularVelocity(velocity);
			});

		lua.set_function("apply_torque", [](Entity& entity, float torque) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->ApplyTorque(torque, true);
			});

		lua.set_function("apply_angular_impulse", [](Entity& entity, float impulse) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->ApplyAngularImpulse(impulse, true);
			});

		lua.set_function("apply_force", [](Entity& entity, const vec2f& force) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			vec2f f = force;
			Math::pixels_to_meters(f);
			body->ApplyForceToCenter(f.to_b2vec2(), true);
			});

		lua.set_function("apply_force_at_point", [](Entity& entity, const vec2f& force, const vec2f& point) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			vec2f f = force;
			vec2f p = point;
			Math::pixels_to_meters(f);
			Math::pixels_to_meters(p);
			body->ApplyForce(f.to_b2vec2(), p.to_b2vec2(), true);
			});

		lua.set_function("apply_impulse_at_point", [](Entity& entity, const vec2f& impulse, const vec2f& point) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			vec2f i = impulse;
			vec2f p = point;
			Math::pixels_to_meters(i);
			Math::pixels_to_meters(p);
			body->ApplyLinearImpulse(i.to_b2vec2(), p.to_b2vec2(), true);
			});

		lua.set_function("is_awake", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->IsAwake();
			});

		lua.set_function("is_active", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->IsEnabled();
			});

		lua.set_function("set_active", [](Entity& entity, bool active) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetEnabled(active);
			});

		lua.set_function("is_static", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->GetType() == b2_staticBody;
			});

		lua.set_function("is_dynamic", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->GetType() == b2_dynamicBody;
			});

		lua.set_function("is_kinematic", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->GetType() == b2_kinematicBody;
			});

		lua.new_enum<b2BodyType>("BodyType", {
				{"Static", b2_staticBody},
				{"Kinematic", b2_kinematicBody},
				{"Dynamic", b2_dynamicBody}
			});

		lua.set_function("set_body_type", [](Entity& entity, b2BodyType type) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetType(type);
			});

		lua.set_function("get_body_type", [&lua](Entity& entity) -> sol::object {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return sol::nil;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return sol::nil;
			return sol::make_object(lua, body->GetType());
			});

		lua.set_function("get_fixed_rotation", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->IsFixedRotation();
			});

		lua.set_function("set_fixed_rotation", [](Entity& entity, bool fixed) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetFixedRotation(fixed);
			});

		lua.set_function("is_bullet", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			return body->IsBullet();
			});

		lua.set_function("set_bullet", [](Entity& entity, bool bullet) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			body->SetBullet(bullet);
			});

		lua.set_function("get_friction", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return 0.0f;
			return fixture->GetFriction();
			});

		lua.set_function("set_friction", [](Entity& entity, float friction) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return;
			fixture->SetFriction(friction);
			});

		lua.set_function("get_restitution", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return 0.0f;
			return fixture->GetRestitution();
			});

		lua.set_function("set_restitution", [](Entity& entity, float restitution) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return;
			fixture->SetRestitution(restitution);
			});

		lua.set_function("get_density", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return 0.0f;
			return fixture->GetDensity();
			});

		lua.set_function("set_density", [](Entity& entity, float density) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return;
			fixture->SetDensity(density);
			body->ResetMassData();  // must recalc after density change
			});

		lua.set_function("is_sensor", [](Entity& entity) -> bool {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return false;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return false;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return false;
			return fixture->IsSensor();
			});

		lua.set_function("set_sensor", [](Entity& entity, bool sensor) {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return;
			auto* fixture = body->GetFixtureList();
			if (!fixture) return;
			fixture->SetSensor(sensor);
			});

		lua.set_function("get_world_gravity", [](Entity& entity) -> vec2f {
			auto& world = Scene::get_active_scene()->get_world();
			vec2f g = world.GetGravity();
			Math::meters_to_pixels(g);
			return g;
			});

		lua.set_function("get_physics_position", [](Entity& entity) -> vec2f {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return vec2f(0, 0);
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return vec2f(0, 0);
			vec2f pos = body->GetPosition();
			Math::meters_to_pixels(pos);
			return pos;
			});

		lua.set_function("get_physics_angle", [](Entity& entity) -> float {
			if (!detail::has_component<PhysicsBody_Component>(entity)) return 0.0f;
			auto& body = entity.get_component<PhysicsBody_Component>().body;
			if (!body) return 0.0f;
			return body->GetAngle();
			});
	}
}