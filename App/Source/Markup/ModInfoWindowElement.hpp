#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "ModInfoHeader.hpp"
class ModInfoWindowElement : public KlemmUI::UIBox
{

public:
	KlemmUI::UIBox* actionBox;
	KlemmUI::UIBox* descriptionBox;
	ModInfoHeader* header;
	ModInfoWindowElement() : UIBox(true)
	{
	this->SetTryFill((bool)true);
	this->SetHorizontal(false);
	{
	header = new ModInfoHeader();
	this->AddChild(header);
	actionBox = new KlemmUI::UIBox(true);
	actionBox->SetPadding((float)float(5));
	actionBox->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(actionBox);
	auto* e_3 = new KlemmUI::UIBackground(true, 0, 1);
	e_3->SetColor(Vector3f(float(0.6), float(0), float(0.1)));
	e_3->SetMinSize(Vector2f(float(2)));
	e_3->SetMaxSize(Vector2f(float(2)));
	e_3->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_3->SetTryFill((bool)true);
	this->AddChild(e_3);
	descriptionBox = new KlemmUI::UIBox(true);
	descriptionBox->SetPadding((float)float(5));
	descriptionBox->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	descriptionBox->SetTryFill((bool)true);
	descriptionBox->SetHorizontal(false);
	this->AddChild(descriptionBox);
	}

	}

};
