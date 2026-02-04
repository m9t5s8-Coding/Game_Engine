#pragma once
#include <Project/GamePacker.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <memory>


namespace ag
{
  class AssetManager
  {
  private:
    static AssetManager& instance()
    {
      static AssetManager s_instance;
      return s_instance;
    }

    std::unique_ptr<PakLoader> m_pak;
    std::string                m_base_path;
    bool                       m_using_pak = false;

  public:
    static void init(const std::string& pak_path, const std::string& fallback_base_path)
    {
      auto& mgr = instance();

      mgr.m_pak = std::make_unique<PakLoader>();
      if (mgr.m_pak->load(pak_path))
      {
        mgr.m_using_pak = true;
        AERO_CORE_INFO("[AssetManager] Running in Packed Mode");
      }
      else
      {
        mgr.m_pak.reset();
        mgr.m_using_pak = false;
        mgr.m_base_path = fallback_base_path;
        AERO_CORE_INFO("[AssetManager] Running in Development Mode");
      }
    }

    static bool is_packed() { return instance().m_using_pak; }

   
    static std::vector<uint8_t> read_bytes(const std::string& path)
    {
      auto& mgr = instance();

      if (mgr.m_using_pak)
      {
        return mgr.m_pak->read(path);
      }

      std::ifstream f(path, std::ios::binary | std::ios::ate);
      if (!f.is_open()) return {};

      size_t size = f.tellg();
      f.seekg(0);
      std::vector<uint8_t> buf(size);
      f.read((char*)buf.data(), size);
      return buf;
    }

   
    static std::string read_string(const std::string& path)
    {
      auto data = read_bytes(path);
      return std::string(data.begin(), data.end());
    }

  
    static json read_json(const std::string& path)
    {
      std::string content = read_string(path);
      if (content.empty()) return {};
      return json::parse(content);
    }

   
    static bool exists(const std::string& path)
    {
      auto& mgr = instance();

      if (mgr.m_using_pak)
        return mgr.m_pak->exists(path);

      std::string full = mgr.m_base_path + "/" + path;
      std::ifstream f(full);
      return f.good();
    }

  
    static std::vector<std::string> list_files()
    {
      auto& mgr = instance();
      if (mgr.m_using_pak)
        return mgr.m_pak->list_files();
      return {};
    }
  };
}