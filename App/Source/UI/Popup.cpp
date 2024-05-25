#include "Popup.h"
#include "UI.h"
#include "../Error.h"

using namespace KlemmUI;

std::mutex Popup::PopupMutex;
std::vector<Popup*> Popup::Popups;

void Popup::PopupStart(Popup* p)
{
	Error::RegisterErrorHandler();
	p->PopupWindow = new Window(
		p->GetWindowTitle(),
		p->HasBorder() ? Window::WindowFlag::None : Window::WindowFlag::Borderless | Window::WindowFlag::AlwaysOnTop,
		Window::POSITION_CENTERED,
		p->GetWindowResolution()
	);

	UI::Load();

	p->PopupBackground = new UIBackground(false, -1, 0.1f, 2);
	p->Init();
	Popups.push_back(p);

	while (p->PopupWindow && ((p->PopupWindow->UpdateWindow() && !p->ShouldClose) || !p->CanClose))
	{
		p->Update();

		if (!p->CanClose)
		{
			p->PopupWindow->CancelClose();
		}
	}
	p->Destroy();
	if (p->PopupWindow)
	{
		delete p->PopupWindow;
	}
	p->PopupWindow = nullptr;

	UI::Unload();
}

Popup::Popup()
{
}

Vector2ui Popup::GetWindowResolution()
{
	return Vector2ui(640, 480);
}

bool Popup::HasBorder()
{
	return true;
}

void Popup::Destroy()
{
}

Popup::~Popup()
{
}

void Popup::Update()
{
}

void Popup::UpdatePopups()
{
	for (size_t i = 0; i < Popups.size(); i++)
	{
		Popup* Current = Popups[i];
		if (!Current->PopupWindow)
		{
			Current->PopupThread->join();
			delete Current->PopupThread;
			delete Current;
			Popups.erase(Popups.begin() + i);
			break;
		}
	}
}