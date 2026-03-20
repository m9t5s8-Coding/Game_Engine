#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ag
{
enum class PacketType : uint8_t
{
  Heartbeat  = 0,
  Disconnect = 1,
  Connected  = 2,
  Error      = 3,

  Game_0 = 10,
  Game_1 = 11,
  Game_2 = 12,
  Game_3 = 13,
  Game_4 = 14,
  Game_5 = 15,
};

struct Packet
{
  Packet() = default;
  Packet(PacketType type)
    : m_type(type)
  {
  }
  Packet(uint8_t type)
    : m_type((PacketType)type)
  {
  }

  void write_byte(uint8_t v);
  void write_int(int32_t v);
  void write_uint(uint32_t v);
  void write_float(float v);
  void write_bool(bool v);
  void write_string(const std::string& v);

  uint8_t     read_byte();
  int32_t     read_int();
  uint32_t    read_uint();
  float       read_float();
  bool        read_bool();
  std::string read_string();

  PacketType m_type = PacketType::Heartbeat;

  std::vector<uint8_t> serialize() const;
  static bool          deserialize(const uint8_t* data, int size, Packet& out, uint8_t type);

  void reset()
  {
    m_data.clear();
    m_read_pos = 0;
  }
  bool empty() const
  {
    return m_data.empty();
  }
  size_t size() const
  {
    return m_data.size();
  }
  bool has_data() const
  {
    return m_read_pos < m_data.size();
  }

private:
  std::vector<uint8_t> m_data;
  size_t               m_read_pos = 0;
};
}  // namespace ag
