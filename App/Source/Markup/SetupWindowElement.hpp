#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class SetupWindowElement : public KlemmUI::UIBox
{

public:
	KlemmUI::UIBox* buttonBox;
	KlemmUI::UIBox* content;
	KlemmUI::UIButton* nextButton;
	SetupWindowElement() : UIBox(true)
	{
	this->SetPosition(Vector2f(-1,-1));
	this->SetHorizontal(false);
	{
	content = new KlemmUI::UIBox(true);
	content->SetMinSize(Vector2f(2,1.8));
	content->SetHorizontal(false);
	this->AddChild(content);
	buttonBox = new KlemmUI::UIBox(true);
	buttonBox->SetMinSize(Vector2f(2,0.2));
	buttonBox->SetHorizontalAlign(KlemmUI::UIBox::Align::Reverse);
	buttonBox->SetVerticalAlign(KlemmUI::UIBox::Align::Default);
	this->AddChild(buttonBox);
	{
	nextButton = new KlemmUI::UIButton(true, 0, 1, nullptr);
	nextButton->SetColor(Vector3f(0.6,0.1,0.1));
	nextButton->SetHoveredColor(Vector3f(0.6,0.1,0.1) * 0.75f);
	nextButton->SetPressedColor(Vector3f(0.6,0.1,0.1) * 0.5f);
	nextButton->SetPadding((float)5);
	nextButton->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	buttonBox->AddChild(nextButton);
	{
	auto* e_4 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_4->SetText("Next");
	e_4->SetTextSize(12);
	e_4->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_4->SetPadding((float)5);
	e_4->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->SetLeftPadding((float)10);
	e_4->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->SetRightPadding((float)10);
	e_4->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	nextButton->AddChild(e_4);
	}
	}
	}

	}

};
