#include "DownloadHandler.h"
#include "WindowsFunctions.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "NexusModsAPI.h"
#include "FileUtil.h"
#include "UI/LoadingBar.h"
#include "BackgroundTask.h"
#include "Net.h"
#include "Archive.h"
#include "ModInfo.h"
#include "UI/Tabs/InstalledModsTab.h"

static std::string ModDownloadUrl;
static int ModID;
static void DownloadModAsync()
{
	std::string CurrentModUrl = ModDownloadUrl;
	int CurrentModID = ModID;
	auto Mod = NexusModsAPI::ModInfo::GetModFromID(CurrentModID);

	std::atomic<float> Progress = 0;
	auto LoadBar = Popup::CreatePopup<LoadingBar>();
	LoadBar->ProgressValue = &Progress;

	LoadBar->LoadingString = "Downloading mod...";
	Net::GetFile(ModDownloadUrl, "app/temp/dl.zip", LoadBar->ProgressValue);
	LoadBar->ProgressValue = nullptr;
	LoadBar->LoadingString = "Extracting mod...";

	std::string ModPath = "app/profiles/test/mod_files/" + Mod.Name + "/";
	Archive::Extract("app/temp/dl.zip", ModPath, nullptr, 0);

	std::filesystem::create_directories("app/profiles/test/images/");
	std::string ImagePath = "app/profiles/test/images/" + Mod.Name + ".webp";
	Net::GetFile(Mod.ImageUrl, ImagePath);

	auto ModFiles = FileUtil::GetAllFilesInFolder(ModPath);

	ModInfo NewMod = ModInfo{
		.Name = Mod.Name,
		.Description = Mod.Summary,
		.ImagePath = ImagePath,
		.ModID = CurrentModID,
		.Enabled = false,
		.Files = ModFiles,
	};

	LoadBar->LoadingString = "Saving mod information...";
	std::filesystem::create_directories("app/profiles/test/");
	NewMod.WriteFile("app/profiles/test/" + Mod.Name + ".json");
	LoadBar->ShouldClose = true;
	LoadBar->CanClose = true;
	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
}

void DownloadHandler::DownloadModUri(Uri ModUri)
{
	if (Windows::IsProcessRunning(Windows::GetCurrentProcessName()))
	{
		std::ofstream ProcessNotify = std::ofstream("app/OpenUri");
		ProcessNotify << ModUri.UriString;
		ProcessNotify.close();
		exit(0);
	}

	if (ModUri.Scheme != "nxm")
	{
		Windows::ErrorBox("Invalid Uri scheme: " + ModUri.Scheme + "\nURI: " + ModUri.UriString);
		return;
	}

	if (ModUri.Authority != "cyberpunk2077")
	{
		Windows::ErrorBox("Unsupported game: " + ModUri.Authority + "\nURI: " + ModUri.UriString);
		return;
	}

	// mods / {modId} / files / {fileId)
	if (ModUri.Path.size() != 4)
	{
		Windows::ErrorBox("Invalid path in URI\nURI: " + ModUri.UriString);
		return;
	}

	for (auto& i : ModUri.Query)
	{
		std::cout << i.first << " = " << i.second << std::endl;
	}

	ModID = std::stoi(ModUri.Path[1]);

	ModDownloadUrl = NexusModsAPI::ModInfo::ModFile::DownloadLink(
		ModID,
		std::stoi(ModUri.Path[3]),
		ModUri.Query["key"],
		ModUri.Query["expires"]
	);

	new BackgroundTask(&DownloadModAsync);
}

void DownloadHandler::CheckDownloadRequest()
{
	if (std::filesystem::exists("app/OpenUri"))
	{
		static std::string OpenFile = FileUtil::ReadFile("app/OpenUri");
		
		new BackgroundTask([]() {
			DownloadModUri(OpenFile);
			});
		std::filesystem::remove("app/OpenUri");
	}
}
