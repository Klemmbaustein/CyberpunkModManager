#include "ModListTab.h"
#include "../../Webp.h"
#include <KlemmUI/Rendering/Texture.h>
#include "../ModInfoWindow.h"
#include <filesystem>
#include <Net.h>
#include "../../Markup/ModEntry.hpp"
#include "../../Markup/ModHeader.hpp"
#include "../../BackgroundTask.h"
#include <StrUtil.h>
using namespace KlemmUI;

static std::vector<ModListTab::ModsSection> LoadedMods;
static std::mutex PageLoadMutex;
static std::mutex ImageLoadMutex;
static bool LoadedNewImage;

static ModListTab* CurrentReloadedPage = nullptr;
void ModListTab::LoadMainPageAsync()
{
	std::lock_guard Guard(PageLoadMutex);
	LoadedMods.clear();

	CurrentReloadedPage->LoadSections();
}

static void LoadModImageWebP(NexusModsAPI::ModInfo& Mod, std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		return;
	}
	auto Buffer = Webp::LoadBuffer(FilePath);
	Mod.ImageBuffer = Buffer.Bytes;
	Mod.ImageWidth = Buffer.Width;
	Mod.ImageHeight = Buffer.Height;
}

void ModListTab::LoadSection(std::vector<NexusModsAPI::ModInfo> Mods, std::string Title)
{
	LoadedMods.push_back(ModsSection{
		.Title = Title,
		.Mods = Mods
	});
}

void ModListTab::ShowLoadingScreen()
{
	Images.clear();
	ModsScrollBox->DeleteChildren();
	for (unsigned int tex : LoadedTextures)
	{
		Texture::UnloadTexture(tex);
	}
}

ModListTab::ModListTab()
	: AppTab("abc")
{
	ModsScrollBox = new UIScrollBox(false, 0, true);
	TabBackground->AddChild(ModsScrollBox);
	LoadMainPage();

}

void ModListTab::OpenModFromIndex(int Index)
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

void ModListTab::LoadImages()
{
	for (auto& i : LoadedMods)
	{
		for (auto& j : i.Mods)
		{
			LoadModImageWebP(j, GetModImage(j));
			std::unique_lock Guard(ImageLoadMutex);
			LoadedNewImage = true;
		}
	}
	std::unique_lock Guard(ImageLoadMutex);
	LoadedNewImage = true;
}

void ModListTab::LoadMainPage()
{
	ShowLoadingScreen();

	CurrentReloadedPage = this;
	new BackgroundTask(&LoadMainPageAsync, []() {
		new BackgroundTask([]()
			{
				GetTabOfType<ModListTab>()->LoadImages();
			});
		GetTabOfType<ModListTab>()->Generate();
		});
}

void ModListTab::Generate()
{
	ShowLoadingScreen();

	size_t ElementIndex = 0;
	for (auto& i : LoadedMods)
	{
		GenerateSection(i, ElementIndex);
	}
}

std::string ModListTab::GetModImage(NexusModsAPI::ModInfo Mod)
{
	std::filesystem::create_directories("app/temp/images/");
	std::string ImageFile = Mod.GetImagePath();

	if (std::filesystem::exists(ImageFile))
	{
		LoadModImageWebP(Mod, ImageFile);
		return ImageFile;
	}
	Net::GetFile(Mod.ImageUrl, ImageFile);

	return ImageFile;
}

void ModListTab::LoadPageAsync()
{
	LoadedMods.push_back(ModsSection{
		.Title = "Trending",
		.Mods = NexusModsAPI::GetMods("trending")
		});
	LoadedMods.push_back(ModsSection{
		.Title = "Last Updated",
		.Mods = NexusModsAPI::GetMods("latest_updated")
		});
}

void ModListTab::GenerateSection(ModsSection Section, size_t& Index)
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

	int ModsPerPage = (Window::GetActiveWindow()->GetSize().X - 150 * Window::GetActiveWindow()->GetDPI()) / (200 * Window::GetActiveWindow()->GetDPI());

	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		auto Entry = new ModEntry();
		Entry->SetName(StrUtil::ShortenIfTooLong(Section.Mods[i].Name, 40));
		Entry->SetDescription(StrUtil::ShortenIfTooLong(Section.Mods[i].Summary, 165));
		Entry->SetInfo("Downloads: " + std::to_string(Section.Mods[i].Downloads));
		Entry->button->ButtonIndex = Index++;
		Entry->button->OnClickedFunctionIndex = OnButtonClickedFunction;

		Images.push_back(Entry->imageBackground);
		Rows[i / ModsPerPage]->AddChild(Entry);
	}
}

void ModListTab::UpdateImages()
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

void ModListTab::LoadSections()
{
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

void ModListTab::Update()
{
	ModsScrollBox->SetMinSize(TabBackground->GetMinSize());
	ModsScrollBox->SetMaxSize(TabBackground->GetMinSize());

	if (ShouldReload)
	{
		ShouldReload = false;
		LoadMainPage();
	}

	if (LoadedNewImage)
	{
		UpdateImages();
		LoadedNewImage = false;
	}
}

void ModListTab::OnResized()
{
	LoadMainPage();
}
