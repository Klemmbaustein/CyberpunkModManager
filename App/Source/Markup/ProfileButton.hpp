#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
class ProfileButton : public KlemmUI::UIBox
{
	std::string Image;

public:
	KlemmUI::UIButton* button;
	ProfileButton() : UIBox(true)
	{
	{
	button = new KlemmUI::UIButton(true, 0, 1, nullptr);
	button->SetUseTexture(true, Image);
	button->SetMinSize(Vector2f(25));
	button->SetMaxSize(Vector2f(25));
	button->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->SetPadding((float)10);
	button->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(button);
	}

	}

	void SetImage(std::string NewValue)
	{
		Image = NewValue;
		button->SetUseTexture(true, Image);
	}
};
