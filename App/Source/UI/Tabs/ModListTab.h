#pragma once
#include "AppTab.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include "NexusModsAPI.h"

class ModListTab : public AppTab
{
	static void LoadMainPageAsync();
	std::vector<KlemmUI::UIBackground*> Images;
	std::vector<unsigned int> LoadedTextures;

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

	virtual std::string GetModImage(NexusModsAPI::ModInfo Mod);
	virtual void LoadPageAsync();

	void GenerateSection(ModsSection Section, size_t& Index);
	void UpdateImages();
	virtual void LoadSections();
	virtual void Update() override;
	virtual void OnResized() override;

	KlemmUI::UIScrollBox* ModsScrollBox = nullptr;

};