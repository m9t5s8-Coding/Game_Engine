#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <iostream>

namespace ag
{
 
  static constexpr uint32_t PAK_MAGIC = 0x41455241;
  static constexpr uint32_t PAK_VERSION = 1;

  struct PackedEntry
  {
    uint64_t offset;
    uint64_t size;
  };

 
  class GamePacker
  {
  public:
    struct PackResult
    {
      bool    success = false;
      uint32_t fileCount = 0;
      uint64_t totalSize = 0;
    };

    static PackResult pack(const std::string& project_root, const std::string& output_pak)
    {
      namespace fs = std::filesystem;
      PackResult result;

      if (!fs::exists(project_root))
      {
        std::cerr << "[Packer] ERROR: Project root does not exist: " << project_root << std::endl;
        return result;
      }

      // Collect all files
      struct FileData
      {
        std::string         rel_path;
        std::vector<uint8_t> data;
      };

      std::vector<FileData> files;
      uint64_t total_data_size = 0;

      std::cout << "[Packer] Scanning: " << project_root << std::endl;

      for (auto& entry : fs::recursive_directory_iterator(project_root))
      {
        if (entry.is_directory()) continue;

        std::string rel = fs::relative(entry.path(), project_root).string();
        std::replace(rel.begin(), rel.end(), '\\', '/');

        // Skip hidden files and temp files
        if (rel.find("/.") != std::string::npos) continue;
        if (rel.find("\\.") != std::string::npos) continue;

        std::ifstream in(entry.path(), std::ios::binary | std::ios::ate);
        if (!in.is_open())
        {
          std::cerr << "[Packer] WARNING: Cannot read: " << entry.path() << std::endl;
          continue;
        }

        uint64_t size = in.tellg();
        in.seekg(0);

        FileData fd;
        fd.rel_path = rel;
        fd.data.resize(size);
        in.read((char*)fd.data.data(), size);
        in.close();

        total_data_size += size;
        std::cout << "[Packer]   + " << rel << " (" << size << " bytes)" << std::endl;
        files.push_back(std::move(fd));
      }

      if (files.empty())
      {
        std::cerr << "[Packer] ERROR: No files found to pack." << std::endl;
        return result;
      }

      // Calculate header size to know dataOffset
      uint64_t header_size = sizeof(PAK_MAGIC) + sizeof(PAK_VERSION) + sizeof(uint32_t) + sizeof(uint64_t);
      for (auto& f : files)
        header_size += sizeof(uint32_t) + f.rel_path.size() + sizeof(uint64_t) + sizeof(uint64_t);

      uint64_t data_offset = header_size;

      // Write .pak
      std::ofstream out(output_pak, std::ios::binary);
      if (!out.is_open())
      {
        std::cerr << "[Packer] ERROR: Cannot create output file: " << output_pak << std::endl;
        return result;
      }

      // Header
      uint32_t entryCount = (uint32_t)files.size();
      out.write((char*)&PAK_MAGIC, sizeof(PAK_MAGIC));
      out.write((char*)&PAK_VERSION, sizeof(PAK_VERSION));
      out.write((char*)&entryCount, sizeof(entryCount));
      out.write((char*)&data_offset, sizeof(data_offset));

      // Entry table
      uint64_t current_offset = 0;
      for (auto& f : files)
      {
        uint32_t pathLen = (uint32_t)f.rel_path.size();
        uint64_t size = (uint64_t)f.data.size();

        out.write((char*)&pathLen, sizeof(pathLen));
        out.write(f.rel_path.data(), pathLen);
        out.write((char*)&current_offset, sizeof(current_offset));
        out.write((char*)&size, sizeof(size));

        current_offset += size;
      }

      // File data
      for (auto& f : files)
        out.write((char*)f.data.data(), f.data.size());

      out.close();

      result.success = true;
      result.fileCount = entryCount;
      result.totalSize = total_data_size;

      std::cout << "[Packer] Done! Packed " << entryCount << " files ("
        << total_data_size << " bytes) -> " << output_pak << std::endl;

      return result;
    }
  };

 
  class PakLoader
  {
  private:
    std::ifstream                                m_stream;
    std::unordered_map<std::string, PackedEntry> m_index;
    uint64_t                                     m_data_offset = 0;
    bool                                         m_loaded = false;

  public:
    bool load(const std::string& pak_path)
    {
      m_stream.open(pak_path, std::ios::binary);
      if (!m_stream.is_open()) return false;

      uint32_t magic, version;
      uint32_t entryCount;

      m_stream.read((char*)&magic, sizeof(magic));
      m_stream.read((char*)&version, sizeof(version));
      m_stream.read((char*)&entryCount, sizeof(entryCount));
      m_stream.read((char*)&m_data_offset, sizeof(m_data_offset));

      if (magic != PAK_MAGIC || version != PAK_VERSION)
      {
        m_stream.close();
        return false;
      }

      for (uint32_t i = 0; i < entryCount; i++)
      {
        uint32_t pathLen;
        m_stream.read((char*)&pathLen, sizeof(pathLen));

        std::string path(pathLen, '\0');
        m_stream.read(&path[0], pathLen);

        PackedEntry entry;
        m_stream.read((char*)&entry.offset, sizeof(entry.offset));
        m_stream.read((char*)&entry.size, sizeof(entry.size));

        m_index[path] = entry;
      }

      m_loaded = true;
      return true;
    }

   
    std::vector<uint8_t> read(const std::string& path)
    {
      auto it = m_index.find(path);
      if (it == m_index.end()) return {};

      auto& e = it->second;
      std::vector<uint8_t> buf(e.size);
      m_stream.seekg(m_data_offset + e.offset);
      m_stream.read((char*)buf.data(), e.size);
      return buf;
    }

   
    std::string read_string(const std::string& path)
    {
      auto data = read(path);
      return std::string(data.begin(), data.end());
    }

    bool exists(const std::string& path) const
    {
      return m_index.find(path) != m_index.end();
    }

    bool is_loaded() const { return m_loaded; }

    std::vector<std::string> list_files() const
    {
      std::vector<std::string> out;
      out.reserve(m_index.size());
      for (auto& [name, _] : m_index)
        out.push_back(name);
      return out;
    }

    void close()
    {
      m_stream.close();
      m_index.clear();
      m_loaded = false;
    }

    ~PakLoader() { close(); }
  };
}