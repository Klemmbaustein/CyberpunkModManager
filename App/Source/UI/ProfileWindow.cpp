#include "ProfileWindow.h"
#include <filesystem>
#include "../WindowsFunctions.h"
#include <ProfileWindow.kui.hpp>
#include "Profile.h"
#include "Tabs/InstalledModsTab.h"
#include "LoadingBar.h"
#include <iostream>
#include <kui/UI/UITextField.h>

static void ProfileEnable(Profile Target, ProfileWindow* TargetWindow)
{
	bool LoadingBarRequired = Target.GetModsCount() > 0;

	LoadingBar* LoadBar = nullptr;

	if (LoadingBarRequired)
	{
		LoadBar = Popup::CreatePopup<LoadingBar>();
		LoadBar->SetLoadingString("Loading...");
	}
	Target.MakeActive();
	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
	TargetWindow->GenerateList();

	if (LoadingBarRequired)
	{
		LoadBar->ShouldClose = true;
	}
}

static void ProfileDelete(Profile Target, ProfileWindow* TargetWindow)
{
	// Do not delete the last profile
	if (Profile::GetAllProfiles().size() == 1)
	{
		return;
	}

	if (!Windows::YesNoBox("Really delete '" + Target.DisplayName + "'?"))
	{
		return;
	}

	bool LoadingBarRequired = Target.GetModsCount() > 0;

	LoadingBar* LoadBar = nullptr;

	if (LoadingBarRequired)
	{
		LoadBar = Popup::CreatePopup<LoadingBar>();
		LoadBar->SetLoadingString("Deleting...");
	}

	if (Target.IsCurrent())
	{
		Profile::GetAllProfiles()[0].MakeActive();
	}

	std::filesystem::remove_all(Target.Path);
	if (LoadingBarRequired)
	{
		LoadBar->ShouldClose = true;
	}

	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
	TargetWindow->GenerateList();
}

static bool IsValidProfileName(const std::string& Name)
{
	if (Name.empty())
	{
		return false;
	}
	
	char InvalidChars[] = {
		':',
		'?',
		'<',
		'>',
		'|',
		'"',
		'*',
		'/',
		'\\',
		'\n',
		'='
	};

	for (char c : InvalidChars)
	{
		if (Name.find(c) != std::string::npos)
		{
			return false;
		}
	}

	return true;
}

static void ProfileCopy(Profile Target, ProfileWindow* TargetWindow)
{
	bool LoadingBarRequired = Target.GetModsCount() > 0;

	LoadingBar* LoadBar = nullptr;

	if (LoadingBarRequired)
	{
		LoadBar = Popup::CreatePopup<LoadingBar>();
		LoadBar->SetLoadingString("Copying profile...");
	}
	if (!Target.Copy())
	{
		Windows::ErrorBox("Failed to copy profile.");
	}
	if (LoadingBarRequired)
	{
		LoadBar->ShouldClose = true;
	}

	TargetWindow->GenerateList();
}

void ProfileWindow::GenerateList()
{
	using namespace kui;

	ProfilesList->DeleteChildren();

	auto Profiles = Profile::GetAllProfiles();
	for (const Profile& pf : Profiles)
	{
		bool Selected = pf.IsCurrent();
		auto Entry = new ProfileEntry();
		Entry->activeIcon->IsVisible = Selected;

		Entry->profileButton->OnClicked = std::bind(ProfileEnable, pf, this);

		auto Color = Selected ? Vec3f(1, 0.3f, 0.4f) : Vec3f(0.6f, 0.1f, 0.1f);

		Entry->SetColor(Color);
		UITextField* NameTextField = nullptr;
		NameTextField = new UITextField(0, Color, UI::Text, nullptr);

		NameTextField->OnChanged = [pf, NameTextField, this]()
			{
				std::string NewName = NameTextField->GetText();
				Profile ProfileCopy = pf;

				if (IsValidProfileName(NewName))
				{
					ProfileCopy.Rename(NewName);
				}
				else
				{
					Windows::ErrorBox("Not a valid profile name.");
				}
				GenerateList();
			};

		NameTextField
			->SetText(pf.DisplayName)
			->SetTextSize(15_px)
			->SetOpacity(0)
			->SetMinSize(Vec2f(1, 0));

		Entry->nameBox->AddChild(NameTextField);
		Entry->SetDescription(std::to_string(pf.GetModsCount()) + (Selected ? " mods (active)" : " mods"));
		Entry->deleteButton->button->OnClicked = std::bind(ProfileDelete, pf, this);
		Entry->copyButton->button->OnClicked = std::bind(ProfileCopy, pf, this);
		ProfilesList->AddChild(Entry);
	}
}

void ProfileWindow::Init()
{
	using namespace kui;

	ProfilesList = new UIScrollBox(false, 0, true);

	PopupBackground->AddChild(ProfilesList);

	ProfilesList->SetMinSize(Vec2f(2.0f, 1.7f));
	ProfilesList->SetMaxSize(ProfilesList->GetMinSize());

	auto NewBox = new NewProfileBox();
	PopupBackground->AddChild(NewBox);
	NewBox
		->SetPadding(10_px);

	NewBox->newButton->button->OnClicked = [this]() {

		Profile::NewProfile("New profile").MakeActive();

		AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
		GenerateList();
		};

	GenerateList();
}

void ProfileWindow::Update()
{
}

std::string ProfileWindow::GetWindowTitle()
{
	return "Profiles";
}
