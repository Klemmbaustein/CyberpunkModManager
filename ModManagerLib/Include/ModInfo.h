#pragma once
#include "NexusModsAPI.h"

struct ModInfo
{
	std::string Name;
	std::string Description;
	std::string ImagePath;
	std::string FileCategory;
	int ModID = 0;
	int FileID = 0;
	bool Enabled = false;

	std::vector<std::string> Files;

	bool RequiresUpdate = false;
	static ModInfo ParseFile(std::string FilePath);

	void WriteFile(std::string FilePath);
	void Save();

	static std::vector<ModInfo> GetAllInstalledMods();
	static ModInfo GetModByName(std::string Name);

	void Enable();
	void Disable();
	void Remove();
};