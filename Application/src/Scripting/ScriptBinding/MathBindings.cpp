#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_color() {
  auto& lua = ScriptManager::get_lua();

  lua.new_usertype<Color>("Color",
                          sol::constructors<Color(),
                                            Color(uint8_t, uint8_t, uint8_t),
                                            Color(uint8_t, uint8_t, uint8_t, uint8_t)>(),
                          "r",
                          &Color::r,
                          "g",
                          &Color::g,
                          "b",
                          &Color::b,
                          "a",
                          &Color::a,
                          sol::meta_function::equal_to,
                          &Color::operator==,
                          sol::meta_function::to_string,
                          [](const Color& c) {
                            return "Color(" + std::to_string(c.r) + ", " + std::to_string(c.g) +
                                   ", " + std::to_string(c.b) + ", " + std::to_string(c.a) + ")";
                          });

  auto color_table           = lua["Color"];
  color_table["Transparent"] = Color::Transparent;
  color_table["White"]       = Color::White;
  color_table["Black"]       = Color::Black;
  color_table["Red"]         = Color::Red;
  color_table["Green"]       = Color::Green;
  color_table["Blue"]        = Color::Blue;
  color_table["Yellow"]      = Color::Yellow;
  color_table["Cyan"]        = Color::Cyan;
  color_table["Magenta"]     = Color::Magenta;
  color_table["Orange"]      = Color::Orange;
  color_table["Purple"]      = Color::Purple;
  color_table["Pink"]        = Color::Pink;
  color_table["Brown"]       = Color::Brown;
  color_table["Gray"]        = Color::Gray;
  color_table["LightGray"]   = Color::LightGray;
  color_table["DarkGray"]    = Color::DarkGray;
  color_table["Lime"]        = Color::Lime;
  color_table["Navy"]        = Color::Navy;
  color_table["Teal"]        = Color::Teal;
  color_table["Olive"]       = Color::Olive;
  color_table["Maroon"]      = Color::Maroon;
}

void ScriptBinding::register_vec2() {
  auto& lua = ScriptManager::get_lua();

  // vec2f (float vector)
  lua.new_usertype<vec2f>(
      "vec2f",
      sol::call_constructor,
      sol::factories([](float x, float y) { return vec2f(x, y); }),
      "x",
      &vec2f::x,
      "y",
      &vec2f::y,

      sol::meta_function::addition,
      sol::overload(sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator+),
                    sol::resolve<vec2f(const float) const>(&vec2f::operator+)),
      sol::meta_function::subtraction,
      sol::overload(sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator-),
                    sol::resolve<vec2f(const float) const>(&vec2f::operator-)),
      sol::meta_function::multiplication,
      sol::overload(sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator*),
                    sol::resolve<vec2f(const float) const>(&vec2f::operator*)),
      sol::meta_function::division,
      sol::overload(sol::resolve<vec2f(const vec2f&) const>(&vec2f::operator/),
                    sol::resolve<vec2f(const float) const>(&vec2f::operator/)),

      sol::meta_function::equal_to,
      sol::resolve<bool(const vec2f&) const>(&vec2f::operator==),
      sol::meta_function::to_string,
      [](const vec2f& v) {
        return "vec2f(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
      },

      "length",
      [](const vec2f& v) { return std::sqrt(v.x * v.x + v.y * v.y); },
      "normalized",
      [](const vec2f& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        return len > 0 ? vec2f(v.x / len, v.y / len) : vec2f(0, 0);
      },
      "dot",
      [](const vec2f& v1, const vec2f& v2) { return v1.x * v2.x + v1.y * v2.y; });

  // vec2i (integer vector)
  lua.new_usertype<vec2i>("vec2i",
                          sol::call_constructor,
                          sol::factories([](int x, int y) { return vec2i(x, y); }),
                          "x",
                          &vec2i::x,
                          "y",
                          &vec2i::y,

                          sol::meta_function::addition,
                          sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator+),
                          sol::meta_function::subtraction,
                          sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator-),
                          sol::meta_function::multiplication,
                          sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator*),
                          sol::meta_function::division,
                          sol::resolve<vec2i(const vec2i&) const>(&vec2i::operator/),
                          sol::meta_function::equal_to,
                          sol::resolve<bool(const vec2i&) const>(&vec2i::operator==),
                          sol::meta_function::to_string,
                          [](const vec2i& v) {
                            return "vec2i(" + std::to_string(v.x) + ", " + std::to_string(v.y) +
                                   ")";
                          });

  // vec2u (unsigned integer vector)
  lua.new_usertype<vec2u>("vec2u",
                          sol::call_constructor,
                          sol::factories([](AG_uint x, AG_uint y) { return vec2u(x, y); }),
                          "x",
                          &vec2u::x,
                          "y",
                          &vec2u::y,

                          sol::meta_function::addition,
                          sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator+),
                          sol::meta_function::subtraction,
                          sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator-),
                          sol::meta_function::multiplication,
                          sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator*),
                          sol::meta_function::division,
                          sol::resolve<vec2u(const vec2u&) const>(&vec2u::operator/),
                          sol::meta_function::equal_to,
                          sol::resolve<bool(const vec2u&) const>(&vec2u::operator==),
                          sol::meta_function::to_string,
                          [](const vec2u& v) {
                            return "vec2u(" + std::to_string(v.x) + ", " + std::to_string(v.y) +
                                   ")";
                          });
}

}  // namespace ag
