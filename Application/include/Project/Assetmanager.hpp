#pragma once
#include <fstream>
#include <memory>
#include <Project/GamePacker.hpp>
#include <string>
#include <vector>

#ifdef PLATFORM_ANDROID
  #include <Platform/Android/AndroidPlatform.hpp>
#endif

namespace ag
{
class AssetManager
{
public:
  enum class Domain
  {
    Engine,
    Project
  };

private:
  static AssetManager& instance()
  {
    static AssetManager s_instance;
    return s_instance;
  }

  struct PakMount
  {
    std::unique_ptr<PakLoader> pak;
    std::string                base_path;
    bool                       using_pak = false;
  };

  PakMount m_engine;
  PakMount m_project;

  static PakMount& mount(Domain d)
  {
    auto& mgr = instance();
    return d == Domain::Engine ? mgr.m_engine : mgr.m_project;
  }

  static void init_mount(PakMount&          m,
                         const std::string& pak_path,
                         const std::string& fallback_base_path,
                         const char*        label)
  {
    m.pak = std::make_unique<PakLoader>();
    if (m.pak->load(pak_path))
    {
      m.using_pak = true;
      AERO_CORE_INFO("[AssetManager] {} running in Packed Mode", label);
    }
    else
    {
      m.pak.reset();
      m.using_pak = false;
      m.base_path = fallback_base_path;
      AERO_CORE_INFO("[AssetManager] {} running in Development Mode", label);
    }
  }

public:
  static void init_engine(const std::string& pak_path, const std::string& fallback_base_path)
  {
    init_mount(instance().m_engine, pak_path, fallback_base_path, "Engine");
  }

  static void init_project(const std::string& pak_path, const std::string& fallback_base_path)
  {
    init_mount(instance().m_project, pak_path, fallback_base_path, "Project");
  }

  static void set_fallback_path(const std::string& fallback_path, Domain d)
  {
    mount(d).base_path = fallback_path;
  }

  static bool is_packed(Domain d)
  {
    return mount(d).using_pak;
  }

  // ── The only function that needs Android support ──────────────────────────
  static std::vector<uint8_t> read_bytes(const std::string& path, Domain d)
  {
    auto& m = mount(d);
    if (m.using_pak)
      return m.pak->read(path);

    std::string   full = m.base_path + "/" + path;
    std::ifstream f(full, std::ios::binary | std::ios::ate);
    if (!f.is_open())
    {
      AERO_CORE_ERROR("[AssetManager] Failed to open: {}", full);
      return {};
    }
    size_t size = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> buf(size);
    f.read(reinterpret_cast<char*>(buf.data()), size);
    return buf;
  }
  // ─────────────────────────────────────────────────────────────────────────

  static std::string read_string(const std::string& path, Domain d)
  {
    auto data = read_bytes(path, d);
    return std::string(data.begin(), data.end());
  }

  static json read_json(const std::string& path, Domain d)
  {
    std::string content = read_string(path, d);
    if (content.empty())
      return {};
    return json::parse(content, nullptr, false);
  }

  static bool exists(const std::string& path, Domain d)
  {
    auto& m = mount(d);
    if (m.using_pak)
      return m.pak->exists(path);
    std::ifstream f(m.base_path + "/" + path);
    return f.good();
  }

  static std::vector<std::string> list_files(Domain d)
  {
    auto& m = mount(d);
    if (m.using_pak)
      return m.pak->list_files();
    return {};
  }

  static std::vector<uint8_t> read_bytes_any(const std::string& path)
  {
    if (exists(path, Domain::Project))
      return read_bytes(path, Domain::Project);
    return read_bytes(path, Domain::Engine);
  }
};
}  // namespace ag
