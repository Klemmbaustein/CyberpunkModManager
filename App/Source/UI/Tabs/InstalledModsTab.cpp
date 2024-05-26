#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"
#include <mutex>
#include "../../BackgroundTask.h"
#include "../../Markup/ModInfoButton.hpp"
#include "../LoadingBar.h"

static InstalledModsTab* CurrentInstalledTab = nullptr;

static void OnButtonClicked(int Index)
{
	CurrentInstalledTab->OpenModFromIndex(Index);
}

static void CheckModUpdates(void*)
{
	auto LoadBar = Popup::CreatePopup<LoadingBar>();

	std::atomic<float> ProgressValue = 0;
	LoadBar->ProgressValue = &ProgressValue;
	LoadBar->SetLoadingString("Checking for mod updates");
	auto AllMods = ModInfo::GetAllInstalledMods();

	for (auto& i : AllMods)
	{
		i.CheckModUpdateStatus();

		ProgressValue += 1.0f / (float)AllMods.size();
	}
	LoadBar->CanClose = true;
	LoadBar->ShouldClose = true;
}

InstalledModsTab::InstalledModsTab()
{
	IconFile = "storage.png";
	CurrentInstalledTab = this;
	OnButtonClickedFunction = &OnButtonClicked;
	
	auto UpdateButton = new ModInfoButton();
	UpdateButton->SetText("Check for mod updates");
	UpdateButton->SetImage("app/icons/search_web.png");
	UpdateButton->button->OnClickedFunction = []() {
		new BackgroundTask(CheckModUpdates, [](void*) {
			CurrentInstalledTab->ShouldReload = true;
			});
		};
	HeaderBox->AddChild(UpdateButton);

	auto LocalModButton = new ModInfoButton();
	LocalModButton->SetText("Install local mod");
	LocalModButton->SetImage("app/icons/storage.png");
	LocalModButton->button->OnClickedFunction = []()
		{
			Windows::ErrorBox("Not implemented");
		};
	HeaderBox->AddChild(LocalModButton);

	LoadMainPage();
}

void InstalledModsTab::LoadSections()
{
	auto Installed = ModInfo::GetAllInstalledMods();

	std::vector<NxmAPI::ModInfo> NexusInstalledMods;

	for (auto& i : Installed)
	{
		auto NewMod = NxmAPI::ModInfo
		{
			.Name = i.Name,
			.Summary = i.Description,
			.ImageUrl = i.ImagePath,
			.InfoString = i.Enabled ? "Enabled" : "Disabled",
			.InfoColor = i.Enabled ? NxmAPI::ModInfo::Green : NxmAPI::ModInfo::Red,
			.ModID = i.ModID,
		};

		if (i.RequiresUpdate)
		{
			NewMod.InfoString = "Enabled - Requires update",
			NewMod.InfoColor = NxmAPI::ModInfo::Yellow;
		}
		NexusInstalledMods.push_back(NewMod);
	}

	LoadSection(NexusInstalledMods, "Installed");
}

std::string InstalledModsTab::GetModImage(NxmAPI::ModInfo Mod)
{
	return Mod.ImageUrl;
}
