#include "Game.h"
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

	return StringData;
}
