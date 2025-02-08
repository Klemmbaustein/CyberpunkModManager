#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"
#include <Common.kui.hpp>
#include "../../BackgroundTask.h"
#include "../../DownloadHandler.h"
#include "../LoadingBar.h"
#include "../ProfileWindow.h"
#include "StrUtil.h"
#include "Profile.h"
#include <iostream>

using namespace kui;

static void CheckModUpdates()
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
	: ModListTab("Installed mods")
{
	IconFile = "storage.png";

	UIBox* TopBox = new UIBox(true);
	HeaderBox->SetHorizontal(false);
	HeaderBox->AddChild(TopBox);
	HeaderBox->SetMaxWidth(UISize::Parent(1));
	HeaderBox->SetMinWidth(UISize::Parent(1));

	auto UpdateButton = new AppButton();
	UpdateButton->SetText("Check for mod updates");
	UpdateButton->SetImage("res:icons/search_web.png");
	UpdateButton->button->OnClicked = [this]() {
		new BackgroundTask(CheckModUpdates, [this]()
			{
				ShouldReload = true;
			});
		};
	TopBox->AddChild(UpdateButton);

	auto LocalModButton = new AppButton();
	LocalModButton->SetText("Install local mod");
	LocalModButton->SetImage("res:icons/storage.png");
	LocalModButton->button->OnClicked = []() {
		std::string File = Windows::OpenFileDialog(false);

		if (File.empty())
		{
			return;
		}

		std::string FileName = File.substr(File.find_last_of("/\\") + 1);

		FileName = FileName.substr(0, FileName.find_last_of("."));
		DownloadHandler::InstallZip(File, FileName, "Mod installed from local files.");
		};
	TopBox->AddChild(LocalModButton);

	auto ProfilesButton = new AppButton();
	ProfilesButton->SetText("Profiles");
	ProfilesButton->SetImage("res:icons/profile.png");
	ProfilesButton->button->OnClicked = []() {
		Popup::CreatePopup<ProfileWindow>();
		};
	TopBox->AddChild(ProfilesButton);

	SearchField = new UITextField(0, 0.05f, UI::Text, [this]() {
		std::string NewFilter = SearchField->GetText();

		if (SearchFilter != NewFilter)
		{
			SearchFilter = NewFilter;
			ShouldReload = true;
		}
		});

	HeaderBox->AddChild(SearchField
		->SetHintText("Search installed mods")
		->SetTextSize(13_px)
		->SetCorner(5_px)
		->SetPadding(5_px)
		->SetMinWidth(UISize::Parent(1))
		->SetMaxWidth(UISize::Parent(1)));

	LoadMainPage();
}

void InstalledModsTab::LoadSections()
{
	auto Installed = ModInfo::GetAllInstalledMods();

	std::vector<NxmAPI::ModInfo> NexusInstalledMods;

	for (auto& i : Installed)
	{
		auto NewMod = NxmAPI::ModInfo{
			.Name = i.Name,
			.Summary = i.Description,
			.InfoString = i.Enabled ? "Enabled" : "Disabled",
			.InfoColor = i.Enabled ? NxmAPI::ModInfo::Green : NxmAPI::ModInfo::Red,
			.ModID = i.ModID,
		};

		NewMod.ImageUrl = GetModImage(NewMod);

		if (!SearchFilter.empty() && StrUtil::Lower(i.Name).find(StrUtil::Lower(SearchFilter)) == std::string::npos
			&& StrUtil::Lower(i.Description).find(StrUtil::Lower(SearchFilter)) == std::string::npos)
		{
			continue;
		}

		if (i.RequiresUpdate)
		{
			NewMod.InfoString = "Enabled - Requires update",
				NewMod.InfoColor = NxmAPI::ModInfo::Yellow;
		}
		NexusInstalledMods.push_back(NewMod);
	}

	LoadSection(NexusInstalledMods, "Installed (Profile: " + Profile::Current.DisplayName + ")");
}

std::string InstalledModsTab::GetModImage(NxmAPI::ModInfo Mod)
{
	return Profile::Current.Path + "/images/" + Mod.Name + ".webp";
}
