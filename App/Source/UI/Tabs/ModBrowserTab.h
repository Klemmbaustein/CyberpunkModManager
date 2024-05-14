#pragma once
#include "AppTab.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include "NexusModsAPI.h"
#include <KlemmUI/Input.h>

class ModBrowserTab : public AppTab
{
	static void LoadMainPageAsync();
	std::vector<KlemmUI::UIBackground*> Images;
	std::vector<unsigned int> LoadedTextures;

	void ShowLoadingScreen();
public:
	ModBrowserTab();
	static void OpenModFromIndex(int Index);

	void LoadMainPage();
	void Generate();

	struct ModsSection
	{
		std::string Title;
		std::vector<NexusModsAPI::ModInfo> Mods;
	};

	void GenerateSection(ModsSection Section, size_t& Index);
	void UpdateImages();
	virtual void Update() override;
	virtual void OnResized() override;

	KlemmUI::UIScrollBox* ModsScrollBox = nullptr;
};