#pragma once
#include "AppTab.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include "SettingsManager.h"

extern SettingsManager Settings;

class SettingsTab : public AppTab
{
public:
	static std::string GetSetting(std::string Name, std::string Default);

	KlemmUI::UIScrollBox* SettingsBox = nullptr;

	SettingsTab();

	void Generate();
};