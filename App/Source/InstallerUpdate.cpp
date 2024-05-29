#include "InstallerUpdate.h"
#include "BackgroundTask.h"
#include "GithubAPI.h"
#include "Archive.h"
#include <iostream>
#include "WindowsFunctions.h"
#include "UI/LoadingBar.h"

#ifdef CI_BUILD
const std::string InstallerUpdate::BuildName = "Build " + std::to_string(CI_BUILD);
#else
const std::string InstallerUpdate::BuildName = "DevBuild";
#endif

const std::string SourceRepo = "Klemmbaustein/CyberpunkModManager";
#if _WIN32
const std::string PlatformName = "Windows";
#elif __linux__
const std::string PlatformName = "Linux";
#endif

static void UpdateCheckFunction(void*)
{
	using namespace InstallerUpdate;
	std::string Latest = GhAPI::GetLatestReleaseOf(SourceRepo);

	if (BuildName != Latest && Windows::YesNoBox("Update required.\n" + BuildName + " -> " + Latest + "\nUpdate now?"))
	{
		auto LoadBar = Popup::CreatePopup<LoadingBar>();
		LoadBar->SetLoadingString("Downloading update");

		try
		{
			std::string Release = GhAPI::DownloadLatestReleaseOf(SourceRepo, PlatformName);
			std::string Extension = Release.substr(Release.find_first_of("."));

			LoadBar->SetLoadingString("Extracting update");

			Archive::Extract(Release, "app/temp/update/", nullptr, 0);
		}
		catch (std::exception& e)
		{
			Windows::ErrorBox(e.what());
		}

#if _WIN32
		system("app\\bin\\update.bat");
#else
		system("sh app/bin/update.sh &");
#endif
		exit(0);
	}
}

void InstallerUpdate::CheckForUpdate()
{
#ifdef CI_BUILD
	new BackgroundTask(UpdateCheckFunction);
#endif
}
