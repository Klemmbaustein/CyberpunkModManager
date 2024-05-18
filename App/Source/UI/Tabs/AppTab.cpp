#include "AppTab.h"
#include <KlemmUI/Window.h>
using namespace KlemmUI;

std::vector<AppTab*> AppTab::AllTabs;
size_t AppTab::SelectedTab = 0;

AppTab::AppTab(std::string Name)
{
	TabBackground = new UIBackground(false, -1, 0.15f, 2);
	AllTabs.push_back(this);
}

AppTab::~AppTab()
{
	delete TabBackground;
}

void AppTab::Update()
{
}

void AppTab::OnResized()
{
}

void AppTab::ResizeAll()
{
	UpdateAll();
	for (AppTab* Tab : AllTabs)
	{
		Tab->OnResized();
	}
}

void AppTab::UpdateAll()
{
	Vector2f SiteBarSize = UIBox::PixelSizeToScreenSize(Vector2f(80, 0), Window::GetActiveWindow());
	size_t TabIndex = 0;
	for (AppTab* Tab : AllTabs)
	{
		Tab->TabBackground->IsVisible = SelectedTab == TabIndex++;
		Tab->TabBackground->SetPosition(Vector2f(-1) + SiteBarSize);
		Tab->TabBackground->SetMinSize(Vector2f(2) - SiteBarSize);
		Tab->TabBackground->SetMaxSize(Vector2f(2) - SiteBarSize);
		Tab->Update();
	}
}
