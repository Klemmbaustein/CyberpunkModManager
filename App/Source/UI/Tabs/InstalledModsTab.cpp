#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"
#include <mutex>

static InstalledModsTab* CurrentInstalledTab = nullptr;

static void OnButtonClicked(int Index)
{
	CurrentInstalledTab->OpenModFromIndex(Index);
}

InstalledModsTab::InstalledModsTab()
{
	IconFile = "storage.png";
	CurrentInstalledTab = this;
	OnButtonClickedFunction = &OnButtonClicked;
	LoadMainPage();
}

void InstalledModsTab::LoadSections()
{
	auto Installed = ModInfo::GetAllInstalledMods();

	std::vector<NexusModsAPI::ModInfo> NexusInstalledMods;

	for (auto& i : Installed)
	{
		NexusInstalledMods.push_back(NexusModsAPI::ModInfo
			{
				.Name = i.Name,
				.Summary = i.Description,
				.ImageUrl = i.ImagePath,
				.InfoString = i.Enabled ? "Enabled" : "Disabled",
				.InfoColor = i.Enabled ? NexusModsAPI::ModInfo::Green : NexusModsAPI::ModInfo::Red,
				.ModID = i.ModID,
			});
	}

	LoadSection(NexusInstalledMods, "Installed");
}

std::string InstalledModsTab::GetModImage(NexusModsAPI::ModInfo Mod)
{
	return Mod.ImageUrl;
}
