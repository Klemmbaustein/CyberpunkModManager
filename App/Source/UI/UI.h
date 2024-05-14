#pragma once
#include <KlemmUI/UI/UIText.h>

namespace UI
{
	void Load();
	void Unload();

	extern thread_local KlemmUI::Font* Text;
}