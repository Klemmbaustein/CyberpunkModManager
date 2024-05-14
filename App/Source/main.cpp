#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>

#include "NexusModsAPI.h"
#include "Net.h"

#include "UI/Popup.h"
#include "UI/Tabs/AppTab.h"
#include "UI/Tabs/ModBrowserTab.h"
#include "BackgroundTask.h"
#include "WindowsFunctions.h"

#include <iostream>

using namespace KlemmUI;

static void OnResized(KlemmUI::Window* Target)
{
	AppTab::ResizeAll();
}

int main()
{
	Application::Error::SetErrorCallback([](std::string Message) {
		Windows::ShowDialogBox("Internal error:\n" + Message);
		});
	Application::Initialize("../Dependencies/KlemmUI/Shaders");

	Window AppWindow = Window("Very cool Cyberpunk 2077 mod manager", Window::WindowFlag::Resizable);
	AppWindow.OnResizedCallback = &OnResized;

	Net::SetAPIKey(NexusModsAPI::GetAPIKey());
	UI::Load();

	new ModBrowserTab();

	while (AppWindow.UpdateWindow())
	{
		AppTab::UpdateAll();
		Popup::UpdatePopups();
		BackgroundTask::UpdateTasks();
	}
}