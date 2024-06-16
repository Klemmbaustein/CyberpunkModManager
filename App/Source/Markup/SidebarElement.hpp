#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
class SidebarElement : public KlemmUI::UIBox
{

public:
	KlemmUI::UIBackground* bg;
	SidebarElement() : UIBox(true)
	{
	this->SetMinSize(Vector2f(0,2));
	{
	bg = new KlemmUI::UIBackground(true, 0, 1);
	bg->SetColor(Vector3f(0.1));
	bg->SetMinSize(Vector2f(80));
	bg->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	bg->SetTryFill((bool)true);
	bg->SetHorizontal(false);
	this->AddChild(bg);
	}

	}

};
