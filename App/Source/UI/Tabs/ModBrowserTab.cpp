#include "ModBrowserTab.h"

#include <KlemmUI/Window.h>
#include <KlemmUI/Rendering/Texture.h>

#include <iostream>
#include <mutex>
#include <filesystem>

#include "StrUtil.h"
#include "Net.h"
#include "../ModInfoWindow.h"

#include "../../Markup/ModEntry.hpp"
#include "../../Markup/ModHeader.hpp"
#include "../../BackgroundTask.h"
#include "../../Webp.h"
using namespace KlemmUI;

static std::vector<ModBrowserTab::ModsSection> LoadedMods;
static std::mutex PageLoadMutex;
static std::mutex ImageLoadMutex;
static bool LoadedNewImage;

void ModBrowserTab::LoadMainPageAsync()
{
	std::lock_guard Guard(PageLoadMutex);
	LoadedMods.clear();
	LoadedMods.push_back(ModsSection{
		.Title = "Trending",
		.Mods = NexusModsAPI::GetMods("trending")
		});
	LoadedMods.push_back(ModsSection{
		.Title = "Last Updated",
		.Mods = NexusModsAPI::GetMods("latest_updated")
		});
	LoadedMods.push_back(ModsSection{
	.Title = "Last Added",
	.Mods = NexusModsAPI::GetMods("latest_added")
		});

}

static void LoadModImageWebP(NexusModsAPI::ModInfo& Mod, std::string FilePath)
{
	auto Buffer = Webp::LoadBuffer(FilePath);
	Mod.ImageBuffer = Buffer.Bytes;
	Mod.ImageWidth = Buffer.Width;
	Mod.ImageHeight = Buffer.Height;
}

static void LoadModImages()
{
	std::filesystem::create_directories("app/temp/images/");

	for (auto& i : LoadedMods)
	{
		for (auto& j : i.Mods)
		{
			std::string ImageFile = j.GetImagePath();

			if (std::filesystem::exists(ImageFile))
			{
				LoadedNewImage = true;
				LoadModImageWebP(j, ImageFile);
				continue;
			}
			Net::GetFile(j.ImageUrl, ImageFile);
			LoadModImageWebP(j, ImageFile);
			std::unique_lock Guard(ImageLoadMutex);
			LoadedNewImage = true;
		}
	}
	std::unique_lock Guard(ImageLoadMutex);
	LoadedNewImage = true;
}

void ModBrowserTab::ShowLoadingScreen()
{
	Images.clear();
	ModsScrollBox->DeleteChildren();
	for (unsigned int tex : LoadedTextures)
	{
		Texture::UnloadTexture(tex);
	}

}

ModBrowserTab::ModBrowserTab()
	: AppTab("Mod browser")
{
	ModsScrollBox = new UIScrollBox(false, 0, true);
	TabBackground->AddChild(ModsScrollBox);
	LoadMainPage();
}

void ModBrowserTab::OpenModFromIndex(int Index)
{
	int it = 0;
	for (const auto& i : LoadedMods)
	{
		for (const auto& m : i.Mods)
		{
			if (it++ == Index)
			{
				Popup::CreatePopup<ModInfoWindow>()->SetModInfo(m);
				return;
			}
		}
	}
}

void ModBrowserTab::LoadMainPage()
{
	ShowLoadingScreen();

	new BackgroundTask(&LoadMainPageAsync, []() {
		new BackgroundTask(LoadModImages);
		GetTabOfType<ModBrowserTab>()->Generate();
		});
}

void ModBrowserTab::Generate()
{
	ShowLoadingScreen();

	size_t ElementIndex = 0;
	for (auto& i : LoadedMods)
	{
		GenerateSection(i, ElementIndex);
	}
}

void ModBrowserTab::GenerateSection(ModsSection Section, size_t& Index)
{
	auto Header = new ModHeader();
	Header->SetName(Section.Title);
	ModsScrollBox->AddChild(Header);

	std::vector<UIBox*> Rows;
	for (int i = 0; i < 20; i++)
	{
		UIBox* Row = new UIBox(true);
		ModsScrollBox->AddChild(Row);
		Rows.push_back(Row);
	}

	int ModsPerPage = (Window::GetActiveWindow()->GetSize().X - 150) / 200;

	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		auto Entry = new ModEntry();
		Entry->SetName(StrUtil::ShortenIfTooLong(Section.Mods[i].Name, 40));
		Entry->SetDescription(StrUtil::ShortenIfTooLong(Section.Mods[i].Summary, 165));
		Entry->SetInfo("Downloads: " + std::to_string(Section.Mods[i].Downloads));
		Entry->button->ButtonIndex = Index++;
		Entry->button->OnClickedFunctionIndex = &OpenModFromIndex;

		Images.push_back(Entry->imageBackground);
		Rows[i / ModsPerPage]->AddChild(Entry);
	}

}

void ModBrowserTab::UpdateImages()
{
	std::unique_lock Guard(ImageLoadMutex);
	size_t ImageIndex = 0;
	for (auto& i : LoadedMods)
	{
		for (auto& j : i.Mods)
		{
			if (j.ImageBuffer && !Images[ImageIndex]->HasTexture())
			{
				unsigned int LoadedWebp = Webp::Load(Webp::WebpBuffer{
					.Bytes = j.ImageBuffer,
					.Width = j.ImageWidth,
					.Height = j.ImageHeight,
					});
				LoadedTextures.push_back(LoadedWebp);
				j.ImageBuffer = nullptr;
				UIBackground* ImageBackground = Images[ImageIndex];

				ImageBackground
					->SetUseTexture(true, LoadedWebp)
					->SetColor(1)
					->SetMinSize(Vector2f(120.0f * ((float)j.ImageWidth / (float)j.ImageHeight), 120.0f));
				ImageBackground->DeleteChildren();
			}
			ImageIndex++;
		}
	}
}

void ModBrowserTab::Update()
{
	ModsScrollBox->SetMinSize(TabBackground->GetMinSize());
	ModsScrollBox->SetMaxSize(TabBackground->GetMinSize());

	if (LoadedNewImage)
	{
		UpdateImages();
		LoadedNewImage = false;
	}
}

void ModBrowserTab::OnResized()
{
	LoadMainPage();
}
