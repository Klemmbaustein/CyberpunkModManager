#include "ProfileWindow.h"
#include <filesystem>
#include "../WindowsFunctions.h"
#include "../Markup/ProfileEntry.hpp"
#include "../Markup/NewProfileBox.hpp"
#include "Profile.h"
#include "Tabs/InstalledModsTab.h"
#include "LoadingBar.h"
#include <iostream>

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
	using namespace KlemmUI;

	ProfilesList->DeleteChildren();

	auto Profiles = Profile::GetAllProfiles();
	for (const Profile& pf : Profiles)
	{
		bool Selected = pf.IsCurrent();
		auto Entry = new ProfileEntry();
		Entry->activeIcon->IsVisible = Selected;

		Entry->profileButton->OnClickedFunction = std::bind(ProfileEnable, pf, this);

		Vector3f Color = Selected ? Vector3f(1, 0.3f, 0.4f) : Vector3f(0.6f, 0.1f, 0.1f);

		Entry->SetColor(Color);
		UITextField* NameTextField = nullptr;
		NameTextField = new UITextField(0, Color, UI::Text, nullptr);

		NameTextField->OnClickedFunction = [pf, NameTextField, this]()
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
				this->GenerateList();
			};

		NameTextField
			->SetText(pf.DisplayName)
			->SetTextSize(15)
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
			->SetMinSize(Vector2f(1, 0));

		Entry->nameBox->AddChild(NameTextField);
		Entry->SetDescription(std::to_string(pf.GetModsCount()) + (Selected ? " mods (active)" : " mods"));
		Entry->deleteButton->button->OnClickedFunction = std::bind(ProfileDelete, pf, this);
		Entry->copyButton->button->OnClickedFunction = std::bind(ProfileCopy, pf, this);
		ProfilesList->AddChild(Entry);
	}
}

void ProfileWindow::Init()
{
	using namespace KlemmUI;

	ProfilesList = new UIScrollBox(false, 0, true);

	PopupBackground->AddChild(ProfilesList);

	ProfilesList->SetMinSize(Vector2f(2.0f, 1.7f));
	ProfilesList->SetMaxSize(ProfilesList->GetMinSize());

	auto NewBox = new NewProfileBox();
	PopupBackground->AddChild(NewBox);

	NewNameTextField = new UITextField(0, 0, UI::Text, nullptr);

	NewBox->nameBox->AddChild(NewNameTextField);
	NewNameTextField
		->SetHintText("Profile name")
		->SetTextSize(13)
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetMinSize(Vector2f(1.75f, 0.01f));

	NewBox->newButton->button->OnClickedFunction = [this]() {

		std::string ProfileName = NewNameTextField->GetText();

		if (!IsValidProfileName(ProfileName))
		{
			Windows::ErrorBox("Enter a valid profile name");
			return;
		}

		Profile::NewProfile(ProfileName).MakeActive();

		NewNameTextField->SetText("");

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
