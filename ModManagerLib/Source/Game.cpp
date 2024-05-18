#include "Game.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "FileUtil.h"

static std::string GameLocation;

#if _WIN32
#include <Windows.h>


std::string Game::SearchForGOGGame()
{
	std::string StringData;

	DWORD StringSize = 0;
	RegGetValueA(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WOW6432Node\\GOG.com\\Games\\1423049311",
		"path",
		RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
		NULL,
		NULL,
		&StringSize
	);

	StringData.resize(StringSize, ' ');
	RegGetValueA(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WOW6432Node\\GOG.com\\Games\\1423049311",
		"path",
		RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
		NULL,
		StringData.data(),
		&StringSize
	);

	return StringData.substr(0, StringData.find_last_not_of(" \t"));
}

std::string Game::SearchForGame()
{
	return SearchForGOGGame();
}
#else
std::string Game::SearchForGOGGame()
{
	return "";
}

std::string Game::SearchForGame()
{
	FILE* FindCommand = popen("find / -type f -name Cyberpunk2077.exe", "r");

	while (!feof(FindCommand))
	{
		char Buffer[8000];
		size_t Read = fread(Buffer, sizeof(char), sizeof(Buffer) / sizeof(char) - 1, FindCommand);
		Buffer[Read] = 0;

		std::string BufferString = Buffer;
		BufferString = BufferString.substr(0, BufferString.find_last_of("/"));
		// Executable is located in GameDir/bin/x64/
		BufferString = std::filesystem::canonical(BufferString + "/../../").string();
		BufferString = BufferString.substr(0, BufferString.find_last_not_of(" \t") + 1);

		std::cout << "'" << BufferString << "'" << std::endl;
		if (!std::filesystem::exists(BufferString))
		{
			continue;
		}
		if (!std::filesystem::exists(BufferString + "/archive"))
		{
			continue;
		}

		pclose(FindCommand);
		std::cout << "FOUND: " << BufferString << std::endl;
		return BufferString;

	}
	pclose(FindCommand);
	return "";
}

#endif

static std::string GameDirFile = "app/saved/gameDir.txt";

std::string Game::GetGameLocation()
{
	if (GameLocation.empty())
	{
		if (std::filesystem::exists(GameDirFile))
		{
			GameLocation = FileUtil::ReadFile(GameDirFile);
		}

		if (!std::filesystem::exists(GameLocation))
		{
			GameLocation = SearchForGame();
			std::filesystem::create_directories("app/saved");
			std::ofstream LocationFile = std::ofstream(GameDirFile);
			LocationFile << GameLocation;
			LocationFile.close();
		}
	}
	return GameLocation;
}