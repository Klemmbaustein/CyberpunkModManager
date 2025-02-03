#pragma once
#include "Popup.h"
#include <ModInfo.h>
#include <mutex>

class ModOptionsSelection : public Popup
{
	bool LoadedMod = true;
	ModInfo TargetMod;
	std::mutex ModLoadMutex;
	void GenerateOptions();
public:

	void LoadMod(ModInfo Mod);

	void Init() override;
	void Update() override;

	std::string GetWindowTitle() override;
	kui::Vec2ui GetWindowResolution() override;
};