#pragma once
#include "ModListTab.h"
#include <kui/UI/UITextField.h>
#include <Common.kui.hpp>

class InstalledModsTab : public ModListTab
{
public:
	InstalledModsTab();

	kui::UITextField* SearchField = nullptr;
	std::string SearchFilter;

	virtual void LoadSections() override;
	virtual std::string GetModImage(NxmAPI::ModInfo Mod) override;

private:

	enum class SelectedButtonMode
	{
		DisableAll,
		EnableAll,
		DisableSelected,
		EnableSelected,
	};

	virtual void Update() override;

	void UpdateViewModeButton();
	void UpdateDisableButton();

	SelectedButtonMode ButtonMode = SelectedButtonMode::DisableAll;

	void AddButtons();
	AppButton* ViewModeButton = nullptr;
	AppButton* SelectActionButton = nullptr;
};