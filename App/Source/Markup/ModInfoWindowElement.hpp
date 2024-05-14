#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "ModInfoHeader.hpp"
class ModInfoWindowElement : public KlemmUI::UIBox
{

public:
	ModInfoHeader* header;
	ModInfoWindowElement() : UIBox(true)
	{
	this->SetTryFill((bool)true);
	this->SetHorizontal(false);
	{
	header = new ModInfoHeader();
	this->AddChild(header);
	auto* e_2 = new KlemmUI::UIBackground(true, 0, 1);
	e_2->SetColor(Vector3f(0.6,0,0.1));
	e_2->SetMinSize(Vector2f(1));
	e_2->SetMaxSize(Vector2f(1));
	e_2->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetTryFill((bool)true);
	this->AddChild(e_2);
	}

	}

};
