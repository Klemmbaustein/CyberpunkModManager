#include "SettingsTab.h"
#include "../../Markup/SettingsElement.hpp"
#include "../../Markup/CheckBox.hpp"
#include "../../Markup/Separator.hpp"
#include "../../InstallerUpdate.h"
#include "NexusModsAPI.h"
#include "ModBrowserTab.h"
using namespace KlemmUI;

SettingsManager Settings;
bool LoadedSettings = false;

static std::vector<std::string> BoolValues;
static int BoolIt = 0;

static void AddBoolEntry(std::string Name, std::string Value, std::string Default, UIBox* Parent)
{
	bool Val = Settings.GetValue(Value, Default) == "1";

	auto Element = new SettingsElement();
	Element->SetSettingName(Name);
	Parent->AddChild(Element);

	auto CheckButton = new CheckBox();

	if (Val)
	{
		CheckButton->SetImage("app/icons/Checkbox.png");
	}

	CheckButton->checkButton->OnClickedFunctionIndex = [](int Index)
		{
			std::string ValueName = BoolValues.at(Index);
			Settings.SetValue(ValueName, (Settings.GetValue(ValueName, "") == "1") ? "0" : "1");

			if (ValueName == "show_nsfw_mods")
			{
				AppTab::GetTabOfType<ModBrowserTab>()->ShouldReload = true;
			}

			AppTab::GetTabOfType<SettingsTab>()->Generate();
		};
	CheckButton->checkButton->ButtonIndex = BoolIt++;

	BoolValues.push_back(Value);

	Element->content->AddChild(CheckButton);
}

static void AddText(std::string TextName, std::string Text, UIBox* Parent)
{
	auto Element = new SettingsElement();
	Element->SetSettingName(TextName);
	Parent->AddChild(Element);
	Element->content->AddChild((new UIText(12, 1, Text, UI::Text))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative));
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
	SettingsBox->SetMinSize(Vector2f(1, 2));

	TabBackground->AddChild(SettingsBox);

	Generate();
}

void SettingsTab::Generate()
{
	BoolIt = 0;
	BoolValues.clear();

	SettingsBox->DeleteChildren();

	AddBoolEntry("Check for app updates on startup", "check_updates", "1", SettingsBox);
	AddBoolEntry("Show NSFW mods in the mod browser", "show_nsfw_mods", "0", SettingsBox);
	AddBoolEntry("Is nxm:// URL handler", "is_uri_handler", "1", SettingsBox);

	auto SettingsSeparator = new Separator();
	SettingsBox->AddChild(SettingsSeparator);

#if _WIN32
	std::string OsString = "Windows";
#else
	std::string OsString = "Linux";
#endif

	AddText("Mod Manager build", InstallerUpdate::BuildName + " (" + OsString + ") - " + std::string(__DATE__), SettingsBox);
}
