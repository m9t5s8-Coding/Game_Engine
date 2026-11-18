#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef PLATFORM_ANDROID
  #include <Platform/Android/AndroidPlatform.hpp>
#endif

namespace ag
{

static constexpr uint32_t PAK_MAGIC   = 0x41455241;
static constexpr uint32_t PAK_VERSION = 1;

struct PackedEntry
{
  uint64_t offset;
  uint64_t size;
};

class GamePacker
{
public:
  struct PackOptions
  {
    bool verbose        = true;
    bool include_hidden = false;
  };

  struct PackResult
  {
    bool        success   = false;
    uint32_t    fileCount = 0;
    uint64_t    totalSize = 0;
    std::string error;
  };

  static PackResult pack(const std::string& project_root, const std::string& output_pak)
  {
    return pack(project_root, output_pak, PackOptions{});
  }

  static PackResult pack(const std::string&   project_root,
                         const std::string&   output_pak,
                         const PackOptions&   options)
  {
    namespace fs = std::filesystem;

    static_assert(sizeof(uint32_t) == 4, "PAK format requires 32-bit integers");
    static_assert(sizeof(uint64_t) == 8, "PAK format requires 64-bit integers");

    PackResult result;

    auto fail = [&result](const std::string& message) -> PackResult
    {
      result.success = false;
      result.error   = message;
      std::cerr << "[Packer] ERROR: " << message << std::endl;
      return result;
    };

    auto log = [&options](const std::string& message)
    {
      if (options.verbose)
        std::cout << message << std::endl;
    };

    // --- Resolve paths -----------------------------------------------------
    std::error_code ec;
    fs::path        root = fs::absolute(fs::path(project_root), ec);
    if (ec)
      return fail("Invalid input path '" + project_root + "': " + ec.message());

    if (!fs::is_directory(root, ec) || ec)
      return fail("Input folder does not exist or is not a directory: " + root.string());

    {
      std::error_code canon_ec;
      fs::path        canonical = fs::weakly_canonical(root, canon_ec);
      if (!canon_ec)
        root = std::move(canonical);
    }

    fs::path output = fs::absolute(fs::path(output_pak), ec);
    if (ec)
      return fail("Invalid output path '" + output_pak + "': " + ec.message());

    if (output.has_parent_path())
    {
      fs::create_directories(output.parent_path(), ec);
      if (ec)
        return fail("Cannot create output folder '" + output.parent_path().string() +
                    "': " + ec.message());
    }

    fs::path temp = output;
    temp += ".tmp";

    auto normalized = [](const fs::path& path)
    {
      std::error_code path_ec;
      fs::path        canonical = fs::weakly_canonical(path, path_ec);
      if (path_ec)
        canonical = fs::absolute(path, path_ec);
      return canonical.lexically_normal();
    };

    const fs::path output_norm = normalized(output);
    const fs::path temp_norm   = normalized(temp);

    auto discard_temp = [&temp]()
    {
      std::error_code remove_ec;
      fs::remove(temp, remove_ec);
    };

    // --- Collect files -----------------------------------------------------
    struct FileEntry
    {
      std::string rel_path;
      fs::path    path;
      uint64_t    size = 0;
    };

    std::vector<FileEntry> files;
    uint64_t               total_data_size = 0;

    log("[Packer] Scanning: " + root.string());

    try
    {
      for (auto it = fs::recursive_directory_iterator(root);
           it != fs::recursive_directory_iterator();
           ++it)
      {
        const fs::directory_entry& entry = *it;
        std::error_code            entry_ec;

        if (entry.is_directory(entry_ec))
        {
          // Do not descend into hidden folders (.git, .cache, ...).
          if (!options.include_hidden)
          {
            std::string name = entry.path().filename().string();
            if (!name.empty() && name.front() == '.')
              it.disable_recursion_pending();
          }
          continue;
        }

        if (!entry.is_regular_file(entry_ec))
        {
          if (entry_ec)
            std::cerr << "[Packer] WARNING: Cannot inspect '" << entry.path().string()
                      << "': " << entry_ec.message() << std::endl;
          continue;
        }

        // Never pack the pak we are writing (or a leftover temp file).
        const fs::path entry_norm = normalized(entry.path());
        if (entry_norm == output_norm || entry_norm == temp_norm)
          continue;

        std::string rel = fs::relative(entry.path(), root, entry_ec).generic_string();
        if (entry_ec || rel.empty())
        {
          std::cerr << "[Packer] WARNING: Skipping '" << entry.path().string()
                    << "': " << entry_ec.message() << std::endl;
          continue;
        }

        if (!options.include_hidden && has_hidden_component(rel))
          continue;

        uint64_t size = fs::file_size(entry.path(), entry_ec);
        if (entry_ec)
          return fail("Cannot read size of '" + entry.path().string() +
                      "': " + entry_ec.message());

        if (!checked_add(total_data_size, size, total_data_size))
          return fail("Input is too large for a .pak file");

        log("[Packer]   + " + rel + " (" + std::to_string(size) + " bytes)");
        files.push_back(FileEntry{std::move(rel), entry.path(), size});
      }
    }
    catch (const fs::filesystem_error& error)
    {
      return fail(std::string("Scanning failed: ") + error.what());
    }

    if (files.empty())
      return fail("No files found to pack in '" + root.string() + "'");

    // Deterministic pak layout: same input always produces the same bytes.
    std::sort(files.begin(), files.end(),
              [](const FileEntry& a, const FileEntry& b) { return a.rel_path < b.rel_path; });

    for (size_t i = 1; i < files.size(); ++i)
    {
      if (files[i].rel_path == files[i - 1].rel_path)
        return fail("Duplicate path in pak: '" + files[i].rel_path + "'");
    }

    if (files.size() > std::numeric_limits<uint32_t>::max())
      return fail("Too many files for a .pak (maximum is 4294967295)");

    // --- Build the header --------------------------------------------------
    uint64_t header_size = sizeof(PAK_MAGIC) + sizeof(PAK_VERSION) + sizeof(uint32_t) +
                           sizeof(uint64_t);

    for (const FileEntry& file : files)
    {
      if (file.rel_path.size() > std::numeric_limits<uint32_t>::max())
        return fail("Path is too long for a .pak: '" + file.rel_path + "'");

      uint64_t entry_size = sizeof(uint32_t) + file.rel_path.size() + sizeof(uint64_t) +
                            sizeof(uint64_t);
      if (!checked_add(header_size, entry_size, header_size))
        return fail("Pak header is too large");
    }

    const uint64_t data_offset = header_size;

    uint64_t expected_size = data_offset;
    if (!checked_add(expected_size, total_data_size, expected_size))
      return fail("Input is too large for a .pak file");

    // --- Write to a temp file, then atomically replace the output ----------
    std::ofstream out(temp, std::ios::binary | std::ios::trunc);
    if (!out.is_open())
      return fail("Cannot create output file: " + temp.string());

    auto fail_write = [&](const std::string& message) -> PackResult
    {
      out.close();
      discard_temp();
      return fail(message);
    };

    auto write_bytes = [&out](const void* data, uint64_t size) -> bool
    {
      out.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
      return static_cast<bool>(out);
    };

    const uint32_t entry_count = static_cast<uint32_t>(files.size());

    if (!write_bytes(&PAK_MAGIC, sizeof(PAK_MAGIC)) ||
        !write_bytes(&PAK_VERSION, sizeof(PAK_VERSION)) ||
        !write_bytes(&entry_count, sizeof(entry_count)) ||
        !write_bytes(&data_offset, sizeof(data_offset)))
      return fail_write("Failed to write pak header to '" + temp.string() + "'");

    uint64_t current_offset = 0;
    for (const FileEntry& file : files)
    {
      const uint32_t path_len = static_cast<uint32_t>(file.rel_path.size());
      const uint64_t size     = file.size;

      if (!write_bytes(&path_len, sizeof(path_len)) ||
          !write_bytes(file.rel_path.data(), path_len) ||
          !write_bytes(&current_offset, sizeof(current_offset)) ||
          !write_bytes(&size, sizeof(size)))
        return fail_write("Failed to write pak entry table to '" + temp.string() + "'");

      current_offset += size;
    }

    // Stream file contents instead of holding the whole pak in memory.
    std::vector<char> buffer(64 * 1024);

    for (const FileEntry& file : files)
    {
      std::ifstream in(file.path, std::ios::binary);
      if (!in.is_open())
        return fail_write("Cannot read '" + file.path.string() + "'");

      uint64_t remaining = file.size;
      while (remaining > 0)
      {
        const uint64_t chunk =
            std::min<uint64_t>(remaining, static_cast<uint64_t>(buffer.size()));
        in.read(buffer.data(), static_cast<std::streamsize>(chunk));
        if (static_cast<uint64_t>(in.gcount()) != chunk)
          return fail_write("File changed while packing: '" + file.path.string() + "'");

        if (!write_bytes(buffer.data(), chunk))
          return fail_write("Failed to write '" + file.path.string() + "' to the pak");

        remaining -= chunk;
      }
    }

    out.flush();
    if (!out)
      return fail_write("Failed to flush '" + temp.string() + "'");

    out.close();
    if (out.fail())
      return fail_write("Failed to close '" + temp.string() + "'");

    const uint64_t actual_size = fs::file_size(temp, ec);
    if (ec)
      return fail_write("Cannot verify '" + temp.string() + "': " + ec.message());
    if (actual_size != expected_size)
      return fail_write("Unexpected pak size (expected " + std::to_string(expected_size) +
                        " bytes, wrote " + std::to_string(actual_size) + " bytes)");

    // Rename over the destination. On Windows an existing destination cannot
    // always be replaced directly, so remove it and retry once.
    fs::rename(temp, output, ec);
    if (ec)
    {
      std::error_code remove_ec;
      fs::remove(output, remove_ec);

      ec.clear();
      fs::rename(temp, output, ec);
    }
    if (ec)
      return fail_write("Cannot replace '" + output.string() + "': " + ec.message());

    result.success   = true;
    result.fileCount = entry_count;
    result.totalSize = total_data_size;

    log("[Packer] Done! Packed " + std::to_string(entry_count) + " files (" +
        std::to_string(total_data_size) + " bytes) -> " + output.string());

    return result;
  }

private:
  static bool has_hidden_component(const std::string& relative_path)
  {
    size_t start = 0;
    while (start < relative_path.size())
    {
      size_t end = relative_path.find('/', start);
      if (end == std::string::npos)
        end = relative_path.size();

      if (end > start && relative_path[start] == '.')
        return true;

      start = end + 1;
    }
    return false;
  }

