#include "ModOptionSelection.h"
#include <ModOptions.kui.hpp>
#include "UI.h"
#include "Tabs/InstalledModsTab.h"

using namespace kui;

void ModOptionsSelection::GenerateOptions()
{
	PopupBackground->DeleteChildren();
	PopupBackground->SetHorizontalAlign(UIBox::Align::Centered);

	PopupBackground->AddChild((new UIText(15_px, 1, "Select mod version", UI::Text))
		->SetPadding(0.02f));

	auto Options = TargetMod.GetVersions();

	for (auto& i : Options)
	{
		auto Option = new ModOption();
		Option->SetOptionName(i);
		Option->button->OnClicked = [i, this]()
			{
				TargetMod.InstallVersion(i);
				ShouldClose = true;

				AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};
		PopupBackground->AddChild(Option);
	}
	PopupBackground->RedrawElement();
}

void ModOptionsSelection::LoadMod(ModInfo Mod)
{
	std::lock_guard g{ ModLoadMutex };

	TargetMod = Mod;
	LoadedMod = false;
}

void ModOptionsSelection::Init()
{
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
	return "Select mod version";
}

Vec2ui ModOptionsSelection::GetWindowResolution()
{
	return Vec2ui(400, 500);
}
