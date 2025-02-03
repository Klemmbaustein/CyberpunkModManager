#include "ModListTab.h"
#include "../../Webp.h"
#include <kui/Image.h>
#include "../ModInfoWindow.h"
#include <filesystem>
#include <Net.h>
#include "FileUtil.h"
#include <ModEntry.kui.hpp>
#include "../../BackgroundTask.h"
#include <StrUtil.h>
#include <iostream>
#include "../../WindowsFunctions.h"
using namespace kui;

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
		image::UnloadImage(tex);
	}
	LoadedTextures.clear();
}

void ModListTab::ShowLoadingScreen()
{
	ClearContent();

	ContentBox->SetVerticalAlign(UIBox::Align::Centered);
	ContentBox->SetHorizontalAlign(UIBox::Align::Centered);
	ContentBox->AddChild((new UIText(20_px, 1, "Loading...", UI::Text))
		->SetPadding(0.8f));
}

ModListTab::ModListTab(std::string Name)
	: AppTab(Name)
{
	ModsScrollBox = new UIScrollBox(false, 0, true);

	HeaderBox = new UIBox(true);
	ContentBox = new UIBox(false);
	ContentBox->SetMinWidth(UISize::Parent(1));
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
	new BackgroundTask([this]()
		{
			std::lock_guard Guard(PageLoadMutex);
			LoadedMods.clear();
			LoadSections();
		},
		[this]()
		{
			Generate();
			new BackgroundTask([this]()
				{
					LoadImages();
					IsLoadingList = false;
				});
		});
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

void ModListTab::OnClicked(int Index)
{
	OpenModFromIndex(Index);
}

const size_t MAX_IMAGE_TEMP_SIZE = 1000 * 1000 * 20;

std::string ModListTab::GetModImage(NxmAPI::ModInfo Mod)
{
	if (FileUtil::GetPathSize("app/temp/images") > MAX_IMAGE_TEMP_SIZE)
	{
		std::filesystem::remove_all("app/temp/images");
	}

	std::filesystem::create_directories("app/temp/images/");
	std::string ImageFile = Mod.GetImagePath();

	if (std::filesystem::exists(ImageFile))
	{
		return ImageFile;
	}
	Net::GetFile(Mod.ImageUrl, ImageFile, true);

	return ImageFile;
}

static Vec3f InfoTextColors[] =
{
	// Red
	Vec3f(1, 0, 0.1f),
	// Green
	Vec3f(0.1f, 1, 0.0f),
	// Yellow
	Vec3f(1, 1, 0.1f),
	// Grey
	Vec3f(0.65f),
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

	int ModsPerPage = int((Window::GetActiveWindow()->GetSize().X - 150 * Window::GetActiveWindow()->GetDPI()) / (200.0f * Window::GetActiveWindow()->GetDPI()));

	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		auto Entry = new ModEntry();
		Entry->SetName(Section.Mods[i].Name);
		Entry->SetDescription(StrUtil::Replace(Section.Mods[i].Summary, "&amp;", "&"));
		Entry->SetInfo(Section.Mods[i].InfoString);
		Entry->button->OnClicked = std::bind(&ModListTab::OnClicked, this, (int)Index);
		Index++;
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
					->SetMinSize(Vec2f(120.0f * ((float)j.ImageWidth / (float)j.ImageHeight), 120.0f));
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
