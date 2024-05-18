#include "Sidebar.h"
#include "../Markup/SidebarElement.hpp"
#include "../Markup/SidebarButton.hpp"
#include "Tabs/AppTab.h"
#include <iostream>

namespace Sidebar
{
	SidebarElement* Element = nullptr;
	std::vector<SidebarButton*> Buttons;
}

void Sidebar::Load()
{
	Element = new SidebarElement();
	Element->SetPosition(-1);

	int Index = 0;
	for (auto& i : AppTab::AllTabs)
	{
		auto Button = new SidebarButton();
		Button->button->OnClickedFunctionIndex = [](int Ind) {
			AppTab::SelectedTab = Ind;
			};
		Button->button->ButtonIndex = Index++;
		Button->image->SetUseTexture(true, "app/icons/" + i->IconFile);
		Element->bg->AddChild(Button);
		Buttons.push_back(Button);
	}
}

void Sidebar::Update()
{
	size_t Index = 0;
	for (auto& i : Buttons)
	{
		bool Selected = Index == AppTab::SelectedTab;
		i->button->SetColor(Selected ? Vector3f(1, 0.3f, 0.4f) : Vector3f(0.6f, 0.1f, 0.1f));
		i->button->SetHoveredColor(Selected ? Vector3f(1, 0.3f, 0.4f) : Vector3f(0.5f, 0.1f, 0.1f));
		i->button->SetPressedColor(Selected ? Vector3f(1, 0.3f, 0.4f) : Vector3f(0.4f, 0.1f, 0.1f));
		Index++;
	}
}
