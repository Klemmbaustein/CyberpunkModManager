#include "UI.h"
#include <kui/Window.h>
#include <iostream>
#include <filesystem>
using namespace kui;

thread_local Font* UI::Text = nullptr;

void UI::Load()
{
	Text = new Font("res:Font.ttf");
	Window::GetActiveWindow()->Markup.SetDefaultFont(Text);
}

void UI::Unload()
{
	delete Text;
}