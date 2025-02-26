#include "FileUtil.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

std::string FileUtil::ReadFile(std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		return "";
	}
	std::ifstream InStream = std::ifstream(FilePath);
	std::stringstream stream;
	stream << InStream.rdbuf();
	InStream.close();
	return stream.str();
}

std::vector<std::string> FileUtil::GetAllFilesInFolder(std::string FolderPath, std::string RelativePath)
{
	std::vector<std::string> ReturnValue;

	if (!std::filesystem::exists(FolderPath))
	{
		std::cout << "Attempted to get all files in '" << FolderPath << "' but it does not exist." << std::endl;
		return {};
	}

	for (auto& i : std::filesystem::directory_iterator(FolderPath))
	{
		if (std::filesystem::is_directory(i))
		{
			std::string NameString = i.path().filename().string();
			auto DirectoryContent = GetAllFilesInFolder(FolderPath + "/" + NameString, RelativePath + "/" + NameString);

			for (const auto& File : DirectoryContent)
			{
				ReturnValue.push_back(File);
			}
		}
		else
		{
			ReturnValue.push_back(RelativePath + "/" + i.path().filename().string());
		}
	}
	return ReturnValue;
}

size_t FileUtil::GetPathSize(std::filesystem::path TargetPath)
{
	size_t Size = 0;
	if (!std::filesystem::exists(TargetPath))
	{
		return 0;
	}
	for (const auto& File : std::filesystem::recursive_directory_iterator(TargetPath))
	{
		if (std::filesystem::is_regular_file(File))
		{
			Size += std::filesystem::file_size(File);
		}
	}
	return Size;
}
