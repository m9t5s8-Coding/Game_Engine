#include <iostream>
#include <Project/GamePacker.hpp>

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