  static bool checked_add(uint64_t a, uint64_t b, uint64_t& out)
  {
    if (a > std::numeric_limits<uint64_t>::max() - b)
      return false;
    out = a + b;
    return true;
  }
};

class PakLoader
{
private:
  std::vector<uint8_t>                         m_data;    // ← holds entire pak in memory on Android
  std::ifstream                                m_stream;  // ← used on desktop only
  std::unordered_map<std::string, PackedEntry> m_index;
  uint64_t                                     m_data_offset = 0;
  bool                                         m_loaded      = false;

  // Simple memory reader helper for Android
  struct MemReader
  {
    const uint8_t* data;
    size_t         pos = 0;

    template <typename T>
    void read(T* out)
    {
      memcpy(out, data + pos, sizeof(T));
      pos += sizeof(T);
    }

    void read_bytes(void* out, size_t size)
    {
      memcpy(out, data + pos, size);
      pos += size;
    }
  };

public:
  bool load(const std::string& pak_path)
  {
#ifdef PLATFORM_ANDROID

    if (!g_asset_manager)
      return false;

    AAsset* asset = AAssetManager_open(g_asset_manager, pak_path.c_str(), AASSET_MODE_BUFFER);
    if (!asset)
      return false;

    size_t size = AAsset_getLength(asset);
    m_data.resize(size);
    AAsset_read(asset, m_data.data(), size);
    AAsset_close(asset);

    // Parse header from memory
    MemReader r{m_data.data()};
    uint32_t  magic, version, entryCount;
    r.read(&magic);
    r.read(&version);
    r.read(&entryCount);
    r.read(&m_data_offset);

    if (magic != PAK_MAGIC || version != PAK_VERSION)
    {
      m_data.clear();
      return false;
    }

    for (uint32_t i = 0; i < entryCount; i++)
    {
      uint32_t pathLen;
      r.read(&pathLen);
      std::string path(pathLen, '\0');
      r.read_bytes(&path[0], pathLen);

      PackedEntry entry;
      r.read(&entry.offset);
      r.read(&entry.size);
      m_index[path] = entry;
    }

    m_loaded = true;
    return true;

#else
    // Desktop — existing ifstream code unchanged
    m_stream.open(pak_path, std::ios::binary);
    if (!m_stream.is_open())
      return false;

    uint32_t magic, version, entryCount;
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
#endif
  }

