#include "ModInfoWindow.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/Rendering/Texture.h>
#include "../Webp.h"
#include "Net.h"
#include "UI.h"
#include "../Markup/ModInfoButton.hpp"
#include "ModInfo.h"
#include <filesystem>
#include <iostream>
#include "ModOptionSelection.h"
#include "../WindowsFunctions.h"
#include "Tabs/InstalledModsTab.h"

using namespace KlemmUI;

thread_local ModInfoWindow* ModInfoWindow::CurrentWindow = nullptr;

void ModInfoWindow::LoadInfo()
{
	PopupBackground->DeleteChildren();
	NxmAPI::ModInfo Mod = GetModInfo();

	InfoElement = new ModInfoWindowElement();

	InfoElement->header->SetName(Mod.Name);
	InfoElement->header->SetSubTitle(Mod.Summary);

	InfoElement->header->image->SetUseTexture(true, Webp::Load(Webp::LoadBuffer(Mod.GetImagePath())));

	GenerateActionButtons(InfoElement->actionBox, Mod);

	RenderMarkupString(Mod.Description, InfoElement->descriptionBox);

	PopupWindow->SetTitle("Mod info: " + Mod.Name);

	PopupBackground->AddChild((new UIScrollBox(false, 0, true))
		->SetMinSize(2)
		->SetMaxSize(2)
		->AddChild(InfoElement));
}

static void OpenModInBrowser(const NxmAPI::ModInfo& Mod, std::string Tab = "")
{
	std::string Link = "https://nexusmods.com/cyberpunk2077/mods/" + std::to_string(Mod.ModID) + Tab;

#if _WIN32
	Windows::Open(Link);
#else
	Windows::Open("xdg-open \"" + Link + "\" &");
#endif
}

void ModInfoWindow::GenerateActionButtons(KlemmUI::UIBox* Parent, const NxmAPI::ModInfo& Mod)
{
	Parent->DeleteChildren();
	ActionsBox = Parent;

	// If the mod isn't installed, an empty value is returned.
	auto ModInf = ModInfo::GetModByName(Mod.Name);

	if (ModInf.Name.empty())
	{
		auto* InstallButton = new ModInfoButton();
		InstallButton->SetText("Install");
		InstallButton->button->OnClickedFunction = []() {
			// Can't actually install anything since NexusMods is the worst thing ever.
			OpenModInBrowser(CurrentWindow->GetModInfo(), "?tab=files");
			};
		Parent->AddChild(InstallButton);
		InstallButton->SetImage("app/icons/download.png");
	}
	else
	{
		if (Mod.InfoColor == NxmAPI::ModInfo::Yellow)
		{
			auto* UpdateButton = new ModInfoButton();
			UpdateButton->SetText("Update");
			UpdateButton->button->OnClickedFunction = []() {
				OpenModInBrowser(CurrentWindow->GetModInfo(), "?tab=files");
				CurrentWindow->ShouldClose = true;
				};
			Parent->AddChild(UpdateButton);
			UpdateButton->SetImage("app/icons/download.png");
		}

		auto* EnableButton = new ModInfoButton();
		EnableButton->SetText(ModInf.Enabled ? "Disable" : "Enable");
		EnableButton->button->OnClickedFunction = []() {
			auto Mod = ModInfo::GetModByName(CurrentWindow->GetModInfo().Name);

			if (Mod.Enabled)
			{
				Mod.Disable();
			}
			else
			{
				if (Mod.ContainsMultipleVersions())
				{
					auto Option = Popup::CreatePopup<ModOptionsSelection>();
					Option->LoadMod(Mod);
					CurrentWindow->ShouldClose = true;
					return;
				}
				else
				{
					Mod.Enable();
				}
			}

			CurrentWindow->GenerateActionButtons(CurrentWindow->ActionsBox, CurrentWindow->GetModInfo());
			AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};

		EnableButton->SetImage(ModInf.Enabled ? "app/icons/disabled.png" : "app/icons/enabled.png");

		Parent->AddChild(EnableButton);

		auto* RemoveButton = new ModInfoButton();
		RemoveButton->SetText("Uninstall");
		RemoveButton->button->OnClickedFunction = []() {
			auto Mod = ModInfo::GetModByName(CurrentWindow->GetModInfo().Name);
			Mod.Remove();
			CurrentWindow->GenerateActionButtons(CurrentWindow->ActionsBox, CurrentWindow->GetModInfo());
			AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};
		RemoveButton->SetImage("app/icons/delete.png");
		Parent->AddChild(RemoveButton);

	}

	if (ModInf.ModID != 0 || Mod.ModID != 0)
	{
		auto* OpenInBrowserButton = new ModInfoButton();
		OpenInBrowserButton->SetText("Open website");
		OpenInBrowserButton->button->OnClickedFunction = []() {
			OpenModInBrowser(CurrentWindow->GetModInfo());
			};
		OpenInBrowserButton->SetImage("app/icons/open.png");
		Parent->AddChild(OpenInBrowserButton);
	}

}

