#include "FOMODInstall.h"
#include <ModSetup.kui.hpp>
#include <ModEntry.kui.hpp>
#include <iostream>
#include <filesystem>
#include <FileUtil.h>
#include "Game.h"
#include "StrUtil.h"
#include "../WindowsFunctions.h"
#include "Tabs/InstalledModsTab.h"

using namespace kui;

void FOMODInstall::NextInstallGroup(bool Force)
{
	if (!Force)
	{
		for (auto& i : CurrentStep->Groups)
		{
			if (!i.HasValidSelection())
			{
				Windows::ErrorBox("Invalid selection for category: " + i.Name);
				return;
			}
		}
	}

	for (auto& i : CurrentStep->Groups)
	{
		for (auto& j : i.Plugins)
		{
			if (j.Selected)
			{
				j.Enable(InstalledMod);
			}
		}
	}

	UnloadTextures();

	if (StepIndex + 1 >= InstalledMod.Steps.size())
	{
		InstallMod();
		ShouldClose = true;
		return;
	}

	CurrentStep = &InstalledMod.Steps[++StepIndex];

	if (CurrentStep->Visible.Evaluate(InstalledMod))
	{
		Background->GetScrollObject()->Scrolled = 0;
		GenerateUI();
	}
	else
	{
		NextInstallGroup(true);
	}
}

void FOMODInstall::GenerateUI()
{
	Background->DeleteChildren();

	int Index = 0;

	for (auto& i : CurrentStep->Groups)
	{
		GenerateGroup(i, Index);
	}

	auto Footer = new SetupFooter();
	Background->AddChild(Footer);
	Footer->nextButton->OnClicked = [this]() {
		NextInstallGroup(false);
	};
}

void FOMODInstall::UnloadTextures()
{
	for (auto& i : Textures)
	{
		image::UnloadImage(i.ID);
	}
	Textures.clear();
}

void FOMODInstall::GenerateGroup(FOMOD::InstallGroup Group, int& ItemIndex)
{
	auto Section = new ModHeader();

	std::string GroupName = " (" + Group.Type + ")";

	if (Group.Type == "SelectAll")
	{
		GroupName.clear();
	}

	Section->SetName(Group.Name + GroupName);
	Background->AddChild(Section);

	UIBox* Boxes[20];

	for (auto& i : Boxes)
	{
		i = new UIBox(true);
		Section->AddChild(i);
	}

	size_t SlotsPerRow = std::max(int(Window::GetActiveWindow()->GetSize().X / 250 * Window::GetActiveWindow()->GetDPI()), 1);

	size_t PluginIndex = 0;

	for (auto& i : Group.Plugins)
	{
		auto PluginOption = new SetupOption();
		PluginOption->SetName(i.Name);
		PluginOption->SetDescription(StrUtil::ShortenIfTooLong(i.Description, 235));
		if (i.Selected)
		{
			PluginOption->SetColor(Vec3f(0.4f, 0.05f, 0.05f));
		}
		PluginOption->optionButton->OnClicked = [this, &i]() {
			i.Selected = !i.Selected;
			GenerateUI();
			return;
		};

		image::ImageInfo ElementImage;

		if (Textures.size() > ItemIndex)
		{
			ElementImage = Textures[ItemIndex];
		}
		else
		{
			ElementImage = image::LoadImageWithInfo(i.ImageFile);
			Textures.push_back(ElementImage);
		}

		Vec2f Aspect = Vec2f(1, (float)ElementImage.Height / (float)ElementImage.Width);

		Aspect.Y *= 1.5f;

		if (Aspect.Y > 1.0f / (16.0f / 9.0f))
		{
			Aspect.Y = 1 / Aspect.Y;
			std::swap(Aspect.X, Aspect.Y);
		}

		PluginOption->image->SetUseTexture(true, ElementImage.ID);

		if (std::max(Aspect.X, Aspect.Y) > 1)
		{
			Aspect = Aspect / std::max(Aspect.X, Aspect.Y);
		}

		PluginOption->image->SetMinSize(SizeVec(Vec2f(250, 175) * Aspect, SizeMode::PixelRelative));
		PluginOption->image->SetMaxSize(PluginOption->image->GetMinSize());

		Boxes[PluginIndex / SlotsPerRow]->AddChild(PluginOption);
		PluginIndex++;
		ItemIndex++;
		if (PluginIndex / SlotsPerRow >= sizeof(Boxes) / sizeof(UIBox*))
		{
			break;
		}
	}
}

void FOMODInstall::InstallMod()
{
	InstalledMod.Install(ToPath);
	Info.Files = FileUtil::GetAllFilesInFolder(ToPath);

	try
	{
		for (auto& i : Info.Files)
		{
			size_t PathLocation = i.find_last_of("/");
			if (PathLocation != std::string::npos)
			{
				std::filesystem::create_directories(Game::GetGameLocation() + i.substr(0, PathLocation));
			}
			std::filesystem::copy(ToPath + "/" + i, Game::GetGameLocation() + i, std::filesystem::copy_options::overwrite_existing);
		}
		std::filesystem::remove_all(ToPath);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	Info.Enabled = true;
	Info.Save();
	AppTab::GetTabOfType<InstalledModsTab>()->ShouldReload = true;
}

void FOMODInstall::RegisterFOMODCallback()
{
	ModInfo::InstallFOMODCallback = [](ModInfo Info, std::string FromPath, std::string ToPath) {
		auto Install = Popup::CreatePopup<FOMODInstall>();
		Install->LoadModPath(Info, FromPath, ToPath);
	};
}

void FOMODInstall::LoadModPath(ModInfo NewInfo, std::string NewModPath, std::string NewToPath)
{
	std::lock_guard g{ ModPathMutex };

	this->Info = NewInfo;
	this->ModPath = NewModPath;
	this->ToPath = NewToPath;
}

void FOMODInstall::Init()
{
	Background = new UIScrollBox(false, 0, true);
	PopupBackground->AddChild(Background
			->SetMinSize(2)
			->SetMaxSize(2));
}

void FOMODInstall::Update()
{
	std::lock_guard g(ModPathMutex);

	if (!ModPath.empty())
	{
		InstalledMod = FOMOD::Parse(ModPath);

		if (InstalledMod.Steps.empty())
		{
			InstallMod();
			ShouldClose = true;
			return;
		}
		CurrentStep = &InstalledMod.Steps[0];

		GenerateUI();
	}
	ModPath.clear();
}

void FOMODInstall::Destroy()
{
	UnloadTextures();
}

std::string FOMODInstall::GetWindowTitle()
{
	return "Mod setup";
}

Vec2ui FOMODInstall::GetWindowResolution()
{
	return Vec2ui(1100, 800);
}
