#pragma once
#include "ModListTab.h"
#include <kui/UI/UITextField.h>

class InstalledModsTab : public ModListTab
{
public:
	InstalledModsTab();

	kui::UITextField* SearchField = nullptr;
	std::string SearchFilter;

	virtual void LoadSections() override;
	virtual std::string GetModImage(NxmAPI::ModInfo Mod) override;
};