Vector2ui ModInfoWindow::GetWindowResolution()
{
	return Vector2ui(800, 700);
}

void ModInfoWindow::SetModInfo(NxmAPI::ModInfo Info)
{
	std::lock_guard Guard = std::lock_guard(InfoMutex);

	if (NxmAPI::GetIsLoggedIn() && Info.ModID != 0)
	{
		std::string ModImage = Info.ImageUrl;
		auto Color = Info.InfoColor;
		LoadedInfo = NxmAPI::ModInfo::GetModFromID(Info.ModID);
		LoadedInfo.ImageUrl = ModImage;
		LoadedInfo.InfoColor = Color;
	}
	else
	{
		LoadedInfo = Info;
	}
	HasInfo = true;
}

NxmAPI::ModInfo ModInfoWindow::GetModInfo()
{
	std::lock_guard Guard = std::lock_guard(InfoMutex);
	return LoadedInfo;
}

std::string ModInfoWindow::GetWindowTitle()
{
	return "";
}

void ModInfoWindow::Init()
{
	CurrentWindow = this;
	PopupBackground->SetHorizontalAlign(UIBox::Align::Centered);
	PopupBackground->SetVerticalAlign(UIBox::Align::Centered);
	PopupBackground->AddChild(new UIText(1, 1, "Loading...", UI::Text));

}

void ModInfoWindow::Update()
{
	if (HasInfo)
	{
		HasInfo = false;
		LoadInfo();
	}
}

void ModInfoWindow::RenderMarkupString(std::string Markup, KlemmUI::UIBox* Parent)
{
	std::string NewLine = "";
	std::string Tag;
	std::string TagContent;
	bool IsTag = false;
	bool Centered = false;
	bool InTag = false;

	for (char i : Markup)
	{
		if (i == '[')
		{
			IsTag = true;
			continue;
		}

		if (i == ']')
		{
			IsTag = false;
			if (Tag.empty())
			{
				continue;
			}
			InTag = Tag == "img";

			if (Tag == "/img")
			{
				std::filesystem::create_directories("app/temp/preview/");
				Net::GetFile(TagContent, "app/temp/preview/img.png", false);

				Texture::TextureInfo Tex = Texture::LoadTextureWithInfo("app/temp/preview/img.png");
				if (Tex.Width != 0 && Tex.Height != 0)
				{
					float SizeScale = std::min(Tex.Width / 400.0f, 1.95f);

					Parent->AddChild((new UIBox(true))
						->SetTryFill(true)
						->SetHorizontalAlign(Centered ? UIBox::Align::Centered : UIBox::Align::Default)
						->AddChild((new UIBackground(true, 0, 1, Vector2f(SizeScale) * Vector2f(1, (float)Tex.Height / (float)Tex.Width)))
							->SetUseTexture(true, Tex.ID)
							->SetSizeMode(UIBox::SizeMode::AspectRelative)));
				}
			}

			if (Tag == "center")
			{
				Centered = true;
			}
			if (Tag == "/center")
			{
				Centered = false;
			}
			TagContent.clear();
			Tag.clear();
			continue;
		}

		if (IsTag)
		{
			Tag.push_back(i);
			continue;
		}

		if (InTag)
		{
			TagContent.push_back(i);
			continue;
		}

		if (i == '\n')
		{
			RenderMarkupLine(NewLine, Centered, Parent);
			NewLine.clear();
		}
		else
		{
			NewLine.push_back(i);
		}
	}
	RenderMarkupLine(NewLine, Centered, Parent);

}

void ModInfoWindow::RenderMarkupLine(std::string Line, bool Centered, KlemmUI::UIBox* Parent)
{
	// No.
	if (Line.find("Bitcoin") != std::string::npos
		|| Line.find("Dogecoin") != std::string::npos
		|| Line.find("Ethereum") != std::string::npos)
	{
		return;
	}

	Parent->AddChild((new UIBox(true))
		->SetTryFill(true)
		->SetHorizontalAlign(Centered ? UIBox::Align::Centered : UIBox::Align::Default)
		->AddChild((new UIText(11, 1, Line, UI::Text))
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
			->SetWrapEnabled(true, 3.85f, UIBox::SizeMode::ScreenRelative)
			->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative)
			->SetPadding(5, 5, 0, 0)));

}
