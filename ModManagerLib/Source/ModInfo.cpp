#include "ModInfo.h"
#include "FileUtil.h"
#include "nlohmann/json.hpp"
#include "Game.h"

#include <fstream>
#include <iostream>
#include "../../App/Source/WindowsFunctions.h"

using namespace nlohmann;

ModInfo ModInfo::ParseFile(std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		std::cout << "file does not exist" << std::endl;
		return ModInfo();
	}
	try
	{
		json FileJson = json::parse(FileUtil::ReadFile(FilePath));

		return ModInfo{
			.Name = FileJson.at("name"),
			.Description = FileJson.at("description"),
			.ImagePath = FileJson.at("imagePath"),
			.ModID = FileJson.at("modId"),
			.Enabled = FileJson.at("enabled"),
			.Files = FileJson.at("files"),
		};
	}
	catch (json::exception)
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
		{ "imagePath", ImagePath },
		{ "modId", ModID },
		{ "files", Files },
		{ "enabled", Enabled },
	};
	Out.close();
}
ModInfo ModInfo::GetModByName(std::string Name)
{
	return ParseFile("app/profiles/test/" + Name + ".json");
}

std::vector<ModInfo> ModInfo::GetAllInstalledMods()
{
	std::vector<ModInfo> FoundMods;
	std::filesystem::create_directories("app/profiles/test/");

	for (auto& i : std::filesystem::directory_iterator("app/profiles/test/"))
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
	WriteFile("app/profiles/test/" + Name + ".json");
}

void ModInfo::Enable()
{
	if (!std::filesystem::exists(Game::GetGameLocation()))
	{
		return;
	}

	try
	{
		std::string FromDir = "app/profiles/test/mod_files/" + Name + "/";

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

	std::filesystem::remove_all("app/profiles/test/mod_files/" + Name + "/");
	std::filesystem::remove("app/profiles/test/" + Name + ".json");
}