#include "FileUtil.h"
#include <fstream>
#include <filesystem>
#include <iostream>

std::string FileUtil::ReadFile(std::string FilePath)
{
	std::ifstream InStream = std::ifstream(FilePath);
	std::string ContentString((std::istreambuf_iterator<char>(InStream)),
		(std::istreambuf_iterator<char>()));
	InStream.close();
	return ContentString;
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
