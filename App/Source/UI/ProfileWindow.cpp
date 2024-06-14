#include "ProfileWindow.h"
#include <filesystem>
#include "../WindowsFunctions.h"
#include "../Markup/ProfileEntry.hpp"
#include "Profile.h"
#include "Tabs/InstalledModsTab.h"
#include <KlemmUI/UI/UITextField.h>

static void ProfileEnable(Profile Target, ProfileWindow* TargetWindow)
{
	Target.MakeActive();
	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
	TargetWindow->GenerateList();
}

static void DeleteProfile(Profile Target, ProfileWindow* TargetWindow)
{
	if (Windows::YesNoBox("Really delete '" + Target.DisplayName + "'?"))
	{
		std::filesystem::remove_all(Target.Path);
		AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
		TargetWindow->GenerateList();
	}
}

void ProfileWindow::GenerateList()
{
	PopupBackground->DeleteChildren();

	auto Profiles = Profile::GetAllProfiles();
	for (const Profile& pf : Profiles)
	{
		bool Selected = pf.Path == Profile::Current.Path;
		auto Entry = new ProfileEntry();
		Entry->activeIcon->IsVisible = Selected;
		Entry->profileButton->OnClickedFunction = std::bind(ProfileEnable, pf, this);
		Entry->nameBox->AddChild(new KlemmUI::UITextField(1, 0, UI::Text, 0));
		Entry->SetDescription(Selected ? "0 mods (active)" : "0 mods");
		Entry->SetColor(Selected ? Vector3f(1, 0.3f, 0.4f) : Vector3f(0.6f, 0.1f, 0.1f));
		Entry->deleteButton->button->OnClickedFunction = std::bind(DeleteProfile, pf, this);
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
