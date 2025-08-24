#include "TitleBar.h"
#include <kui/KlemmUI.h>
#include <TitleBar.kui.hpp>
#include <iostream>

using namespace kui;

bool TitleBar::IsVisible = true;
bool TitleBar::IsMaximized = false;

namespace TitleBar
{
	TitleBarElement* Elem = nullptr;
}

void TitleBar::Load()
{
	Elem = new TitleBarElement();

	Elem->close->btn->OnClicked = []()
		{
			kui::Window::GetActiveWindow()->Close();
		};

	Elem->maximize->btn->OnClicked = []()
		{
			bool NewMaximized = !Window::GetActiveWindow()->GetWindowFullScreen();
			kui::Window::GetActiveWindow()->SetMaximized(NewMaximized);
		};

	Elem->minimize->btn->OnClicked = []()
		{
			kui::Window::GetActiveWindow()->SetMinimized(true);
		};

	kui::Window::GetActiveWindow()->IsAreaGrabbableCallback = [](Window*) -> bool
		{
			return Elem->bg->IsBeingHovered();
		};
	Elem->IsVisible = TitleBar::IsVisible;
}

void TitleBar::Update()
{
	Window* Current = Window::GetActiveWindow();

	bool NewIsMaximized = Current->GetWindowFullScreen();

	if (NewIsMaximized != IsMaximized)
	{
		IsMaximized = NewIsMaximized;
		Elem->maximize->SetIcon(NewIsMaximized ? "res:icons/ChromeRestore.png" : "res:icons/ChromeMaximize.png");
		Elem->RedrawElement();
	}

	if (Elem->IsVisible != TitleBar::IsVisible)
	{
		Window::WindowFlag Flags = Current->GetWindowFlags();

		Flags = Window::WindowFlag(int(Flags) & ~int(Window::WindowFlag::Borderless));

		if (TitleBar::IsVisible)
		{
			Flags = Flags | Window::WindowFlag::Borderless;
		}

		Current->SetWindowFlags(Flags);

		Elem->IsVisible = TitleBar::IsVisible;
	}

	if (TitleBar::IsVisible)
	{
		Elem->title->SetColor(Current->HasFocus() ? Vec3f(1) : Vec3f(0.75f));
	}
}