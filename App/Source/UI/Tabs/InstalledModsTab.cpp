#include "InstalledModsTab.h"
#include "ModInfo.h"
#include "../../WindowsFunctions.h"

static InstalledModsTab* CurrentInstalledTab = nullptr;

static void OnButtonClicked(int Index)
{
	CurrentInstalledTab->OpenModFromIndex(Index);
}

InstalledModsTab::InstalledModsTab()
{
	CurrentInstalledTab = this;
	OnButtonClickedFunction = &OnButtonClicked;
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
				.ModID = i.ModID,
			});
	}

	LoadSection(NexusInstalledMods, "Installed");
}

std::string InstalledModsTab::GetModImage(NexusModsAPI::ModInfo Mod)
{
	return Mod.ImageUrl;
}
