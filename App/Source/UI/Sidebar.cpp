#include "Sidebar.h"
#include <Sidebar.kui.hpp>
#include "Tabs/AppTab.h"
#include <iostream>
#include <kui/KlemmUI.h>
using namespace kui;

namespace Sidebar
{
	SidebarElement* Element = nullptr;
	SidebarTooltip* Tooltip = nullptr;
	UIBox* LastHoveredBox = nullptr;
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
		Button->button->OnClicked = [Index]() {
			AppTab::SelectedTab = Index;
			};
		Index++;
		Button->image->SetUseTexture(true, "res:icons/" + i->IconFile);
		Element->bg->AddChild(Button);
		Buttons.push_back(Button);
	}
}

void Sidebar::Update()
{
	size_t Index = 0;

	UIBox* HoveredBox = nullptr;
	size_t HoveredIndex = 0;
	for (SidebarButton* i : Buttons)
	{
		bool Selected = Index == AppTab::SelectedTab;
		i->button->SetColor(Selected ? Vec3f(1, 0.3f, 0.4f) : Vec3f(0.6f, 0.1f, 0.1f));
		i->button->SetHoveredColor(Selected ? Vec3f(1, 0.3f, 0.4f) : Vec3f(0.5f, 0.1f, 0.1f));
		i->button->SetPressedColor(Selected ? Vec3f(1, 0.3f, 0.4f) : Vec3f(0.4f, 0.1f, 0.1f));

		if (i->IsBeingHovered() && i->GetParentWindow()->HasFocus())
		{
			HoveredBox = i;
			HoveredIndex = Index;
		}

		Index++;
	}

	if (!HoveredBox && Tooltip)
	{
		delete Tooltip;
		LastHoveredBox = nullptr;
		Tooltip = nullptr;
	}

	if ((HoveredBox && !Tooltip) || LastHoveredBox != HoveredBox)
	{
		if (Tooltip)
			delete Tooltip;

		Tooltip = new SidebarTooltip();
		Tooltip->SetPosition(Vec2f(Element->GetPosition().X + (70_px).GetScreen().X, HoveredBox->GetPosition().Y));
		Tooltip->SetText(AppTab::AllTabs[HoveredIndex]->Name);
		Tooltip->UpdateElement();
		Tooltip->RedrawElement();
		LastHoveredBox = HoveredBox;
	}
}
