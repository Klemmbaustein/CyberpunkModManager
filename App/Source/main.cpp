#include <kui/Window.h>
#include <kui/App.h>
#include <kui/Platform.h>

#include "NexusModsAPI.h"
#include "Net.h"

#include "UI/Popup.h"
#include "UI/Sidebar.h"
#include "UI/TitleBar.h"
#include "UI/FOMODInstall.h"
#include "UI/Tabs/AppTab.h"
#include "UI/Tabs/HomeTab.h"
#include "UI/Tabs/ModBrowserTab.h"
#include "UI/Tabs/InstalledModsTab.h"
#include "UI/Tabs/SettingsTab.h"
#include "BackgroundTask.h"
#include "WindowsFunctions.h"
#include "DownloadHandler.h"
#include "Profile.h"
#include "InstallerUpdate.h"
#include "Error.h"


using namespace kui;

static void OnResized(kui::Window*)
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

	new HomeTab();
	new InstalledModsTab();
	new ModBrowserTab();
	new SettingsTab();
	AppTab::UpdateAll();

	TitleBar::Load();
	Sidebar::Load();
}

int main(int argc, char** argv)
{
	Error::RegisterErrorHandler();
	Windows::SetWorkingDirectory();

	app::error::SetErrorCallback([](std::string Message, bool) {
		Windows::ErrorBox("Internal error:\n" + Message);
	});

	Net::SetAPIKey(NxmAPI::GetAPIKey());

	if (SettingsTab::GetSetting("is_uri_handler", "1") == "1")
	{
		Windows::RegisterSelfAsUriHandler();
	}

#if _WIN32
	TitleBar::IsVisible = SettingsTab::GetSetting("use_custom_title_bar", "1") == "1";
#else
	TitleBar::IsVisible = SettingsTab::GetSetting("use_custom_title_bar", "0") == "1";
#endif

	HandleArgs(argc, argv);
	Profile::Init();

	Window::WindowFlag Flags = Window::WindowFlag::Resizable | platform::win32::WindowFlag::DarkTitleBar;

	if (TitleBar::IsVisible)
	{
		Flags = Flags | Window::WindowFlag::Borderless;
	}

	Window AppWindow = Window("Cyberpunk 2077 mod manager", Flags);
	AppWindow.OnResizedCallback = &OnResized;
	AppWindow.BorderColor = Vec3f(0.6f, 0.2f, 0.2f);
	AppWindow.SetMinSize(Vec2ui(700, 400));

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
		TitleBar::Update();
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
