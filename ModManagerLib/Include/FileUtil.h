#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace FileUtil
{
	std::string ReadFile(std::string FilePath);

	std::vector<std::string> GetAllFilesInFolder(std::string FolderPath, std::string RelativePath = "");

	size_t GetPathSize(std::filesystem::path TargetPath);
}