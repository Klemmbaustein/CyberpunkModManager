#include "ModOptionSelection.h"
#include "../Markup/ModOption.hpp"
#include "UI.h"
#include "Tabs/InstalledModsTab.h"

using namespace KlemmUI;

static thread_local ModOptionsSelection* CurrentSelection = nullptr;

void ModOptionsSelection::GenerateOptions()
{
	PopupBackground->DeleteChildren();
	PopupBackground->SetHorizontalAlign(UIBox::Align::Centered);

	PopupBackground->AddChild((new UIText(15, 1, "Select mod version", UI::Text))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetPadding(0.02f));

	auto Options = TargetMod.GetVersions();

	int ButtonIndex = 0;
	for (auto& i : Options)
	{
		auto Option = new ModOption();
		Option->SetOptionName(i);
		Option->button->OnClickedFunctionIndex = [](int Index)
			{
				CurrentSelection->TargetMod.InstallVersion(CurrentSelection->TargetMod.GetVersions()[Index]);
				CurrentSelection->ShouldClose = true;

				AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};
		Option->button->ButtonIndex = ButtonIndex++;
		PopupBackground->AddChild(Option);
	}
}

void ModOptionsSelection::LoadMod(ModInfo Mod)
{
	std::lock_guard g{ ModLoadMutex };

	TargetMod = Mod;
	LoadedMod = false;
}

void ModOptionsSelection::Init()
{
	CurrentSelection = this;
}

void ModOptionsSelection::Update()
{
	std::lock_guard g{ ModLoadMutex };
	if (LoadedMod)
	{
		return;
	}
	LoadedMod = true;
	GenerateOptions();
}

std::string ModOptionsSelection::GetWindowTitle()
{
	return std::string();
}

Vector2ui ModOptionsSelection::GetWindowResolution()
{
	return Vector2ui(400, 500);
}
