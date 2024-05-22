#pragma once
#include "Popup.h"
#include "../Markup/ModInfoWindowElement.hpp"
#include "NexusModsAPI.h"


class ModInfoWindow : public Popup
{
	NxmAPI::ModInfo LoadedInfo;
	std::mutex InfoMutex;
	bool HasInfo = false;
	void LoadInfo();
	static thread_local ModInfoWindow* CurrentWindow;
	KlemmUI::UIBox* ActionsBox = nullptr;
	void GenerateActionButtons(KlemmUI::UIBox* Parent, const NxmAPI::ModInfo& Mod);
public:
	Vector2ui GetWindowResolution() override;
	
	ModInfoWindowElement* InfoElement = nullptr;

	void SetModInfo(NxmAPI::ModInfo Info);
	NxmAPI::ModInfo GetModInfo();

	std::string GetWindowTitle() override;
	void Init() override;
	void Update() override;

	void RenderMarkupString(std::string Markup, KlemmUI::UIBox* Parent);
	void RenderMarkupLine(std::string Line, bool Centered, KlemmUI::UIBox* Parent);
};