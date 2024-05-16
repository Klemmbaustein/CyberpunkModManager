#pragma once
#include <string>
#include <vector>

namespace NexusModsAPI
{
	struct ModInfo
	{
		static ModInfo GetModFromID(int ID);

		std::string Name;
		std::string Description;
		std::string Summary;
		std::string ImageUrl;
		uint8_t* ImageBuffer = nullptr;
		int ImageWidth = 0, ImageHeight = 0;
		int ModID = 0;
		int Downloads;

		struct ModFile
		{
			std::string Name;
			std::string Description;
			std::string Version;

			ModInfo* Mod = nullptr;
			int FileID = 0;

			static std::string DownloadLink(int ModID, int FileID, std::string Token, std::string Expires);
		};
		std::vector<ModFile> GetFiles() const;
		std::string GetImagePath() const;
	};

	extern std::string GameDomainName;

	std::vector<ModInfo> GetMods(std::string Category);

	// Gets the current API key. Returns an empty string if there is none.
	std::string GetAPIKey();
}