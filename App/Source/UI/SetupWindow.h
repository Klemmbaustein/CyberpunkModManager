#pragma once
#include "Popup.h"
#include <SetupWindow.kui.hpp>
#include <kui/UI/UITextField.h>

class SetupWindow : public Popup
{
	SetupWindowElement* Element = nullptr;

	kui::UITextField* InputField = nullptr;
	kui::UIBox* InfoBox = nullptr;
public:
	void Init() override;
	std::string GetWindowTitle() override;

	static bool ShouldOpen();

	void AddText(std::string NewText, kui::UIBox* Parent = nullptr);
	void GenerateAccountInfo();

	void GenerateAPIKeySetupPage();

	void GenerateGameLocatePage();
};