#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>

#include "NexusModsAPI.h"
#include "Net.h"

#include <iostream>

using namespace KlemmUI;

int main()
{
	Application::Initialize("../Dependencies/KlemmUI/Shaders");

	Window AppWindow = Window("Test", Window::WindowFlag::Resizable);
	Net::SetAPIKey(NexusModsAPI::GetAPIKey());

	auto Mods = NexusModsAPI::GetMods();
	std::cout << "Last updated mods on nexusmods.com:" << std::endl;
	for (NexusModsAPI::ModInfo i : Mods)
	{
		std::cout << "\t" << i.Name << std::endl;
	}
	while (AppWindow.UpdateWindow())
	{
	}
}