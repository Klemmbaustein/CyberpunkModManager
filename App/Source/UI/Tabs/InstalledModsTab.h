#pragma once
#include "ModListTab.h"

class InstalledModsTab : public ModListTab
{
public:
	InstalledModsTab();

	virtual void LoadSections() override;
	virtual std::string GetModImage(NexusModsAPI::ModInfo Mod) override;
};