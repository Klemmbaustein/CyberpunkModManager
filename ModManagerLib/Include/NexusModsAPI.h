#pragma once
#include <string>
#include <vector>

namespace NexusModsAPI
{
	struct ModInfo
	{
		std::string Name;
		int ModID = 0;

	};

	extern std::string GameDomainName;

	std::vector<ModInfo> GetMods();

	// Gets the current API key. Returns an empty string if there is none.
	std::string GetAPIKey();
}