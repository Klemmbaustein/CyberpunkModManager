#pragma once
#include "AppButton.hpp"
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
class NewProfileBox : public KlemmUI::UIBox
{

public:
	AppButton* newButton;
	KlemmUI::UIBox* nameBox;
	NewProfileBox() : UIBox(true)
	{
	this->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	{
	newButton = new AppButton();
	newButton->SetImage("app/icons/add.png");
	newButton->SetText("New profile");
	this->AddChild(newButton);
	nameBox = new KlemmUI::UIBox(true);
	nameBox->SetPadding((float)float(10));
	nameBox->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(nameBox);
	}

	}

};
