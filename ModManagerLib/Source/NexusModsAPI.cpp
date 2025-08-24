#include "NexusModsAPI.h"
#include "Net.h"
#include "FileUtil.h"
#include "StrUtil.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
using namespace nlohmann;

namespace NxmAPI
{
	std::string SourceUrl = "https://api.nexusmods.com/v1";
	std::string GameDomainName = "cyberpunk2077";
	static bool ShowNSFWMods = false;
}

NxmAPI::ModInfo NxmAPI::ModInfo::GetModFromID(int ID)
{
	json Response = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ID)
		+ ".json",
		true));
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

std::string NxmAPI::ModInfo::ModFile::DownloadLink(int ModID, int FileID, std::string Token, std::string Expires)
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
		+ Expires,
		true));

	return ResponseJson[0].at("URI");
}

NxmAPI::ModInfo::ModFile NxmAPI::ModInfo::ModFile::GetFileInfo(int ModID, int FileID)
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ModID)
		+ "/files/"
		+ std::to_string(FileID)
		+ ".json",
		true));

	return ModFile{
		.Category = ResponseJson.at("category_name")
	};
}

std::vector<NxmAPI::ModInfo::ModFile> NxmAPI::ModInfo::GetFiles(std::string Category) const
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ std::to_string(ModID)
		+ "/files.json?category=main",
		true));

	std::vector<ModFile> FoundFiles;

	try
	{
		for (const auto& i : ResponseJson.at("files"))
		{
			FoundFiles.push_back(ModFile{
				.Name = i.at("name"),
				.Description = i.at("description"),
				.Version = i.at("version"),
				.Category = i.at("category_name"),
				.FileID = i.at("file_id"),
				});
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	return FoundFiles;
}

std::string NxmAPI::ModInfo::GetImagePath() const
{
	if (std::filesystem::exists(ImageUrl))
	{
		return ImageUrl;
	}

	return "app/temp/images/" + Name + ".webp";
}

void NxmAPI::SetShowNSFWMods(bool NewValue)
{
	ShowNSFWMods = NewValue;
}

bool NxmAPI::GetIsLoggedIn()
{
	return !GetAPIKey().empty();
}

std::vector<NxmAPI::ModInfo> NxmAPI::GetMods(std::string Category)
{
	json ResponseJson = json::parse(Net::Get(SourceUrl
		+ "/games/"
		+ GameDomainName
		+ "/mods/"
		+ Category
		+ ".json",
		true));

	// Response should be an array of mods.
	if (!ResponseJson.is_array())
	{
		std::cout << ResponseJson.dump(2) << std::endl;
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

			if (i.at("contains_adult_content") && !ShowNSFWMods)
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

std::string NxmAPI::GetAPIKey()
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

void NxmAPI::SaveAPIKey(std::string Key)
{
	std::filesystem::create_directories("app/saved");
	std::ofstream Out = std::ofstream(KeyJsonFile);
	Out << json{ { "key", Key } }.dump(2);
	Out.close();
}

static json GetAccountInfo(std::string Key)
{
	Net::SetAPIKey(Key);
	json FileJson;
	try
	{
		FileJson = json::parse(Net::Get("https://api.nexusmods.com/v1/users/validate.json", true));
	}
	catch (json::exception& e)
	{
		return json::object();
	}
	if (FileJson.is_null())
		return json::object();
	Net::SetAPIKey(NxmAPI::GetAPIKey());
	return FileJson;
}

std::string NxmAPI::GetAPIKeyAccountName(std::string Key)
{
	json FileJson = GetAccountInfo(Key);
	if (!FileJson.contains("name"))
	{
		return "";
	}
	return FileJson.at("name");
}

std::string NxmAPI::GetAPIKeyAccountLink(std::string Key)
{
	json FileJson = GetAccountInfo(Key);
	if (!FileJson.contains("profile_url"))
	{
		return "";
	}
	return FileJson.at("profile_url");
}
