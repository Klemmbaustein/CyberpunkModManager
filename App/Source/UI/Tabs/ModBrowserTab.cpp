#include "ModBrowserTab.h"

#include <KlemmUI/Window.h>
#include <KlemmUI/Rendering/Texture.h>

#include <iostream>
#include <mutex>
#include <filesystem>

#include "StrUtil.h"
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
	LoadSection(NexusModsAPI::GetMods("trending"), "Trending");
	LoadSection(NexusModsAPI::GetMods("latest_updated"), "Last Updated");
	LoadSection(NexusModsAPI::GetMods("latest_added"), "Last Added");
}
