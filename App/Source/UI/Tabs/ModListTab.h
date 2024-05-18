#pragma once
#include "AppTab.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include "NexusModsAPI.h"

class ModListTab : public AppTab
{
	std::vector<KlemmUI::UIBackground*> Images;
	std::vector<unsigned int> LoadedTextures;
	bool LoadedNewImage = false;
	bool IsLoadingList = false;
protected:

	void LoadSection(std::vector<NexusModsAPI::ModInfo> Mods, std::string Title);
	void ShowLoadingScreen();
public:
	bool ShouldReload = false;

	ModListTab();
	void OpenModFromIndex(int Index);

	void LoadImages();
	void LoadMainPage();
	void Generate();

	void (*OnButtonClickedFunction)(int Index) = nullptr;

	struct ModsSection
	{
		std::string Title;
		std::vector<NexusModsAPI::ModInfo> Mods;
	};
	std::vector<ModListTab::ModsSection> LoadedMods;

	virtual std::string GetModImage(NexusModsAPI::ModInfo Mod);

	void GenerateSection(ModsSection Section, size_t& Index);
	void UpdateImages();
	virtual void LoadSections();
	virtual void Update() override;
	virtual void OnResized() override;

	KlemmUI::UIScrollBox* ModsScrollBox = nullptr;

};