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
#include <iostream>
using namespace KlemmUI;

static std::mutex ImageLoadMutex;
static std::mutex PageLoadMutex;

static void LoadModImageWebP(NxmAPI::ModInfo& Mod, std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		return;
	}
	auto Buffer = Webp::LoadBuffer(FilePath);
	std::unique_lock Guard(ImageLoadMutex);
	Mod.ImageBuffer = Buffer.Bytes;
	Mod.ImageWidth = Buffer.Width;
	Mod.ImageHeight = Buffer.Height;
}

void ModListTab::LoadSection(std::vector<NxmAPI::ModInfo> Mods, std::string Title)
{
	LoadedMods.push_back(ModsSection{
		.Title = Title,
		.Mods = Mods
	});
}

void ModListTab::ClearContent()
{
	Images.clear();
	ContentBox->DeleteChildren();
	ContentBox->SetVerticalAlign(UIBox::Align::Reverse);
	ContentBox->SetHorizontalAlign(UIBox::Align::Default);
	for (unsigned int tex : LoadedTextures)
	{
		Texture::UnloadTexture(tex);
	}
	LoadedTextures.clear();
}

void ModListTab::ShowLoadingScreen()
{
	ClearContent();
	
	ContentBox->SetVerticalAlign(UIBox::Align::Centered);
	ContentBox->SetHorizontalAlign(UIBox::Align::Centered);
	ContentBox->AddChild((new UIText(20, 1, "Loading...", UI::Text))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetPadding(0.8f));
}

ModListTab::ModListTab()
	: AppTab("abc")
{
	ModsScrollBox = new UIScrollBox(false, 0, true);

	HeaderBox = new UIBox(true);
	ContentBox = new UIBox(false);
	TabBackground->AddChild(ModsScrollBox
		->AddChild(HeaderBox)
		->AddChild(ContentBox));
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
			LoadedNewImage = true;
		}
	}
	std::unique_lock Guard(ImageLoadMutex);
	LoadedNewImage = true;
}

void ModListTab::LoadMainPage()
{
	ShowLoadingScreen();
	for (auto& i : LoadedMods)
	{
		for (auto& j : i.Mods)
		{
			if (j.ImageBuffer)
			{
				delete[] j.ImageBuffer;
				j.ImageBuffer = nullptr;
			}
		}
	}

	LoadedMods.clear();
	new BackgroundTask([](void* Data)
		{
			ModListTab* Tab = static_cast<ModListTab*>(Data);
			std::lock_guard Guard(PageLoadMutex);
			Tab->LoadedMods.clear();
			Tab->LoadSections();
		},
		
		[](void* Data) {
			ModListTab* Tab = static_cast<ModListTab*>(Data);
			Tab->Generate();
			new BackgroundTask([](void* Data)
			{
				ModListTab* Tab = static_cast<ModListTab*>(Data);
				Tab->LoadImages();
				Tab->IsLoadingList = false;
			},nullptr, Data);
		}, this);
}

void ModListTab::Generate()
{
	ClearContent();

	size_t ElementIndex = 0;
	for (auto& i : LoadedMods)
	{
		GenerateSection(i, ElementIndex);
	}
}

std::string ModListTab::GetModImage(NxmAPI::ModInfo Mod)
{
	std::filesystem::create_directories("app/temp/images/");
	std::string ImageFile = Mod.GetImagePath();

	if (std::filesystem::exists(ImageFile))
	{
		return ImageFile;
	}
	Net::GetFile(Mod.ImageUrl, ImageFile);

	return ImageFile;
}

static Vector3f InfoTextColors[] =
{
	// Red
	Vector3f(1, 0, 0.1f),
	// Green
	Vector3f(0.1f, 1, 0.0f),
	// Yellow
	Vector3f(1, 1, 0.1f),
	// Grey
	Vector3f(0.65f),
};

void ModListTab::GenerateSection(ModsSection Section, size_t& Index)
{
	auto Header = new ModHeader();
	Header->SetName(Section.Title);
	ContentBox->AddChild(Header);

	std::vector<UIBox*> Rows;
	for (int i = 0; i < 20; i++)
	{
		UIBox* Row = new UIBox(true);
		ContentBox->AddChild(Row);
		Rows.push_back(Row);
	}

	int ModsPerPage = (Window::GetActiveWindow()->GetSize().X - 150 * Window::GetActiveWindow()->GetDPI()) / (200 * Window::GetActiveWindow()->GetDPI());

	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		auto Entry = new ModEntry();
		Entry->SetName(StrUtil::ShortenIfTooLong(Section.Mods[i].Name, 40));
		Entry->SetDescription(StrUtil::ShortenIfTooLong(Section.Mods[i].Summary, 165));
		Entry->SetInfo(Section.Mods[i].InfoString);
		Entry->button->ButtonIndex = Index++;
		Entry->button->OnClickedFunctionIndex = OnButtonClickedFunction;
		Entry->infoText->SetColor(InfoTextColors[Section.Mods[i].InfoColor]);

		Images.push_back(Entry->imageBackground);
		Rows[i / ModsPerPage]->AddChild(Entry);
	}
}

void ModListTab::UpdateImages()
{
	std::lock_guard Guard(ImageLoadMutex);
	size_t ImageIndex = 0;
	for (auto& i : LoadedMods)
	{
		for (auto& j : i.Mods)
		{
			if (Images.size() <= ImageIndex)
			{
				if (j.ImageBuffer)
				{
					delete[] j.ImageBuffer;
					j.ImageBuffer = nullptr;
				}
				break;
			}
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
}

void ModListTab::Update()
{
	ModsScrollBox->SetMinSize(TabBackground->GetMinSize());
	ModsScrollBox->SetMaxSize(TabBackground->GetMinSize());

	if (ShouldReload && !IsLoadingList)
	{
		IsLoadingList = true;
		ShouldReload = false;
		LoadMainPage();
	}

	if (LoadedNewImage)
	{
		LoadedNewImage = false;
		UpdateImages();
	}
}

void ModListTab::OnResized()
{
	ShouldReload = true;
}
