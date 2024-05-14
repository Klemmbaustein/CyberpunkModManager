#include "ModInfoWindow.h"
#include "../Webp.h"

void ModInfoWindow::LoadInfo()
{
	NexusModsAPI::ModInfo Mod = GetModInfo();

	InfoElement = new ModInfoWindowElement();

	InfoElement->header->SetName(Mod.Name);
	InfoElement->header->SetSubTitle(Mod.Summary);

	InfoElement->header->image->SetUseTexture(true, Webp::Load(Webp::LoadBuffer(Mod.GetImagePath())));

	PopupBackground->AddChild(InfoElement);
}

Vector2ui ModInfoWindow::GetWindowResolution()
{
	return Vector2ui(800, 500);
}

void ModInfoWindow::SetModInfo(NexusModsAPI::ModInfo Info)
{
	std::lock_guard Guard = std::lock_guard(InfoMutex);
	LoadedInfo = Info;
	HasInfo = true;
}

NexusModsAPI::ModInfo ModInfoWindow::GetModInfo()
{
	std::lock_guard Guard = std::lock_guard(InfoMutex);
	return LoadedInfo;
}

std::string ModInfoWindow::GetWindowTitle()
{
	return "";
}

void ModInfoWindow::Init()
{
}

void ModInfoWindow::Update()
{
	if (HasInfo)
	{
		LoadInfo();
		HasInfo = false;
	}
}
