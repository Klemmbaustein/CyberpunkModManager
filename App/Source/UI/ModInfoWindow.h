#pragma once
#include "Popup.h"
#include <ModInfoWindow.kui.hpp>
#include "NexusModsAPI.h"


class ModInfoWindow : public Popup
{
	NxmAPI::ModInfo LoadedInfo;
	std::mutex InfoMutex;
	bool HasInfo = false;
	void LoadInfo();
	kui::UIBox* ActionsBox = nullptr;
	std::vector<unsigned int> LoadedImages;
	void GenerateActionButtons(kui::UIBox* Parent, const NxmAPI::ModInfo& Mod);
	unsigned int PreviewWebp = 0;
public:
	kui::Vec2ui GetWindowResolution() override;
	
	ModInfoWindowElement* InfoElement = nullptr;

	void SetModInfo(NxmAPI::ModInfo Info);
	NxmAPI::ModInfo GetModInfo();

	std::string GetWindowTitle() override;
	void Init() override;
	void Update() override;
	void Destroy() override;

	void RenderMarkupString(std::string Markup, kui::UIBox* Parent);
	void RenderMarkupLine(std::string Line, bool Centered, kui::UIBox* Parent);
};