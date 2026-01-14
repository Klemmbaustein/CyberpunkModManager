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
#include "UI/ModOptionSelection.h"
#include "Profile.h"

static std::string ModDownloadUrl;
static int ModID;
static int FileID;
static void DownloadModAsync()
{
	std::string CurrentModUrl = ModDownloadUrl;
	int CurrentModID = ModID;
	int CurrentFileID = FileID;

	auto Mod = NxmAPI::ModInfo::GetModFromID(CurrentModID);
	auto File = NxmAPI::ModInfo::ModFile::GetFileInfo(CurrentModID, CurrentFileID);

	std::atomic<float> Progress = 0;
	auto LoadBar = Popup::CreatePopup<LoadingBar>();
	LoadBar->ProgressValue = &Progress;

	LoadBar->SetLoadingString("Downloading mod: " + Mod.Name);

	std::filesystem::create_directories("app/temp/");

	std::string ArchiveName = "app/temp/" + ModDownloadUrl.substr(ModDownloadUrl.find_last_of("/") + 1);
	ArchiveName = ArchiveName.substr(0, ArchiveName.find_first_of("?"));
	std::cout << "Downloading " << ArchiveName << std::endl;

	Net::GetFile(ModDownloadUrl, ArchiveName, true, LoadBar->ProgressValue);
	LoadBar->ProgressValue = nullptr;
	LoadBar->SetLoadingString("Installing mod...");

	std::filesystem::create_directories(Profile::Current.Path + "/images");
	std::string ImagePath = Profile::Current.Path + "/images/" + Mod.Name + ".webp";
	Net::GetFile(Mod.ImageUrl, ImagePath, true);
	DownloadHandler::InstallZip(ArchiveName, Mod.Name, Mod.Summary, CurrentModID, CurrentFileID, File.Category);
	std::filesystem::remove(ArchiveName);
	LoadBar->ShouldClose = true;
	LoadBar->CanClose = true;
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
	FileID = std::stoi(ModUri.Path[3]);
	ModDownloadUrl = NxmAPI::ModInfo::ModFile::DownloadLink(
		ModID,
		FileID,
		ModUri.Query["key"],
		ModUri.Query["expires"]);

	new BackgroundTask(&DownloadModAsync);
}
void DownloadHandler::InstallZip(std::string ZipPath, std::string Name, std::string Description, int CurrentModID, int CurrentFileID, std::string FileCategory)
{
	auto PrevMod = ModInfo::GetModByName(Name);
	if (!PrevMod.Name.empty())
	{
		PrevMod.Remove();
	}

	std::string ModPath = Profile::Current.Path + "/mod_files/" + Name + "/";
	Archive::Extract(ZipPath, ModPath, nullptr, 0);

	std::filesystem::create_directories(Profile::Current.Path + "/mod_files/");
	std::filesystem::create_directories(Profile::Current.Path + "/images/");

	auto ModFiles = FileUtil::GetAllFilesInFolder(ModPath);

	ModInfo NewMod = ModInfo{
		.Name = Name,
		.Description = Description,
		.FileCategory = FileCategory,
		.ModID = CurrentModID,
		.FileID = CurrentFileID,
		.Enabled = false,
		.Files = ModFiles,
	};

	std::filesystem::create_directories(Profile::Current.Path);
	NewMod.Save();
	if (NewMod.ContainsMultipleVersions())
	{
		auto Option = Popup::CreatePopup<ModOptionsSelection>();
		Option->LoadMod(NewMod);
	}
	else
	{
		NewMod.Enable();
	}
	NewMod.CheckModUpdateStatus();
	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
}

static std::string OpenFile;
void DownloadHandler::CheckDownloadRequest()
{
	if (std::filesystem::exists("app/OpenUri"))
	{
		OpenFile = FileUtil::ReadFile("app/OpenUri");

		new BackgroundTask([]() {
			DownloadModUri(OpenFile);
		});
		std::filesystem::remove("app/OpenUri");
	}
}