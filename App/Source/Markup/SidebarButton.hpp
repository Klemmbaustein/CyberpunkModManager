#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
class SidebarButton : public KlemmUI::UIBox
{

public:
	KlemmUI::UIBackground* image;
	KlemmUI::UIButton* button;
	SidebarButton() : UIBox(true)
	{
	this->SetPadding((float)float(5));
	this->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	{
	button = new KlemmUI::UIButton(true, 0, 1, nullptr);
	button->SetColor(Vector3f(float(0.6), float(0.1), float(0.1)));
	button->SetHoveredColor(Vector3f(float(0.6), float(0.1), float(0.1)) * 0.75f);
	button->SetPressedColor(Vector3f(float(0.6), float(0.1), float(0.1)) * 0.5f);
	button->BoxBorder = KlemmUI::UIBox::BorderType::Rounded;
	button->BorderRadius = float(5);
	button->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	button->SetMinSize(Vector2f(float(70)));
	button->SetMaxSize(Vector2f(float(70)));
	button->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(button);
	{
	image = new KlemmUI::UIBackground(true, 0, 1);
	image->SetColor(Vector3f(float(0)));
	image->SetMinSize(Vector2f(float(70)));
	image->SetMaxSize(Vector2f(float(70)));
	image->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->AddChild(image);
	}
	}

	}

};
