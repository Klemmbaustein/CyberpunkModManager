#pragma once
#include <string>

namespace GhAPI
{
	std::string GetLatestReleaseOf(std::string RepoName);
	
	std::string DownloadLatestReleaseOf(std::string RepoName, std::string NecessaryAssetName = "");
}