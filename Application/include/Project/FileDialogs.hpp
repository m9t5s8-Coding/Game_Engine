#pragma once

namespace ag
{
	enum class FileType
	{
		Project_File, Scene_File, Folder
	};
	class FileDialogs
	{
	public:
		static std::string open_file(const char* filter);

		static std::string save_file(const char* filter);

		static std::string select_folder(const char* title = "Select Folder");
	};
}