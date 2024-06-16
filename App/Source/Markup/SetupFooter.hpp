#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class SetupFooter : public KlemmUI::UIBox
{

public:
	KlemmUI::UIButton* nextButton;
	SetupFooter() : UIBox(true)
	{
	this->SetTryFill((bool)true);
	this->SetHorizontal(false);
	{
	auto* e_1 = new KlemmUI::UIBackground(true, 0, 1);
	e_1->SetColor(Vector3f(float(0.15)));
	e_1->SetMinSize(Vector2f(float(20)));
	e_1->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->SetHorizontalAlign(KlemmUI::UIBox::Align::Reverse);
	e_1->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	e_1->SetTryFill((bool)true);
	this->AddChild(e_1);
	{
	nextButton = new KlemmUI::UIButton(true, 0, 1, nullptr);
	nextButton->SetMinSize(Vector2f(float(100), float(0)));
	nextButton->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	nextButton->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	nextButton->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	nextButton->SetPadding((float)float(10));
	nextButton->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	nextButton->SetRightPadding((float)float(20));
	nextButton->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->AddChild(nextButton);
	{
	auto* e_3 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_3->SetText("Next");
	e_3->SetColor(Vector3f(float(0)));
	e_3->SetTextSize(float(12));
	e_3->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_3->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_3->SetPadding((float)float(5));
	e_3->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	nextButton->AddChild(e_3);
	}
	}
	}

	}

};
