#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>

#include "NexusModsAPI.h"
#include "Net.h"

#include "UI/Popup.h"
#include "UI/Sidebar.h"
#include "UI/SetupWindow.h"
#include "UI/FOMODInstall.h"
#include "UI/Tabs/AppTab.h"
#include "UI/Tabs/ModBrowserTab.h"
#include "UI/Tabs/InstalledModsTab.h"
#include "UI/Tabs/SettingsTab.h"
#include "BackgroundTask.h"
#include "WindowsFunctions.h"
#include "DownloadHandler.h"
#include "Profile.h"
#include "InstallerUpdate.h"
#include "Error.h"

using namespace KlemmUI;

static void OnResized(KlemmUI::Window*)
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
	new SettingsTab();

	Sidebar::Load();

	if (SetupWindow::ShouldOpen())
	{
		Popup::CreatePopup<SetupWindow>();
	}
}

int main(int argc, char** argv)
{
	Error::RegisterErrorHandler();
	Windows::SetWorkingDirectory();

	Application::Error::SetErrorCallback([](std::string Message) {
		Windows::ErrorBox("Internal error:\n" + Message);
		});
	Application::Initialize("app/shaders");

	Net::SetAPIKey(NxmAPI::GetAPIKey());

	if (SettingsTab::GetSetting("is_uri_handler", "1") == "1")
	{
		Windows::RegisterSelfAsUriHandler();
	}

	HandleArgs(argc, argv);
	Profile::Init();

	Window AppWindow = Window("Cyberpunk 2077 mod manager", Window::WindowFlag::Resizable);
	AppWindow.OnResizedCallback = &OnResized;
//	AppWindow.DPIMultiplier = 0.5f;

	if (SettingsTab::GetSetting("check_updates", "1") == "1")
	{
		InstallerUpdate::CheckForUpdate();
	}

	FOMODInstall::RegisterFOMODCallback();

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
