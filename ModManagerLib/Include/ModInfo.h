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

	void CheckModUpdateStatus();

	static void(*InstallFOMODCallback)(ModInfo Info, std::string Path, std::string ToPath);

	void Enable();
	void Disable();
	void Remove();

	// Functions for profiles.
	// When a profile is switched, all mods of the old profile are unloaded
	// and all mods of the new profile are loaded.
	void Unload();
	void Load();

	std::string GetModFilesDir() const;

	static bool IsModDirectory(std::string Path);
	bool ContainsMultipleVersions() const;
	void InstallVersion(std::string VersionName);
	std::vector<std::string> GetVersions() const;
};
