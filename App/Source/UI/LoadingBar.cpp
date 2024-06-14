#include "LoadingBar.h"
#include "UI.h"

using namespace KlemmUI;

void LoadingBar::SetLoadingString(const std::string& NewValue)
{
	std::lock_guard g(PopupMutex);

	LoadingString = NewValue;
}

void LoadingBar::Init()
{
	PopupWindow->BorderColor = Vector3f(0.6f, 0.1f, 0.1f);
	PopupWindow->IsAreaGrabbableCallback = [](Window* Target) { return true; };

	LoadingBackground = new UIBackground(true, Vector2f(-0.9f, -0.6f), 0.15f, Vector2f(1.8f, 0.6f));
	LoadingBarBox = new UIBackground(true, 0, Vector3f(0.6f, 0.1f, 0.1f), 0);

	LoadingBackground->AddChild(LoadingBarBox
		->SetPadding(0));

	LoadingText = new UIText(8, 1, "", UI::Text);
	LoadingText->SetPosition(Vector2f(-0.9f, 0.1f));

	CanClose = false;
}

bool LoadingBar::HasBorder()
{
	return false;
}

void LoadingBar::Update()
{
	if (ShouldClose)
	{
		CanClose = true;
		return;
	}

	std::lock_guard g(PopupMutex);
	{
		LoadingText->SetText(LoadingString);
	}

	if (ProgressValue)
	{
		LoadingBarBox->SetMinSize(Vector2f(1.8f * *ProgressValue, 0.6f));
		LoadingBarBox->SetPadding(0);
	}
	else
	{
		float Progress = fmod(PopupWindow->Time, 2.6f);


		float Start = std::max(Progress - 0.8f, 0.0f);
		float End = std::min(std::max(Progress - 0.4f, 0.0f), 1.8f);

		LoadingBarBox->SetPadding(0, 0, Start, 0);
		LoadingBarBox->SetMinSize(Vector2f(End - Start, 0.6f));
	}
}

std::string LoadingBar::GetWindowTitle()
{
	return "Loading";
}

Vector2ui LoadingBar::GetWindowResolution()
{
	return Vector2ui(400, 75);
}
