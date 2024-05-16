#pragma once
#include "NexusModsAPI.h"

struct ModInfo
{
	std::string Name;
	std::string Description;
	std::string ImagePath;
	int ModID = 0;
	bool Enabled = false;

	std::vector<std::string> Files;

	static ModInfo ParseFile(std::string FilePath);

	void WriteFile(std::string FilePath);

	static std::vector<ModInfo> GetAllInstalledMods();
};