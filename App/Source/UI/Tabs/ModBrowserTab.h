#pragma once
#include "ModListTab.h"
#include <kui/UI/UIScrollBox.h>
#include "NexusModsAPI.h"

class ModBrowserTab : public ModListTab
{
public:
	ModBrowserTab();
	void LoadSections() override;
};