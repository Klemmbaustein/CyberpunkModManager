#pragma once
#include "Popup.h"
#include "../Markup/ModInfoWindowElement.hpp"
#include "NexusModsAPI.h"


class ModInfoWindow : public Popup
{
	NexusModsAPI::ModInfo LoadedInfo;
	std::mutex InfoMutex;
	bool HasInfo = false;
	void LoadInfo();
public:
	Vector2ui GetWindowResolution() override;
	
	ModInfoWindowElement* InfoElement = nullptr;

	void SetModInfo(NexusModsAPI::ModInfo Info);
	NexusModsAPI::ModInfo GetModInfo();

	std::string GetWindowTitle() override;
	void Init() override;
	void Update() override;
};