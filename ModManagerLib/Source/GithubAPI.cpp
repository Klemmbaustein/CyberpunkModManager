#include "GithubAPI.h"
#include <nlohmann/json.hpp>
#include <Net.h>
#include <iostream>
using namespace nlohmann;

std::string GhAPI::GetLatestReleaseOf(std::string RepoName)
{
	auto response = json::parse(Net::Get("https://api.github.com/repos/" + RepoName + "/releases/latest", false));

	std::string LatestName = response.at("name");
	return LatestName;
}

std::string GhAPI::DownloadLatestReleaseOf(std::string RepoName, std::string NecessaryAssetName)
{
	auto response = json::parse(Net::Get("https://api.github.com/repos/" + RepoName + "/releases/latest", false));

	for (auto& elem : response["assets"])
	{
		if (!NecessaryAssetName.empty() && elem.at("name").get<std::string>().find(NecessaryAssetName) == std::string::npos)
		{
			continue;
		}

		std::string url = elem.at("browser_download_url");

		std::string UrlExtension = url.substr(url.find_last_of("/"));
		UrlExtension = UrlExtension.substr(UrlExtension.find_first_of("."));

		Net::GetFile(url, "app/temp/latest" + UrlExtension, false);
		return "app/temp/latest" + UrlExtension;
	}
	return "";
}
