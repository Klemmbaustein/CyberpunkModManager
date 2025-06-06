#include "SettingsTab.h"
#include <Settings.kui.hpp>
#include <Common.kui.hpp>
#include "NexusModsAPI.h"
#include "ModBrowserTab.h"
#include "../TitleBar.h"
#include "../../InstallerUpdate.h"
using namespace kui;

SettingsManager Settings;
bool LoadedSettings = false;

static void AddBoolEntry(std::string Name, std::string Value, std::string Default, UIBox* Parent)
{
	bool Val = Settings.GetValue(Value, Default) == "1";

	auto Element = new SettingsElement();
	Element->SetSettingName(Name);
	Parent->AddChild(Element);

	auto CheckButton = new CheckBox();

	if (Val)
	{
		CheckButton->SetImage("res:icons/Checkbox.png");
	}

	CheckButton->checkButton->OnClicked = [Value]()
		{
			bool NewValue = (Settings.GetValue(Value, "") == "0");
			Settings.SetValue(Value, NewValue ? "1" : "0");

			if (Value == "show_nsfw_mods")
			{
				AppTab::GetTabOfType<ModBrowserTab>()->ShouldReload = true;
			}
			if (Value == "use_custom_title_bar")
			{
				TitleBar::IsVisible = NewValue;
			}

			AppTab::GetTabOfType<SettingsTab>()->Generate();
		};


	Element->content->AddChild(CheckButton);
}

static void AddText(std::string TextName, std::string Text, UIBox* Parent)
{
	auto Element = new SettingsElement();
	Element->SetSettingName(TextName);
	Parent->AddChild(Element);
	Element->content->AddChild((new UIText(12_px, 1, Text, UI::Text)));
}

std::string SettingsTab::GetSetting(std::string Name, std::string Default)
{
	if (!LoadedSettings)
	{
		Settings = SettingsManager();
		LoadedSettings = true;
	}
	return Settings.GetValue(Name, Default);
}

SettingsTab::SettingsTab()
	: AppTab("Settings")
{
	IconFile = "settings.png";
	SettingsBox = new UIScrollBox(false, 0, true);

	TabBackground->HorizontalBoxAlign = UIBox::Align::Centered;
	SettingsBox->SetMinSize(Vec2f(1, 2));

	TabBackground->AddChild(SettingsBox);

	Generate();
}

void SettingsTab::Generate()
{
	SettingsBox->DeleteChildren();

	AddBoolEntry("Check for app updates on startup", "check_updates", "1", SettingsBox);
	AddBoolEntry("Show NSFW mods in the online mods browser", "show_nsfw_mods", "0", SettingsBox);
	AddBoolEntry("Is nxm:// URL handler", "is_uri_handler", "1", SettingsBox);
#if _WIN32
	AddBoolEntry("Use custom window title bar", "use_custom_title_bar", "1", SettingsBox);
#else
	AddBoolEntry("Use custom window title bar", "use_custom_title_bar", "0", SettingsBox);
#endif

	auto SettingsSeparator = new Separator();
	SettingsBox->AddChild(SettingsSeparator);

#if _WIN32
	std::string OsString = "Windows";
#else
	std::string OsString = "Linux";
#endif

	AddText("Mod Manager build", InstallerUpdate::BuildName + " (" + OsString + ") - " + std::string(__DATE__), SettingsBox);
}
