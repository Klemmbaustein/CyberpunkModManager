#pragma once
#include "Popup.h"
#include "../Markup/SetupWindowElement.hpp"
#include <KlemmUI/UI/UITextField.h>

class SetupWindow : public Popup
{
	SetupWindowElement* Element = nullptr;

	KlemmUI::UITextField* InputField = nullptr;
	KlemmUI::UIBox* InfoBox = nullptr;
public:
	void Init() override;
	std::string GetWindowTitle() override;

	static bool ShouldOpen();

	void AddText(std::string NewText, KlemmUI::UIBox* Parent = nullptr);
	void GenerateAccountInfo();

	void GenerateAPIKeySetupPage();

	void GenerateGameLocatePage();
};