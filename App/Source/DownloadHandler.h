#pragma once
#include "UriParse.h"

namespace DownloadHandler
{
	void DownloadModUri(Uri ModUri);

	void InstallZip(std::string ZipPath, std::string Name, std::string Description, std::string Image = "", int ModID = 0);

	void CheckDownloadRequest();
}