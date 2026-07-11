#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
  struct PackResult
  {
    bool     success   = false;
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
      std::string          rel_path;
      std::vector<uint8_t> data;
    };

    std::vector<FileData> files;
    uint64_t              total_data_size = 0;

    std::cout << "[Packer] Scanning: " << project_root << std::endl;

    for (auto& entry : fs::recursive_directory_iterator(project_root))
    {
      if (entry.is_directory())
        continue;

      std::string rel = fs::relative(entry.path(), project_root).string();
      std::replace(rel.begin(), rel.end(), '\\', '/');

      // Skip hidden files and temp files
      if (rel.find("/.") != std::string::npos)
        continue;
      if (rel.find("\\.") != std::string::npos)
        continue;

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
    uint64_t header_size =
        sizeof(PAK_MAGIC) + sizeof(PAK_VERSION) + sizeof(uint32_t) + sizeof(uint64_t);
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
      uint64_t size    = (uint64_t)f.data.size();

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

    result.success   = true;
    result.fileCount = entryCount;
    result.totalSize = total_data_size;

    std::cout << "[Packer] Done! Packed " << entryCount << " files (" << total_data_size
              << " bytes) -> " << output_pak << std::endl;

    return result;
  }
};
}  // namespace ag

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: AssetPacker <assets_folder> <output.pak>\n";
    return 1;
  }

  std::string input  = argv[1];
  std::string output = argv[2];

  auto result = ag::GamePacker::pack(input, output);

  if (!result.success)
  {
    std::cerr << "[AssetPacker] Failed to pack assets.\n";
    return 1;
  }

  std::cout << "[AssetPacker] Packed " << result.fileCount << " files (" << result.totalSize
            << " bytes) -> " << output << "\n";
  return 0;
}
