#pragma once
#include "Popup.h"
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <atomic>
#include <mutex>

class LoadingBar : public Popup
{
	std::string LoadingString;
public:

	KlemmUI::UIBackground* LoadingBackground = nullptr;
	KlemmUI::UIBackground* LoadingBarBox = nullptr;

	KlemmUI::UIText* LoadingText = nullptr;

	std::mutex PopupMutex;

	void SetLoadingString(const std::string& NewValue);

	void Init() override;
	bool HasBorder() override;
	void Update() override;
	std::string GetWindowTitle() override;

	Vector2ui GetWindowResolution() override;

	std::atomic<std::atomic<float>*> ProgressValue = nullptr;
};