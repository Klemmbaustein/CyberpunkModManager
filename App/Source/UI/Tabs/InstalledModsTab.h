#pragma once
#include "ModListTab.h"
#include <KlemmUI/UI/UITextField.h>

class InstalledModsTab : public ModListTab
{
public:
	InstalledModsTab();

	KlemmUI::UITextField* SearchField = nullptr;
	std::string SearchFilter;

	virtual void LoadSections() override;
	virtual std::string GetModImage(NxmAPI::ModInfo Mod) override;
};