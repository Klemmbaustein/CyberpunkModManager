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

		NexusInstalledMods.push_back(NewMod);
	}

	LoadSection(NexusInstalledMods, "Installed");
}

std::string InstalledModsTab::GetModImage(NxmAPI::ModInfo Mod)
{
	return Mod.ImageUrl;
}
