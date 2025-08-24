#include "ModBrowserTab.h"
#include "SettingsTab.h"

#include <Common.kui.hpp>
#include "../../WindowsFunctions.h"

using namespace kui;

ModBrowserTab::ModBrowserTab()
	: ModListTab("Online mods")
{
	IconFile = "search_web.png";

	auto UpdateButton = new AppButton();
	UpdateButton->SetText("Open NexusMods website");
	UpdateButton->SetImage("res:icons/open.png");
	UpdateButton->button->OnClicked = []() {

		Windows::Open("https://nexusmods.com/cyberpunk2077");
		};
	HeaderBox->AddChild(UpdateButton);

	LoadMainPage();
}

void ModBrowserTab::LoadSections()
{
	if (!NxmAPI::GetIsLoggedIn())
	{
		return;
	}
	NxmAPI::SetShowNSFWMods(SettingsTab::GetSetting("show_nsfw_mods", "0") == "1");
	LoadSection(NxmAPI::GetMods("trending"), "Trending");
	LoadSection(NxmAPI::GetMods("latest_updated"), "Last Updated");
	LoadSection(NxmAPI::GetMods("latest_added"), "Last Added");
}

void ModBrowserTab::OnGenerated()
{
	if (NxmAPI::GetIsLoggedIn())
	{
		return;
	}

	ContentBox->DeleteChildren();
	auto HomePageButton = new AppButton();
	HomePageButton->SetText("Open home tab");
	HomePageButton->SetImage("res:icons/home.png");
	HomePageButton->button->OnClicked = []() {
		AppTab::SelectedTab = 0;
		};

	ContentBox->AddChild((new UIBox(false))
		->SetPadding(10_px)
		->AddChild((new UIText(12_px, 1, "Not logged in. Set up your NexusMods account on the home tab.", UI::Text))
			->SetPadding(5_px))
		->AddChild(HomePageButton));
}
