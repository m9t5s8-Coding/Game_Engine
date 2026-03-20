#include <Networking/Packet.hpp>
#include <stdexcept>

namespace ag
{

void Packet::write_byte(uint8_t v)
{
  m_data.push_back(v);
}

void Packet::write_int(int32_t v)
{
  m_data.push_back((v) & 0xFF);
  m_data.push_back((v >> 8) & 0xFF);
  m_data.push_back((v >> 16) & 0xFF);
  m_data.push_back((v >> 24) & 0xFF);
}

void Packet::write_uint(uint32_t v)
{
  m_data.push_back((v) & 0xFF);
  m_data.push_back((v >> 8) & 0xFF);
  m_data.push_back((v >> 16) & 0xFF);
  m_data.push_back((v >> 24) & 0xFF);
}

void Packet::write_float(float v)
{
  uint32_t temp;
  memcpy(&temp, &v, sizeof(float));
  write_uint(temp);
}

void Packet::write_bool(bool v)
{
  write_byte(v ? 1 : 0);
}

void Packet::write_string(const std::string& v)
{
  write_uint((uint32_t)v.size());
  for (char c : v)
    m_data.push_back((uint8_t)c);
}

uint8_t Packet::read_byte()
{
  if (m_read_pos >= m_data.size())
    return 0;
  return m_data[m_read_pos++];
}

int32_t Packet::read_int()
{
  if (m_read_pos + 4 > m_data.size())
    return 0;
  int32_t v = 0;
  v |= (int32_t)m_data[m_read_pos++];
  v |= (int32_t)m_data[m_read_pos++] << 8;
  v |= (int32_t)m_data[m_read_pos++] << 16;
  v |= (int32_t)m_data[m_read_pos++] << 24;
  return v;
}

uint32_t Packet::read_uint()
{
  if (m_read_pos + 4 > m_data.size())
    return 0;
  uint32_t v = 0;
  v |= (int32_t)m_data[m_read_pos++];
  v |= (int32_t)m_data[m_read_pos++] << 8;
  v |= (int32_t)m_data[m_read_pos++] << 16;
  v |= (int32_t)m_data[m_read_pos++] << 24;
  return v;
}

float Packet::read_float()
{
  uint32_t tmp = read_uint();
  float    v;
  memcpy(&v, &tmp, sizeof(float));
  return v;
}

bool Packet::read_bool()
{
  return read_byte() != 0;
}

std::string Packet::read_string()
{
  uint32_t len = read_uint();
  if (m_read_pos + len > m_data.size())
    return "";
  std::string s((char*)&m_data[m_read_pos], len);
  m_read_pos += len;
  return s;
}

std::vector<uint8_t> Packet::serialize() const
{
  std::vector<uint8_t> data;
  uint32_t             data_size = m_data.size();

  data.push_back((data_size >> 24) & 0xFF);
  data.push_back((data_size >> 16) & 0xFF);
  data.push_back((data_size >> 8) & 0xFF);
  data.push_back((data_size) & 0xFF);

  data.push_back((uint8_t)m_type);

  data.insert(data.end(), m_data.begin(), m_data.end());

  return data;
}
bool Packet::deserialize(const uint8_t* data, int size, Packet& out, uint8_t type)
{
  out.m_type = (PacketType)type;
  out.m_data.assign(data, data + size);
  out.m_read_pos = 0;

  return true;
}

}  // namespace ag
