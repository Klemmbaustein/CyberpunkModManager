#include "ModBrowserTab.h"
#include "SettingsTab.h"

#include "../../Markup/ModInfoButton.hpp"
#include "../../WindowsFunctions.h"

static ModBrowserTab* CurrentBrowserTab = nullptr;

static void OnButtonClicked(int Index)
{
	CurrentBrowserTab->OpenModFromIndex(Index);
}

ModBrowserTab::ModBrowserTab()
	: ModListTab("Online mods")
{
	IconFile = "search_web.png";
	CurrentBrowserTab = this;
	OnButtonClickedFunction = &OnButtonClicked;

	auto UpdateButton = new ModInfoButton();
	UpdateButton->SetText("Open NexusMods website");
	UpdateButton->SetImage("app/icons/open.png");
	UpdateButton->button->OnClickedFunction = []() {

#if _WIN32
		Windows::Open("https://nexusmods.com/cyberpunk2077");
#else
		Windows::Open("xdg-open https://nexusmods.com/cyberpunk2077 &");
#endif
		};
	HeaderBox->AddChild(UpdateButton);

	LoadMainPage();
}

void ModBrowserTab::LoadSections()
{
	NxmAPI::SetShowNSFWMods(SettingsTab::GetSetting("show_nsfw_mods", "0") == "1");
	LoadSection(NxmAPI::GetMods("trending"), "Trending");
	LoadSection(NxmAPI::GetMods("latest_updated"), "Last Updated");
	LoadSection(NxmAPI::GetMods("latest_added"), "Last Added");
}
