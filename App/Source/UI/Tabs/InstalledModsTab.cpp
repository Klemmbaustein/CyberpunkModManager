#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"
#include "../../BackgroundTask.h"
#include "../../Markup/ModInfoButton.hpp"
#include "../../DownloadHandler.h"
#include "../LoadingBar.h"
#include "../ProfileWindow.h"
#include "StrUtil.h"

using namespace KlemmUI;

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
	HeaderBox->SetTryFill(true);

	auto UpdateButton = new ModInfoButton();
	UpdateButton->SetText("Check for mod updates");
	UpdateButton->SetImage("app/icons/search_web.png");
	UpdateButton->button->OnClickedFunction = [this]() {
		new BackgroundTask(CheckModUpdates, [this]() {
			ShouldReload = true;
			});
		};
	TopBox->AddChild(UpdateButton);

	auto LocalModButton = new ModInfoButton();
	LocalModButton->SetText("Install local mod");
	LocalModButton->SetImage("app/icons/storage.png");
	LocalModButton->button->OnClickedFunction = []()
		{
			std::string File = Windows::OpenFileDialog();
			std::string FileName = File.substr(File.find_last_of("/\\") + 1);
			FileName = FileName.substr(0, FileName.find_last_of("."));
			DownloadHandler::InstallZip(File, FileName, "Mod installed from local files.");
		};
	TopBox->AddChild(LocalModButton);

	auto ProfilesButton = new ModInfoButton();
	ProfilesButton->SetText("Profiles");
	ProfilesButton->SetImage("app/icons/storage.png");
	ProfilesButton->button->OnClickedFunction = []()
		{
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
