#include "NexusModsAPI.h"
#include "Net.h"
#include "FileUtil.h"

#include <nlohmann/json.hpp>
#include <iostream>
using namespace nlohmann;

namespace NexusModsAPI
{
	std::string SourceUrl = "https://api.nexusmods.com/v1";
	std::string GameDomainName = "cyberpunk2077";
}

std::vector<NexusModsAPI::ModInfo> NexusModsAPI::GetMods()
{
	json ResponseJson = json::parse(Net::Get(SourceUrl + "/games/" + GameDomainName + "/mods/latest_updated.json"));

	// Response should be an array of mods.
	if (!ResponseJson.is_array())
	{
		std::cout << ResponseJson.dump() << std::endl;
		return {};
	}

	try
	{
		std::vector<ModInfo> FoundMods;
		for (const json& i : ResponseJson)
		{
			// Skip the mod if it isn't available.
			// For some reason the API lists unavailable mods but doesn't give much info for them.
			if (!i.contains("available") || !i.at("available").get<bool>())
			{
				continue;
			}

			if (!i.contains("name"))
			{
				std::cout << i.dump() << std::endl;
				continue;
			}

			ModInfo NewMod = ModInfo{
				.Name = i.at("name"),
				.ModID = i.at("mod_id"),
			};

			FoundMods.push_back(NewMod);
		}
		return FoundMods;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return {};
}

std::string NexusModsAPI::GetAPIKey()
{
	static const std::string KeyJsonFile = "app/saved/apikey.json";

	if (!std::filesystem::exists(KeyJsonFile))
	{
		return "";
	}

	json FileJson = json::parse(FileUtil::ReadFile(KeyJsonFile));

	if (!FileJson.contains("key"))
	{
		return "";
	}
	return FileJson.at("key");
}
