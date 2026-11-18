#include <Project/GamePacker.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace
{
void print_usage(const char* executable)
{
  std::cout
      << "Usage: " << executable << " [options] <input_folder> <output.pak>\n"
      << "\n"
      << "Packs every file under <input_folder> into a .pak archive.\n"
      << "Paths inside the pak are relative to <input_folder> and always use '/'.\n"
      << "\n"
      << "Options:\n"
      << "  -h, --help           Show this help and exit\n"
      << "  -q, --quiet          Only print warnings, errors and the final summary\n"
      << "  -v, --verbose        List every packed file (default)\n"
      << "      --include-hidden Include files and folders starting with '.'\n";
}
}  // namespace

int main(int argc, char** argv)
{
  bool                     verbose        = true;
  bool                     include_hidden = false;
  std::vector<std::string> positional;

  for (int i = 1; i < argc; ++i)
  {
    const std::string argument = argv[i];

    if (argument == "-h" || argument == "--help")
    {
      print_usage(argv[0]);
      return 0;
    }
    if (argument == "-q" || argument == "--quiet")
    {
      verbose = false;
      continue;
    }
    if (argument == "-v" || argument == "--verbose")
    {
      verbose = true;
      continue;
    }
    if (argument == "--include-hidden")
    {
      include_hidden = true;
      continue;
    }
    if (!argument.empty() && argument.front() == '-')
    {
      std::cerr << "[AssetPacker] ERROR: Unknown option '" << argument << "'\n";
      print_usage(argv[0]);
      return 2;
    }

    positional.push_back(argument);
  }

  if (positional.size() != 2)
  {
    std::cerr << "[AssetPacker] ERROR: Expected an input folder and an output pak\n\n";
    print_usage(argv[0]);
    return 2;
  }

  ag::GamePacker::PackOptions options;
  options.verbose        = verbose;
  options.include_hidden = include_hidden;

  const ag::GamePacker::PackResult result =
      ag::GamePacker::pack(positional[0], positional[1], options);

  if (!result.success)
  {
    std::cerr << "[AssetPacker] Failed to pack assets.\n";
    return 1;
  }

  std::cout << "[AssetPacker] Packed " << result.fileCount << " files (" << result.totalSize
            << " bytes) -> " << positional[1] << "\n";
  return 0;
}
