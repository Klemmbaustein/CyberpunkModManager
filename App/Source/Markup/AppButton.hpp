#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class AppButton : public KlemmUI::UIBox
{
	std::string Image;
	std::string Text;

public:
	KlemmUI::UIBackground* unnamed_0;
	KlemmUI::UIButton* button;
	KlemmUI::UIText* unnamed_1;
	AppButton() : UIBox(true)
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
	button->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	button->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	this->AddChild(button);
	{
	auto* e_2 = new KlemmUI::UIBackground(true, 0, 1);
	e_2->SetUseTexture(true, Image);
	e_2->SetMinSize(Vector2f(float(20)));
	e_2->SetMaxSize(Vector2f(float(20)));
	e_2->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetPadding((float)float(3));
	e_2->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->AddChild(e_2);
	unnamed_0 = e_2;
	auto* e_3 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_3->SetText(Text);
	e_3->SetTextSize(float(12));
	e_3->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_3->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_3->SetPadding((float)float(5));
	e_3->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->AddChild(e_3);
	unnamed_1 = e_3;
	}
	}

	}

	void SetImage(std::string NewValue)
	{
		Image = NewValue;
		unnamed_0->SetUseTexture(true, Image);
	}
	void SetText(std::string NewValue)
	{
		Text = NewValue;
		unnamed_1->SetText(Text);
	}
};
