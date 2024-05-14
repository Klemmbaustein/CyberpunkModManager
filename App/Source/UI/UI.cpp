#include "UI.h"
#include <KlemmUI/Window.h>
using namespace KlemmUI;

thread_local Font* UI::Text = nullptr;

void UI::Load()
{
	Text = new Font("app/Font.ttf");
	Window::GetActiveWindow()->Markup.SetDefaultFont(Text);
}

void UI::Unload()
{
	delete Text;
}
