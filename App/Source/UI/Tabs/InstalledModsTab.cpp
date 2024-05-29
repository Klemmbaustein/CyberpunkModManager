#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"
#include <mutex>
#include "../../BackgroundTask.h"
#include "../../Markup/ModInfoButton.hpp"
#include "../LoadingBar.h"
#include "StrUtil.h"

using namespace KlemmUI;

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
	: ModListTab("Installed mods")
{
	IconFile = "storage.png";
	CurrentInstalledTab = this;
	OnButtonClickedFunction = &OnButtonClicked;
	
	UIBox* TopBox = new UIBox(true);
	HeaderBox->SetHorizontal(false);
	HeaderBox->AddChild(TopBox);
	HeaderBox->SetTryFill(true);

	auto UpdateButton = new ModInfoButton();
	UpdateButton->SetText("Check for mod updates");
	UpdateButton->SetImage("app/icons/search_web.png");
	UpdateButton->button->OnClickedFunction = []() {
		new BackgroundTask(CheckModUpdates, [](void*) {
			CurrentInstalledTab->ShouldReload = true;
			});
		};
	TopBox->AddChild(UpdateButton);

	auto LocalModButton = new ModInfoButton();
	LocalModButton->SetText("Install local mod");
	LocalModButton->SetImage("app/icons/storage.png");
	LocalModButton->button->OnClickedFunction = []()
		{
			Windows::ErrorBox("Not implemented");
		};
	TopBox->AddChild(LocalModButton);

	SearchField = new UITextField(0, 0.05f, UI::Text, []() {
		std::string NewFilter = CurrentInstalledTab->SearchField->GetText();

		if (CurrentInstalledTab->SearchFilter != NewFilter)
		{
			CurrentInstalledTab->SearchFilter = NewFilter;
			CurrentInstalledTab->ShouldReload = true;
		}
		});

	HeaderBox->AddChild(SearchField
		->SetHintText("Search installed mods")
		->SetTextSize(11)
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetPadding(5)
		->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative)
		->SetTryFill(true));

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

		if (!SearchFilter.empty()
			&& StrUtil::Lower(i.Name).find(StrUtil::Lower(SearchFilter)) == std::string::npos
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

	LoadSection(NexusInstalledMods, "Installed");
}

std::string InstalledModsTab::GetModImage(NxmAPI::ModInfo Mod)
{
	return Mod.ImageUrl;
}
