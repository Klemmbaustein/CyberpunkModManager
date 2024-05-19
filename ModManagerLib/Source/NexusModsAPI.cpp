#include "NexusModsAPI.h"
#include "Net.h"
#include "FileUtil.h"
#include "StrUtil.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
using namespace nlohmann;

namespace NexusModsAPI
{
	std::string SourceUrl = "https://api.nexusmods.com/v1";
	std::string GameDomainName = "cyberpunk2077";
}

NexusModsAPI::ModInfo NexusModsAPI::ModInfo::GetModFromID(int ID)
{
	json Response = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ID)
		+ ".json"));
	// Skip the mod if it isn't available.
	// For some reason the API lists unavailable mods but doesn't give much info for them.
	if (!Response.contains("available") || !Response.at("available").get<bool>())
	{
		return ModInfo();
	}

	if (!Response.contains("name"))
	{
		return ModInfo();
	}

	if (Response.at("picture_url").is_null())
	{
		Response.at("picture_url") = std::string();
	}

	ModInfo NewMod = ModInfo{
		.Name = Response.at("name"),
		.Description = StrUtil::Replace(StrUtil::Replace(Response.at("description"), "\n", ""), "<br />", "\n"),
		.Summary = StrUtil::Replace(StrUtil::Replace(Response.at("summary"), "\n", ""), "<br />", "\n"),
		.ImageUrl = Response.at("picture_url"),
		.ModID = Response.at("mod_id"),
	};

	return NewMod;
}

std::string NexusModsAPI::ModInfo::ModFile::DownloadLink(int ModID, int FileID, std::string Token, std::string Expires)
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ModID)
		+ "/files/"
		+ std::to_string(FileID)
		+ "/download_link.json?key="
		+ Token
		+ "&expires="
		+ Expires));

	return ResponseJson[0].at("URI");
}

std::vector<NexusModsAPI::ModInfo::ModFile> NexusModsAPI::ModInfo::GetFiles() const
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ModID)
		+ "/files.json?category=main"));

	std::vector<ModFile> FoundFiles;

	for (const auto& i : ResponseJson.at("files"))
	{
		FoundFiles.push_back(ModFile{
			.Name = i.at("name"),
			.Description = i.at("description"),
			.Version = i.at("version"),
			});
	}

	return FoundFiles;
}

std::string NexusModsAPI::ModInfo::GetImagePath() const
{
	if (std::filesystem::exists(ImageUrl))
	{
		return ImageUrl;
	}

	return "app/temp/images/" + Name + ".webp";
}

bool NexusModsAPI::GetIsLoggedIn()
{
	return !GetAPIKey().empty();
}

std::vector<NexusModsAPI::ModInfo> NexusModsAPI::GetMods(std::string Category)
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ Category
		+ ".json"));

	// Response should be an array of mods.
	if (!ResponseJson.is_array())
	{
		std::cout << ResponseJson.dump() << std::endl;
		return {};
	}

	try
	{
		std::vector<ModInfo> FoundMods;
		int NumMods = 0;
		for (json& i : ResponseJson)
		{
			// Skip the mod if it isn't available.
			// For some reason the API lists unavailable mods but doesn't give much info for them.
			if (!i.contains("available") || !i.at("available").get<bool>())
			{
				continue;
			}

			if (i.at("contains_adult_content"))
			{
				continue;
			}

			if (!i.contains("name"))
			{
				continue;
			}

			if (i.at("picture_url").is_null())
			{
				i.at("picture_url") = std::string();
			}

			ModInfo NewMod = ModInfo{
				.Name = i.at("name"),
				.Description = StrUtil::Replace(StrUtil::Replace(i.at("description"), "\n", ""), "<br />", "\n"),
				.Summary = StrUtil::Replace(StrUtil::Replace(i.at("summary"), "\n", ""), "<br />", "\n"),
				.ImageUrl = i.at("picture_url"),
				.InfoString = "Downloads: " + std::to_string(i.at("mod_downloads").get<int>()),
				.ModID = i.at("mod_id"),
				.Downloads = i.at("mod_downloads"),
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

static const std::string KeyJsonFile = "app/saved/apikey.json";

std::string NexusModsAPI::GetAPIKey()
{

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

void NexusModsAPI::SaveAPIKey(std::string Key)
{
	std::filesystem::create_directories("app/saved");
	std::ofstream Out = std::ofstream(KeyJsonFile);
	Out << json{ { "key", Key } };
	Out.close();
}

std::string NexusModsAPI::GetAPIKeyAccountName(std::string Key)
{
	Net::SetAPIKey(Key);
	json FileJson = json::parse(Net::Get("https://api.nexusmods.com/v1/users/validate.json"));
	Net::SetAPIKey(GetAPIKey());
	if (!FileJson.contains("name"))
	{
		return "";
	}
	return FileJson.at("name");
}
