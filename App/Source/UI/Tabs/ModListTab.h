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

	ModListTab(std::string Name);
	void OpenModFromIndex(int Index);

	void LoadImages();
	void LoadMainPage();
	void Generate();

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
	int GetModsPerRow();
	int ModsPerRow = 0;

};