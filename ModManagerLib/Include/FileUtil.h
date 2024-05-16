#pragma once
#include <string>
#include <vector>

namespace FileUtil
{
	std::string ReadFile(std::string FilePath);

	std::vector<std::string> GetAllFilesInFolder(std::string FolderPath, std::string RelativePath = "");
}