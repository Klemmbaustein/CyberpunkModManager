#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>

#include "NexusModsAPI.h"
#include "Net.h"
#include "Game.h"

#include "UI/Popup.h"
#include "UI/Sidebar.h"
#include "UI/SetupWindow.h"
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

static void HandleArgs(int argc, char** argv)
{
	// NexusMods handles mod manager downloads with an URI (nxm://).
	// The mod manager opens URIs when they're passed in the launch args with uri=nxm://cyberpunk2077/xyz/def/abc
	const std::string OpenUriArg = "uri=";

	for (int i = 1; i < argc; i++)
	{
		std::string ArgString = std::string(argv[i]);
		if (ArgString.substr(0, OpenUriArg.size()) == OpenUriArg)
		{
			DownloadHandler::DownloadModUri(ArgString.substr(OpenUriArg.size()));
		}
	}
}

static void LoadUI()
{
	UI::Load();

	new InstalledModsTab();
	new ModBrowserTab();

	Sidebar::Load();

	if (SetupWindow::ShouldOpen())
	{
		Popup::CreatePopup<SetupWindow>();
	}
}

int main(int argc, char** argv)
{
	Windows::SetWorkingDirectory();


	Application::Error::SetErrorCallback([](std::string Message) {
		Windows::ErrorBox("Internal error:\n" + Message);
		});
	Application::Initialize("../Dependencies/KlemmUI/Shaders");

	Net::SetAPIKey(NexusModsAPI::GetAPIKey());

	Windows::RegisterSelfAsUriHandler();

	HandleArgs(argc, argv);

	Window AppWindow = Window("Very cool Cyberpunk 2077 mod manager", Window::WindowFlag::Resizable);
	AppWindow.OnResizedCallback = &OnResized;

	LoadUI();

	while (AppWindow.UpdateWindow())
	{
		AppTab::UpdateAll();
		Sidebar::Update();
		Popup::UpdatePopups();
		BackgroundTask::UpdateTasks();
		DownloadHandler::CheckDownloadRequest();
	}
}

#if _WIN32
int WinMain()
{
	return main(__argc, __argv);
}
#endif