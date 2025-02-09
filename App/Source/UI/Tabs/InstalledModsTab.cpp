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

	CanSelectMods = true;

	HeaderBox->SetHorizontal(false);
	HeaderBox->SetMaxWidth(UISize::Parent(1));
	HeaderBox->SetMinWidth(UISize::Parent(1));

	AddButtons();

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
			.Enabled = i.Enabled,
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

void InstalledModsTab::Update()
{
	ModListTab::Update();
	UpdateDisableButton();
}

void InstalledModsTab::UpdateViewModeButton()
{
	switch (CurrentViewMode)
	{
	case ModListTab::ViewMode::Tiles:
		ViewModeButton->SetText("Show list");
		ViewModeButton->SetImage("res:icons/list.png");
		break;
	case ModListTab::ViewMode::List:
		ViewModeButton->SetText("Show tiles");
		ViewModeButton->SetImage("res:icons/tiles.png");
		break;
	default:
		break;
	}
}

void InstalledModsTab::UpdateDisableButton()
{
	static SelectedButtonMode LastButtonMode = SelectedButtonMode::DisableAll;
	ButtonMode = LastButtonMode;

	if (SelectedMods.empty())
	{
		bool Found = false;
		for (ModsSection& s : LoadedMods)
		{
			for (auto& m : s.Mods)
			{
				if (m.Enabled)
				{
					Found = true;
				}
			}
		}

		ButtonMode = Found ? SelectedButtonMode::DisableAll : SelectedButtonMode::EnableAll;
	}
	else
	{
		bool Found = false;
		for (ModsSection& s : LoadedMods)
		{
			for (auto& m : s.Mods)
			{
				if (m.Enabled && SelectedMods.contains(m.Name))
				{
					Found = true;
				}
			}
		}
		ButtonMode = Found ? SelectedButtonMode::DisableSelected : SelectedButtonMode::EnableSelected;
	}

	if (ButtonMode == LastButtonMode)
	{
		return;
	}
	LastButtonMode = ButtonMode;

	switch (ButtonMode)
	{
	case InstalledModsTab::SelectedButtonMode::DisableAll:
		SelectActionButton->SetText("Disable all");
		SelectActionButton->SetImage("res:icons/disabled.png");
		break;
	case InstalledModsTab::SelectedButtonMode::EnableAll:
		SelectActionButton->SetText("Enable all");
		SelectActionButton->SetImage("res:icons/enabled.png");
		break;
	case InstalledModsTab::SelectedButtonMode::DisableSelected:
		SelectActionButton->SetText("Disable selected");
		SelectActionButton->SetImage("res:icons/disabled.png");
		break;
	case InstalledModsTab::SelectedButtonMode::EnableSelected:
		SelectActionButton->SetText("Enable selected");
		SelectActionButton->SetImage("res:icons/enabled.png");
		break;
	default:
		break;
	}
}

void InstalledModsTab::AddButtons()
{
	UIBox* TopBox = new UIBox(true);
	HeaderBox->AddChild(TopBox);

	auto UpdateButton = new AppButton();
	UpdateButton->SetText("Check for updates");
	UpdateButton->SetImage("res:icons/search_web.png");
	UpdateButton->button->OnClicked = [this]() {
		new BackgroundTask(CheckModUpdates, [this]()
			{
				ShouldReload = true;
			});
		};
	TopBox->AddChild(UpdateButton);

	auto LocalModButton = new AppButton();
	LocalModButton->SetText("Add local mod");
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

	ViewModeButton = new AppButton();
	UpdateViewModeButton();
	ViewModeButton->button->OnClicked = [this]() {
		CurrentViewMode = CurrentViewMode == ViewMode::Tiles ? ViewMode::List : ViewMode::Tiles;
		Generate();
		UpdateImages();
		UpdateViewModeButton();
		};
	TopBox->AddChild(ViewModeButton);

	SelectActionButton = new AppButton();
	SelectActionButton->SetText("Disable all");
	SelectActionButton->SetImage("res:icons/disabled.png");
	SelectActionButton->button->OnClicked = [this]() {
		auto Installed = ModInfo::GetAllInstalledMods();
		for (auto& m : Installed)
		{
			switch (ButtonMode)
			{
			case InstalledModsTab::SelectedButtonMode::DisableAll:
				m.Disable();
				break;
			case InstalledModsTab::SelectedButtonMode::EnableAll:
				m.Enable();
				break;
			case InstalledModsTab::SelectedButtonMode::DisableSelected:
				if (SelectedMods.contains(m.Name))
				{
					m.Disable();
				}
				break;
			case InstalledModsTab::SelectedButtonMode::EnableSelected:
				if (SelectedMods.contains(m.Name))
				{
					m.Enable();
				}
				break;
			default:
				break;
			}
		}
		ShouldReload = true;
		};
	TopBox->AddChild(SelectActionButton);


}
