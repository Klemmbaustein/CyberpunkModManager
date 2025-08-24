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
static std::mutex LoadedModsMutex;
static std::mutex ImageListMutex;

std::map<std::string, Webp::WebpBuffer> Images;

static void LoadModImageWebP(NxmAPI::ModInfo& Mod, std::string FilePath)
{
	if (!std::filesystem::exists(FilePath))
	{
		return;
	}
	auto Buffer = Webp::LoadBuffer(FilePath);
	std::unique_lock Guard(ImageLoadMutex);
	std::unique_lock ListGuard(ImageListMutex);
	Mod.ImageBuffer = Buffer.Bytes;
	Mod.ImageWidth = Buffer.Width;
	Mod.ImageHeight = Buffer.Height;
	Images.insert({FilePath, Buffer});
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
		->SetScrollSpeed(30)
		->AddChild(HeaderBox)
		->AddChild(ContentBox));
}

void ModListTab::OpenModFromIndex(int Index)
{
	std::unique_lock Guard(LoadedModsMutex);
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
	std::vector<ModsSection> ModsCopy;
	{
		std::unique_lock Guard(LoadedModsMutex);
		ModsCopy = LoadedMods;
	}
	for (size_t i = 0; i < ModsCopy.size(); i++)
	{
		std::cout << "Loading " << ModsCopy[i].Mods.size() << " images..." << std::endl;
		for (size_t j = 0; j < ModsCopy[i].Mods.size(); j++)
		{
			if (LoadedMods.size() != ModsCopy.size()
				|| ModsCopy[i].Mods.size() != LoadedMods[i].Mods.size())
			{
				return;
			}
			LoadModImageWebP(LoadedMods[i].Mods[j], GetModImage(ModsCopy[i].Mods[j]));
			LoadedNewImage = true;
		}
	}
	std::unique_lock Guard(ImageLoadMutex);
	LoadedNewImage = true;
}

