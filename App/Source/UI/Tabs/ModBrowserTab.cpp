#include "ModBrowserTab.h"
#include "SettingsTab.h"

#include <Common.kui.hpp>
#include "../../WindowsFunctions.h"


ModBrowserTab::ModBrowserTab()
	: ModListTab("Online mods")
{
	IconFile = "search_web.png";

	auto UpdateButton = new AppButton();
	UpdateButton->SetText("Open NexusMods website");
	UpdateButton->SetImage("res:icons/open.png");
	UpdateButton->button->OnClicked = []() {

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
	if (!NxmAPI::GetIsLoggedIn())
		return;

	NxmAPI::SetShowNSFWMods(SettingsTab::GetSetting("show_nsfw_mods", "0") == "1");
	LoadSection(NxmAPI::GetMods("trending"), "Trending");
	LoadSection(NxmAPI::GetMods("latest_updated"), "Last Updated");
	LoadSection(NxmAPI::GetMods("latest_added"), "Last Added");
}
