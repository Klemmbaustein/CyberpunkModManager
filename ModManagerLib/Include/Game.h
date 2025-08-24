#pragma once
#include <string>

namespace Game
{
	std::string SearchForGOGGame();
	bool HasGameLocation();

	std::string SearchForGame();

	std::string GetGameLocation();

	void SetGameLocation(std::string NewPath);
}