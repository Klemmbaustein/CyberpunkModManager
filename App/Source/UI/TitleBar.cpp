#include "TitleBar.h"
#include <kui/KlemmUI.h>
#include <TitleBar.kui.hpp>

using namespace kui;

bool TitleBar::IsVisible = true;

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
			kui::Window::GetActiveWindow()->SetMaximized(!kui::Window::GetActiveWindow()->GetWindowFullScreen());
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
	if (Elem->IsVisible != TitleBar::IsVisible)
	{
		Window* Current = Window::GetActiveWindow();
		Window::WindowFlag Flags = Current->GetWindowFlags();

		Flags = Window::WindowFlag(int(Flags) & ~int(Window::WindowFlag::Borderless));

		if (TitleBar::IsVisible)
		{
			Flags = Flags | Window::WindowFlag::Borderless;
		}

		Current->SetWindowFlags(Flags);

		Elem->IsVisible = TitleBar::IsVisible;
	}
}