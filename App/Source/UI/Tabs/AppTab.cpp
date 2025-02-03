#include "AppTab.h"
#include <kui/Window.h>
using namespace kui;

std::vector<AppTab*> AppTab::AllTabs;
size_t AppTab::SelectedTab = 0;

AppTab::AppTab(std::string Name)
{
	this->Name = Name;
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
	Vec2f SideBarSize = Vec2f((70_px).GetScreen().X, 0);
	size_t TabIndex = 0;
	for (AppTab* Tab : AllTabs)
	{
		Tab->TabBackground->IsVisible = SelectedTab == TabIndex++;
		Tab->TabBackground->SetPosition(Vec2f(-1) + SideBarSize);
		Tab->TabBackground->SetMinSize(Vec2f(2) - SideBarSize);
		Tab->TabBackground->SetMaxSize(Vec2f(2) - SideBarSize);
		Tab->Update();
	}
}