  std::vector<uint8_t> read(const std::string& path)
  {
    auto it = m_index.find(path);
    if (it == m_index.end())
      return {};

    auto&                e = it->second;
    std::vector<uint8_t> buf(e.size);

#ifdef PLATFORM_ANDROID
    // Read directly from in-memory data
    memcpy(buf.data(), m_data.data() + m_data_offset + e.offset, e.size);
#else
    m_stream.seekg(m_data_offset + e.offset);
    m_stream.read((char*)buf.data(), e.size);
#endif

    return buf;
  }

  void close()
  {
#ifdef PLATFORM_ANDROID
    m_data.clear();
#else
    m_stream.close();
#endif
    m_index.clear();
    m_loaded = false;
  }

  // read_string, exists, is_loaded, list_files, ~PakLoader unchanged
  std::string read_string(const std::string& path)
  {
    auto data = read(path);
    return std::string(data.begin(), data.end());
  }
  bool exists(const std::string& path) const
  {
    return m_index.find(path) != m_index.end();
  }
  bool is_loaded() const
  {
    return m_loaded;
  }
  std::vector<std::string> list_files() const
  {
    std::vector<std::string> out;
    out.reserve(m_index.size());
    for (auto& [name, _] : m_index)
      out.push_back(name);
    return out;
  }
  ~PakLoader()
  {
    close();
  }
};
}  // namespace ag
