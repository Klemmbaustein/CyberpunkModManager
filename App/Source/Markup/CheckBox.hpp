#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
class CheckBox : public KlemmUI::UIBox
{
	std::string Image;

public:
	KlemmUI::UIButton* checkButton;
	CheckBox() : UIBox(true)
	{
	{
	checkButton = new KlemmUI::UIButton(true, 0, 1, nullptr);
	checkButton->SetUseTexture(true, Image);
	checkButton->BoxBorder = KlemmUI::UIBox::BorderType::Rounded;
	checkButton->BorderRadius = 5;
	checkButton->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	checkButton->SetMinSize(Vector2f(22));
	checkButton->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(checkButton);
	}

	}

	void SetImage(std::string NewValue)
	{
		Image = NewValue;
		checkButton->SetUseTexture(true, Image);
	}
};
