#pragma once
#include "AppTab.h"
#include <HomeTab.kui.hpp>

class HomeTab : public AppTab
{
public:
	HomeTab();
	GameSection* GamePart = nullptr;
	ProfileSection* ProfilePart = nullptr;
	bool ShouldReload = false;

	void Update() override;

private:

	void UpdateGame();
	void UpdateAccount();
	void UpdateProfileLoggedInButtons();
	void MakeLoginForm();
};