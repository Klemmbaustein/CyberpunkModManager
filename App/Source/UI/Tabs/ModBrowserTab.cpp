#include "ModBrowserTab.h"

#include <KlemmUI/Window.h>
#include <KlemmUI/Rendering/Texture.h>

#include <iostream>
#include <mutex>
#include <filesystem>

#include "StrUtil.h"
#include "SettingsTab.h"
using namespace KlemmUI;

static ModBrowserTab* CurrentBrowserTab = nullptr;

static void OnButtonClicked(int Index)
{
	CurrentBrowserTab->OpenModFromIndex(Index);
}

ModBrowserTab::ModBrowserTab()
{
	IconFile = "search_web.png";
	CurrentBrowserTab = this;
	OnButtonClickedFunction = &OnButtonClicked;
	LoadMainPage();
}

void ModBrowserTab::LoadSections()
{
	NxmAPI::SetShowNSFWMods(SettingsTab::GetSetting("show_nsfw_mods", "0") == "1");
	LoadSection(NxmAPI::GetMods("trending"), "Trending");
	LoadSection(NxmAPI::GetMods("latest_updated"), "Last Updated");
	LoadSection(NxmAPI::GetMods("latest_added"), "Last Added");
}
