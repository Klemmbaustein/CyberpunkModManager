#include "ModInfo.h"
#include "FileUtil.h"
#include "nlohmann/json.hpp"
#include "Game.h"
#include "Profile.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include "../../App/Source/WindowsFunctions.h"

using namespace nlohmann;

std::map<std::string, bool> ModUpdateStatuses;
void(*ModInfo::InstallFOMODCallback)(ModInfo Info, std::string Path, std::string ToPath);

ModInfo ModInfo::ParseFile(std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		return ModInfo();
	}
	try
	{
		json FileJson = json::parse(FileUtil::ReadFile(FilePath));

		ModInfo Info = ModInfo{
			.Name = FileJson.at("name"),
			.Description = FileJson.at("description"),
			.ModID = FileJson.at("modId"),
			.Enabled = FileJson.at("enabled"),
			.Files = FileJson.at("files"),
		};

		if (FileJson.contains("fileId"))
		{
			Info.FileID = FileJson.at("fileId");
		}
		if (FileJson.contains("category"))
		{
			Info.FileCategory = FileJson.at("category");
		}
		std::string UpdateName = Profile::Current.Path + ":" + Info.Name;
		if (ModUpdateStatuses.contains(UpdateName))
		{
			Info.RequiresUpdate = ModUpdateStatuses.at(UpdateName);
		}

		return Info;
	}
	catch (json::exception e)
	{
		return ModInfo();
	}
}

void ModInfo::WriteFile(std::string FilePath)
{
	std::ofstream Out = std::ofstream(FilePath);

	Out << json{
		{ "name", Name },
		{ "description", Description },
		{ "modId", ModID },
		{ "fileId", FileID },
		{ "category", FileCategory },
		{ "files", Files },
		{ "enabled", Enabled },
	}.dump(2);
	Out.close();
}
ModInfo ModInfo::GetModByName(std::string Name)
{
	return ParseFile(Profile::Current.Path + Name + ".json");
}

void ModInfo::CheckModUpdateStatus()
{
	if (ModID && Enabled)
	{
		if (FileID == 0)
		{
			RequiresUpdate = true;
		}
		else
		{
			auto ModFiles = NxmAPI::ModInfo{
				.ModID = ModID
			}.GetFiles(FileCategory);
			if ((ModFiles.size() && ModFiles[0].FileID != FileID))
			{
				RequiresUpdate = true;
			}
		}
	}

	std::string UpdateName = Profile::Current.Path + ":" + Name;

	if (ModUpdateStatuses.contains(UpdateName))
	{
		ModUpdateStatuses.at(UpdateName) = RequiresUpdate;
	}
	else
	{
		ModUpdateStatuses.insert(std::pair(UpdateName, RequiresUpdate));
	}
}

std::vector<ModInfo> ModInfo::GetAllInstalledMods()
{
	std::vector<ModInfo> FoundMods;
	std::filesystem::create_directories(Profile::Current.Path);

	for (auto& i : std::filesystem::directory_iterator(Profile::Current.Path))
	{
		if (std::filesystem::is_directory(i))
		{
			continue;
		}

		FoundMods.push_back(ParseFile(i.path().string()));
	}

	return FoundMods;
}

void ModInfo::Save()
{
	WriteFile(Profile::Current.Path + Name + ".json");
}

void ModInfo::Enable()
{
	if (!std::filesystem::exists(Game::GetGameLocation()))
	{
		return;
	}

	try
	{
		std::string FromDir = GetModFilesDir();

		if (std::filesystem::exists(FromDir + "fomod") && InstallFOMODCallback)
		{
			InstallFOMODCallback(*this, FromDir, "app/temp/fomod_" + Name);
			return;
		}

		for (auto& i : Files)
		{
			std::string FilePath = Game::GetGameLocation() + i;
			std::filesystem::create_directories(FilePath.substr(0, FilePath.find_last_of("/")));
			std::filesystem::copy(FromDir + i, FilePath, std::filesystem::copy_options::update_existing);
		}
		Enabled = true;
		Save();
	}
	catch (std::exception e)
	{
		Windows::ErrorBox(e.what());
	}
}

