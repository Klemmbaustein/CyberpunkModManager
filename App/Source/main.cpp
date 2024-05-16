#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>

#include "NexusModsAPI.h"
#include "Net.h"
#include "Game.h"

#include "UI/Popup.h"
#include "UI/Tabs/AppTab.h"
#include "UI/Tabs/ModBrowserTab.h"
#include "UI/Tabs/InstalledModsTab.h"
#include "BackgroundTask.h"
#include "WindowsFunctions.h"
#include "DownloadHandler.h"

#include <iostream>

using namespace KlemmUI;

static void OnResized(KlemmUI::Window* Target)
{
	AppTab::ResizeAll();
}

// NexusMods handles mod manager downloads with an URI (nxm://).
// The mod manager opens URIs when they're passed in the launch args with uri=nxm://cyberpunk2077/xyz/def/abc
const std::string OpenUriArg = "uri=";

int main(int argc, char** argv)
{
	Windows::SetWorkingDirectory();

	Application::Error::SetErrorCallback([](std::string Message) {
		Windows::ErrorBox("Internal error:\n" + Message);
		});
	Application::Initialize("../Dependencies/KlemmUI/Shaders");

	Net::SetAPIKey(NexusModsAPI::GetAPIKey());
	for (int i = 1; i < argc; i++)
	{
		std::string ArgString = std::string(argv[i]);
		if (ArgString.substr(0, OpenUriArg.size()) == OpenUriArg)
		{
			DownloadHandler::DownloadModUri(ArgString.substr(OpenUriArg.size()));
		}
	}

	Windows::RegisterSelfAsUriHandler();

	Window AppWindow = Window("Very cool Cyberpunk 2077 mod manager", Window::WindowFlag::Resizable);
	AppWindow.OnResizedCallback = &OnResized;


	std::cout << Game::SearchForGOGGame() << std::endl;


	UI::Load();

	//new ModBrowserTab();
	new InstalledModsTab();
	while (AppWindow.UpdateWindow())
	{
		AppTab::UpdateAll();
		Popup::UpdatePopups();
		BackgroundTask::UpdateTasks();
		DownloadHandler::CheckDownloadRequest();
	}
}

int WinMain()
{
	return main(__argc, __argv);
}