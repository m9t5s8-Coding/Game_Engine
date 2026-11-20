#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_network() {
  auto& lua         = ScriptManager::get_lua();
  auto  packet_type = lua.new_usertype<Packet>("Packet", sol::constructors<Packet(uint8_t)>());

  packet_type["write_byte"]   = &Packet::write_byte;
  packet_type["write_int"]    = &Packet::write_int;
  packet_type["write_uint"]   = &Packet::write_uint;
  packet_type["write_float"]  = &Packet::write_float;
  packet_type["write_bool"]   = &Packet::write_bool;
  packet_type["write_string"] = &Packet::write_string;

  packet_type["read_byte"]   = &Packet::read_byte;
  packet_type["read_int"]    = &Packet::read_int;
  packet_type["read_uint"]   = &Packet::read_uint;
  packet_type["read_float"]  = &Packet::read_float;
  packet_type["read_bool"]   = &Packet::read_bool;
  packet_type["read_string"] = &Packet::read_string;

  packet_type["type"]     = &Packet::m_type;
  packet_type["has_data"] = &Packet::has_data;
  packet_type["size"]     = &Packet::size;
  packet_type["reset"]    = &Packet::reset;

  sol::table net = lua.create_named_table("Network");

#ifndef AERO_SERVER
  net.set_function("send", [](Packet& packet) {
    auto& client = NetworkManager::get_client();
    client.send(packet);
  });

  net.set_function("is_connected",
                   []() -> bool { return NetworkManager::get_client().is_connected(); });

#else
  net.set_function("send_to", [](int client_ID, Packet& packet) {
    NetworkManager::get_server().send_to(client_ID, packet);
  });

  net.set_function("boardcast",
                   [](Packet& packet) { NetworkManager::get_server().boardcast(packet); });

  net.set_function("disconnect", [](int client_id) {
    NetworkManager::get_server().disconnect_client(client_id);
  });
#endif
}

}  // namespace ag