void ModListTab::LoadMainPage()
{
	ModsPerRow = GetModsPerRow();
	std::unique_lock Guard(LoadedModsMutex);
	std::unique_lock ImageGuard(ImageLoadMutex);
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

	for (unsigned int tex : LoadedTextures)
	{
		image::UnloadImage(tex);
	}
	LoadedTextures.clear();

	LoadedMods.clear();
	new BackgroundTask([this]()
		{
			std::unique_lock Guard(LoadedModsMutex);
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
	std::unique_lock Guard(LoadedModsMutex);
	ClearContent();

	size_t ElementIndex = 0;
	for (auto& i : LoadedMods)
	{
		GenerateSection(i, ElementIndex);
	}
	OnGenerated();
}

void ModListTab::OnGenerated()
{
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

	switch (CurrentViewMode)
	{
	case ModListTab::ViewMode::List:
		GenerateSectionList(Section, Index);
		break;
	case ModListTab::ViewMode::Tiles:
	default:
		GenerateSectionTiles(Section, Index);
		break;
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
			if (j.ImageBuffer && !Images[ImageIndex]->HasImage())
			{
				unsigned int LoadedWebp = Webp::Load(Webp::WebpBuffer{
					.Bytes = j.ImageBuffer,
					.Width = j.ImageWidth,
					.Height = j.ImageHeight,
					});
				LoadedTextures.push_back(LoadedWebp);
				j.ImageBuffer = nullptr;
				j.ModImageId = LoadedWebp;
				UIBackground* ImageBackground = Images[ImageIndex];

				ImageBackground
					->SetUseTexture(true, LoadedWebp)
					->SetColor(1)
					->SetMinSize(Vec2f(120.0f * ((float)j.ImageWidth / (float)j.ImageHeight), 120.0f));
				ImageBackground->DeleteChildren();
			}
			else if (j.ModImageId && !Images[ImageIndex]->HasImage())
			{
				UIBackground* ImageBackground = Images[ImageIndex];
				ImageBackground
					->SetUseTexture(true, j.ModImageId)
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

	auto Hovered = Window::GetActiveWindow()->UI.HoveredBox;

	if (SelectedMods.size()
		&& Window::GetActiveWindow()->Input.IsLMBClicked
		&& Hovered
		&& !Hovered->IsChildOf(ModsScrollBox))
	{
		SelectedMods.clear();
		ShouldUpdateList = true;
		FirstSelected = SIZE_MAX;
	}

	if (ShouldReload && !IsLoadingList && TabBackground->IsVisible)
	{
		IsLoadingList = true;
		ShouldReload = false;
		LoadMainPage();
	}

	if (LoadedNewImage && TabBackground->IsVisible)
	{
		LoadedNewImage = false;
		UpdateImages();
	}

	if (ShouldUpdateList && !IsLoadingList && TabBackground->IsVisible)
	{
		Generate();
		UpdateImages();
		ShouldUpdateList = false;
	}
}

void ModListTab::OnResized()
{
	if (ModsPerRow != GetModsPerRow() || CurrentViewMode == ViewMode::List)
	{
		ModsPerRow = GetModsPerRow();
		ShouldUpdateList = true;
	}
}

bool ModListTab::HandleSelect(bool IsSelected, size_t Index, ModsSection Section)
{
	if (CanSelectMods
		&& (Window::GetActiveWindow()->Input.IsKeyDown(Key::LCTRL)
		|| Window::GetActiveWindow()->Input.IsKeyDown(Key::RCTRL)))
	{
		auto& Mod = Section.Mods[Index];

		if (SelectedMods.contains(Mod.Name))
			SelectedMods.erase(Mod.Name);
		else
			SelectedMods.insert(Mod.Name);
		FirstSelected = Index;
		ShouldUpdateList = true;
		return false;
	}
	else if (CanSelectMods
		&& (Window::GetActiveWindow()->Input.IsKeyDown(Key::LSHIFT)
		|| Window::GetActiveWindow()->Input.IsKeyDown(Key::RSHIFT)))
	{
		size_t StartIndex = FirstSelected;

		if (StartIndex == SIZE_MAX)
		{
			StartIndex = Index;
		}

		if (StartIndex > Index)
		{
			std::swap(StartIndex, Index);
		}

		for (size_t i = StartIndex; i <= Index; i++)
		{
			if (IsSelected)
				SelectedMods.erase(Section.Mods[i].Name);
			else
				SelectedMods.insert(Section.Mods[i].Name);
		}
		FirstSelected = Index;
		ShouldUpdateList = true;
		return false;
	}
	return true;
}

void ModListTab::SelectMod(std::string Name)
{
	if (SelectedMods.contains(Name))
		SelectedMods.erase(Name);
	else
		SelectedMods.insert(Name);
	ShouldUpdateList = true;
}

void ModListTab::GenerateSectionTiles(ModsSection Section, size_t& Index)
{
	std::vector<UIBox*> Rows;
	for (int i = 0; i < Section.Mods.size() / ModsPerRow + 1; i++)
	{
		UIBox* Row = new UIBox(true);
		ContentBox->AddChild(Row);
		Rows.push_back(Row);
	}

	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		bool IsSelected = SelectedMods.contains(Section.Mods[i].Name);

		auto Entry = new ModEntry();
		Entry->SetName(Section.Mods[i].Name);

		std::string InfoString = Section.Mods[i].InfoString;

		if (IsSelected)
		{
			InfoString.append(" (Selected)");
			Entry->SetColor(Vec3f(0.2f, 0.1f, 0.1f));
			Entry->SetCheckedImage("res:icons/mod_checkbox_1.png");
		}

		if (!CanSelectMods)
		{
			Entry->checkButton->IsVisible = false;
		}

		Entry->SetDescription(StrUtil::Replace(Section.Mods[i].Summary, "&amp;", "&"));
		Entry->SetInfo(InfoString);

		Entry->button->OnClicked = [this, IsSelected, Section, Index = Index]()
			{
				if (HandleSelect(IsSelected, Index, Section))
					ModListTab::OnClicked(Index);
			};

		Entry->checkButton->OnClicked = std::bind(&ModListTab::SelectMod, this, Section.Mods[i].Name);

		Index++;
		Entry->infoText->SetColor(InfoTextColors[Section.Mods[i].InfoColor]);

		Images.push_back(Entry->imageBackground);
		Rows[i / ModsPerRow]->AddChild(Entry);
	}
}

void ModListTab::GenerateSectionList(ModsSection Section, size_t& Index)
{
	for (size_t i = 0; i < Section.Mods.size(); i++)
	{
		bool IsSelected = SelectedMods.contains(Section.Mods[i].Name);

		auto Entry = new ModListEntry();
		std::string InfoString = Section.Mods[i].InfoString;

		if (IsSelected)
		{
			InfoString.append(" (Selected)");
			Entry->SetColor(Vec3f(0.2f, 0.1f, 0.1f));
			Entry->SetCheckedImage("res:icons/mod_checkbox_1.png");
		}

		Entry->SetName(Section.Mods[i].Name);
		Entry->SetDescription(StrUtil::Replace(Section.Mods[i].Summary, "&amp;", "&"));
		Entry->SetInfo(Section.Mods[i].InfoString);

		Entry->button->OnClicked = [this, IsSelected, Section, Index = Index]()
			{
				if (HandleSelect(IsSelected, Index, Section))
					ModListTab::OnClicked(Index);
			};
		Entry->checkButton->OnClicked = std::bind(&ModListTab::SelectMod, this, Section.Mods[i].Name);

		Entry->SetWidth(UISize::Pixels(Window::GetActiveWindow()->GetSize().X - 450));
		Index++;
		Entry->infoText->SetColor(InfoTextColors[Section.Mods[i].InfoColor]);

		Images.push_back(Entry->imageBackground);
		ContentBox->AddChild(Entry);
	}
}

int ModListTab::GetModsPerRow()
{
	return std::max(int((Window::GetActiveWindow()->GetSize().X - 150 * Window::GetActiveWindow()->GetDPI()) / (200.0f * Window::GetActiveWindow()->GetDPI())), 1);
}
