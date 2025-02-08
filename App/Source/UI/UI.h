#pragma once
#include <kui/UI/UIText.h>

namespace UI
{
	void Load();
	void Unload();

	extern thread_local kui::Font* Text;
}