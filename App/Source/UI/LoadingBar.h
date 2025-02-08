#pragma once
#include "Popup.h"
#include <kui/UI/UIText.h>
#include <kui/UI/UIBackground.h>
#include <atomic>
#include <mutex>

class LoadingBar : public Popup
{
	std::string LoadingString;
public:

	kui::UIBackground* LoadingBackground = nullptr;
	kui::UIBackground* LoadingBarBox = nullptr;

	kui::UIText* LoadingText = nullptr;

	std::mutex PopupMutex;

	void SetLoadingString(const std::string& NewValue);

	void Init() override;
	bool HasBorder() override;
	void Update() override;
	std::string GetWindowTitle() override;

	kui::Vec2ui GetWindowResolution() override;

	std::atomic<std::atomic<float>*> ProgressValue = nullptr;
};