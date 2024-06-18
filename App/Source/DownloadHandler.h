#pragma once
#include "UriParse.h"

namespace DownloadHandler
{
	void DownloadModUri(Uri ModUri);

	void InstallZip(
		std::string ZipPath,
		std::string Name,
		std::string Description,
		int ModID = 0,
		int FileID = 0,
		std::string FileCategory = ""
	);

	void CheckDownloadRequest();
}