#pragma once
#include "Popup.h"
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <atomic>

class LoadingBar : public Popup
{
public:

	KlemmUI::UIBackground* LoadingBackground = nullptr;
	KlemmUI::UIBackground* LoadingBarBox = nullptr;

	KlemmUI::UIText* LoadingText = nullptr;

	std::string LoadingString;

	void Init() override;
	bool HasBorder() override;
	void Update() override;
	std::string GetWindowTitle() override;

	Vector2ui GetWindowResolution() override;

	std::atomic<std::atomic<float>*> ProgressValue = nullptr;
};