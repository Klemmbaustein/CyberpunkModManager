#pragma once
#include "FOMOD.h"
#include "Popup.h"
#include <mutex>
#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/Rendering/Texture.h>
#include <ModInfo.h>

class FOMODInstall : public Popup
{
	FOMOD InstalledMod;
	FOMOD::InstallStep* CurrentStep = nullptr;
	std::string ModPath;
	std::string ToPath;
	ModInfo Info;
	std::vector<KlemmUI::Texture::TextureInfo> Textures;

	KlemmUI::UIScrollBox* Background = nullptr;
	void NextInstallGroup(bool Force);
	void GenerateUI();
	void UnloadTextures();
	void GenerateGroup(FOMOD::InstallGroup Group, int& ItemIndex);

	void InstallMod();

	int StepIndex = 0;

public:
	static void RegisterFOMODCallback();

	std::mutex ModPathMutex;

	void LoadModPath(ModInfo Info, std::string ModPath, std::string ToPath);

	void Init() override;
	void Update() override;
	void Destroy() override;
	std::string GetWindowTitle() override;
	Vector2ui GetWindowResolution() override;
};