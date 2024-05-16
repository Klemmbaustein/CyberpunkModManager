#include "ModInfo.h"
#include "FileUtil.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

using namespace nlohmann;

ModInfo ModInfo::ParseFile(std::string FilePath)
{
	json FileJson = json::parse(FileUtil::ReadFile(FilePath));

	return ModInfo{
		.Name = FileJson.at("name"),
		.Description = FileJson.at("description"),
		.ImagePath = FileJson.at("imagePath"),
		.ModID = FileJson.at("modId"),
		.Files = FileJson.at("files"),
	};
}

void ModInfo::WriteFile(std::string FilePath)
{
	std::ofstream Out = std::ofstream(FilePath);

	Out << json{
		{ "name", Name },
		{ "description", Description },
		{ "imagePath", ImagePath },
		{ "modId", ModID },
		{ "files", Files }
	};
	Out.close();
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
