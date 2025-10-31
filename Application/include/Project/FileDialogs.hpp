#pragma once

namespace ag
{
	class FileDialogs
	{
	public:
		static std::string open_file(const char* filter);

		static std::string save_file(const char* filter);

		static std::string select_folder(const char* title = "Select Folder");
	};
}