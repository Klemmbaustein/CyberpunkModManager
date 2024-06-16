#include "ProfileWindow.h"
#include <filesystem>
#include "../WindowsFunctions.h"
#include "../Markup/ProfileEntry.hpp"
#include "Profile.h"
#include "Tabs/InstalledModsTab.h"
#include <KlemmUI/UI/UITextField.h>
#include "LoadingBar.h"

static void ProfileEnable(Profile Target, ProfileWindow* TargetWindow)
{
	bool LoadingBarRequired = Target.GetModsCount() > 0;

	LoadingBar* LoadBar = nullptr;

	if (LoadingBarRequired)
	{
		LoadBar = Popup::CreatePopup<LoadingBar>();
		LoadBar->SetLoadingString("Copying files...");
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

	if (Target.IsCurrent())
	{
		Profile::GetAllProfiles()[0].MakeActive();
	}

	std::filesystem::remove_all(Target.Path);

	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
	TargetWindow->GenerateList();

}

static bool IsValidProfileName(const std::string& Name)
{
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
	if (!Target.Copy())
	{
		Windows::ErrorBox("Failed to copy profile.");
	}
	TargetWindow->GenerateList();
}

void ProfileWindow::GenerateList()
{
	using namespace KlemmUI;

	PopupBackground->DeleteChildren();

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
		PopupBackground->AddChild(Entry);
	}
}

void ProfileWindow::Init()
{
	GenerateList();
}

void ProfileWindow::Update()
{
	
}

std::string ProfileWindow::GetWindowTitle()
{
	return "Profiles";
}
