#pragma once
#include "AppTab.h"
#include <kui/UI/UIScrollBox.h>
#include "NexusModsAPI.h"

class ModListTab : public AppTab
{
	std::vector<kui::UIBackground*> Images;
	std::vector<unsigned int> LoadedTextures;
	bool LoadedNewImage = false;
	bool IsLoadingList = false;
protected:

	void LoadSection(std::vector<NxmAPI::ModInfo> Mods, std::string Title);
	void ClearContent();
	void ShowLoadingScreen();

	kui::UIScrollBox* ModsScrollBox = nullptr;
	kui::UIBox* HeaderBox = nullptr;
	kui::UIBox* ContentBox = nullptr;
public:
	bool ShouldReload = false;
	bool ShouldUpdateList = false;

	enum class ViewMode
	{
		Tiles,
		List,
	};

	bool CanSelectMods = false;

	std::set<std::string> SelectedMods;

	ViewMode CurrentViewMode = ViewMode::Tiles;

	ModListTab(std::string Name);
	void OpenModFromIndex(int Index);

	void LoadImages();
	void LoadMainPage();
	void Generate();

	virtual void OnGenerated();
	virtual void OnClicked(int Index);

	struct ModsSection
	{
		std::string Title;
		std::vector<NxmAPI::ModInfo> Mods;
	};
	std::vector<ModListTab::ModsSection> LoadedMods;

	virtual std::string GetModImage(NxmAPI::ModInfo Mod);

	void GenerateSection(ModsSection Section, size_t& Index);
	void UpdateImages();
	virtual void LoadSections();
	virtual void Update() override;
	virtual void OnResized() override;

private:
	size_t FirstSelected = SIZE_MAX;
	bool HandleSelect(bool IsSelected, size_t Index, ModsSection Section);
	void SelectMod(std::string Name);

	void GenerateSectionTiles(ModsSection Section, size_t& Index);
	void GenerateSectionList(ModsSection Section, size_t& Index);

	int GetModsPerRow();
	int ModsPerRow = 0;

};