void ModInfo::Unload()
{
	if (!Enabled)
	{
		return;
	}
	std::string GameLocation = Game::GetGameLocation();

	if (!std::filesystem::exists(GameLocation))
	{
		return;
	}

	for (auto& i : Files)
	{
		std::string Path = i.substr(0, i.find_last_of("/\\"));
		std::filesystem::create_directories(Profile::Current.Path + "unloaded/" + Path);

		if (!std::filesystem::exists(GameLocation + i))
		{
			std::cout << "Mod file doesn't exist: '" + GameLocation + i << "'" << std::endl;
			continue;
		}

		std::filesystem::copy(GameLocation + i, Profile::Current.Path + "unloaded/" + i,
				std::filesystem::copy_options::overwrite_existing);
		std::filesystem::remove(GameLocation + i);
	}
}

void ModInfo::Load()
{
	if (!Enabled)
	{
		return;
	}
	std::string GameLocation = Game::GetGameLocation();

	if (!std::filesystem::exists(GameLocation))
	{
		return;
	}

	auto UnloadedFiles = FileUtil::GetAllFilesInFolder(Profile::Current.Path + "unloaded/");

	for (auto& i : UnloadedFiles)
	{
		std::string Path = i.substr(0, i.find_last_of("/\\"));
		std::filesystem::create_directories(GameLocation + Path);
		std::filesystem::copy(Profile::Current.Path + "unloaded/" + i, GameLocation + i,
				std::filesystem::copy_options::overwrite_existing);
	}
}

void ModInfo::Disable()
{
	if (!Enabled)
	{
		return;
	}

	std::string GameLocation = Game::GetGameLocation();

	if (!std::filesystem::exists(GameLocation))
	{
		return;
	}

	for (auto& i : Files)
	{
		std::filesystem::remove(GameLocation + i);
	}

	std::vector<std::filesystem::path> EmptyPaths;
	// Remove any empty directories left over when deleting the files
	for (auto& i : std::filesystem::recursive_directory_iterator(GameLocation))
	{
		if (std::filesystem::is_directory(i) && std::filesystem::is_empty(i))
		{
			EmptyPaths.push_back(i);
		}
	}

	for (auto& i : EmptyPaths)
	{
		if (std::filesystem::exists(i))
		{
			std::filesystem::remove_all(i);
		}
	}

	Enabled = false;
	Save();
}

void ModInfo::Remove()
{
	Disable();

	std::filesystem::remove_all(Profile::Current.Path + "/mod_files/" + Name + "/");
	std::filesystem::remove(Profile::Current.Path + Name + ".json");
}

std::string ModInfo::GetModFilesDir() const
{
	return Profile::Current.Path +  "/mod_files/" + Name + "/";
}

static std::vector<std::string> KnownModDirNames =
{
	"archive",
	"red4ext",
	"r6",
	"bin",
	"engine",
	"fomod",
};

bool ModInfo::IsModDirectory(std::string Path)
{
	for (const auto& DirName : KnownModDirNames)
	{
		if (std::filesystem::exists(Path + "/" + DirName))
		{
			return true;
		}
	}
	return false;
}

bool ModInfo::ContainsMultipleVersions() const
{
	return !GetVersions().empty();
}

void ModInfo::InstallVersion(std::string VersionName)
{
	std::string FromDir = GetModFilesDir();

	Files = FileUtil::GetAllFilesInFolder(FromDir + VersionName);
	for (auto& i : Files)
	{
		std::string FilePath = Game::GetGameLocation() + i;
		std::filesystem::create_directories(FilePath.substr(0, FilePath.find_last_of("/")));
		std::filesystem::copy(FromDir + VersionName + "/" + i, FilePath, std::filesystem::copy_options::update_existing);
	}
	Enabled = true;
	Save();
}

std::vector<std::string> ModInfo::GetVersions() const
{
	std::string ModDir = GetModFilesDir();

	if (IsModDirectory(ModDir))
	{
		return {};
	}
	std::vector<std::string> FoundVersions;

	for (auto& i : std::filesystem::directory_iterator(ModDir))
	{
		if (IsModDirectory(i.path().string()))
		{
			FoundVersions.push_back(i.path().filename().string());
		}
	}

	return FoundVersions;
}
