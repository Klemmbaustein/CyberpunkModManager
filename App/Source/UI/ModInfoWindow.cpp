#include "ModInfoWindow.h"
#include <kui/UI/UIScrollBox.h>
#include <kui/Image.h>
#include "../Webp.h"
#include "Net.h"
#include "UI.h"
#include <Common.kui.hpp>
#include "ModInfo.h"
#include <filesystem>
#include <iostream>
#include "ModOptionSelection.h"
#include "../WindowsFunctions.h"
#include "Tabs/InstalledModsTab.h"

using namespace kui;

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

void ModInfoWindow::GenerateActionButtons(kui::UIBox* Parent, const NxmAPI::ModInfo& Mod)
{
	Parent->DeleteChildren();
	ActionsBox = Parent;

	// If the mod isn't installed, an empty value is returned.
	auto ModInf = ModInfo::GetModByName(Mod.Name);

	if (ModInf.Name.empty())
	{
		auto* InstallButton = new AppButton();
		InstallButton->SetText("Install");
		InstallButton->button->OnClicked = [this]() {
			// Can't actually install anything since NexusMods is the worst thing ever.
			OpenModInBrowser(GetModInfo(), "?tab=files");
			};
		Parent->AddChild(InstallButton);
		InstallButton->SetImage("res:icons/download.png");
	}
	else
	{
		if (Mod.InfoColor == NxmAPI::ModInfo::Yellow)
		{
			auto* UpdateButton = new AppButton();
			UpdateButton->SetText("Update");
			UpdateButton->button->OnClicked = [this]() {
				OpenModInBrowser(GetModInfo(), "?tab=files");
				ShouldClose = true;
				};
			Parent->AddChild(UpdateButton);
			UpdateButton->SetImage("res:icons/download.png");
		}

		auto* EnableButton = new AppButton();
		EnableButton->SetText(ModInf.Enabled ? "Disable" : "Enable");
		EnableButton->button->OnClicked = [this]() {
			auto Mod = ModInfo::GetModByName(GetModInfo().Name);

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
					ShouldClose = true;
					return;
				}
				else
				{
					Mod.Enable();
				}
			}

			GenerateActionButtons(ActionsBox, GetModInfo());
			AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};

		EnableButton->SetImage(ModInf.Enabled ? "res:icons/disabled.png" : "res:icons/enabled.png");

		Parent->AddChild(EnableButton);

		auto* RemoveButton = new AppButton();
		RemoveButton->SetText("Uninstall");
		RemoveButton->button->OnClicked = [this]() {
			auto Mod = ModInfo::GetModByName(GetModInfo().Name);
			Mod.Remove();
			GenerateActionButtons(ActionsBox, GetModInfo());
			AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
			};
		RemoveButton->SetImage("res:icons/delete.png");
		Parent->AddChild(RemoveButton);

	}

	if (ModInf.ModID != 0 || Mod.ModID != 0)
	{
		auto* OpenInBrowserButton = new AppButton();
		OpenInBrowserButton->SetText("Open website");
		OpenInBrowserButton->button->OnClicked = [this]() {
			OpenModInBrowser(GetModInfo());
			};
		OpenInBrowserButton->SetImage("res:icons/open.png");
		Parent->AddChild(OpenInBrowserButton);
	}

}

Vec2ui ModInfoWindow::GetWindowResolution()
{
	return Vec2ui(800, 700);
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
	PopupBackground->SetHorizontalAlign(UIBox::Align::Centered);
	PopupBackground->SetVerticalAlign(UIBox::Align::Centered);
	PopupBackground->AddChild(new UIText(15_px, 1, "Loading...", UI::Text));

}

void ModInfoWindow::Update()
{
	if (HasInfo)
	{
		HasInfo = false;
		LoadInfo();
	}
}

void ModInfoWindow::Destroy()
{
	for (auto Image : LoadedImages)
	{
		image::UnloadImage(Image);
	}
	LoadedImages.clear();
}

void ModInfoWindow::RenderMarkupString(std::string Markup, kui::UIBox* Parent)
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
				std::filesystem::create_directories("app/temp/preview_img.png");
				Net::GetFile(TagContent, "app/temp/img.png", false);

				image::ImageInfo Tex = image::LoadImageWithInfo("app/temp/img.png");
				if (Tex.Width != 0 && Tex.Height != 0)
				{
					float SizeScale = std::min(Tex.Width / 400.0f, 1.95f);

					Parent->AddChild((new UIBox(true))
						->SetHorizontalAlign(Centered ? UIBox::Align::Centered : UIBox::Align::Default)
						->AddChild((new UIBackground(true, 0, 1, SizeVec(Vec2f(SizeScale) * Vec2f(1, (float)Tex.Height / (float)Tex.Width), SizeMode::AspectRelative)))
							->SetUseTexture(true, Tex.ID)));
					LoadedImages.push_back(Tex.ID);
				}
				std::filesystem::remove("app/temp/preview_img.png");
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

void ModInfoWindow::RenderMarkupLine(std::string Line, bool Centered, kui::UIBox* Parent)
{
	// No.
	if (Line.find("Bitcoin") != std::string::npos
		|| Line.find("Dogecoin") != std::string::npos
		|| Line.find("Ethereum") != std::string::npos)
	{
		return;
	}

	Parent->AddChild((new UIBox(true))
			->SetMinWidth(UISize::Parent(1))
		->SetHorizontalAlign(Centered ? UIBox::Align::Centered : UIBox::Align::Default)
		->AddChild((new UIText(11_px, 1, Line, UI::Text))
			->SetWrapEnabled(true, 1.9f)
			->SetPadding(5_px, 5_px, 0, 0)));

}
