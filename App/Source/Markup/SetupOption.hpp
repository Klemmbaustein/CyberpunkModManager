#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class SetupOption : public KlemmUI::UIBox
{
	Vector3f Color = Vector3f(float(0.2), float(0.2), float(0.2));
	std::string Description;
	std::string Name;

public:
	KlemmUI::UIBackground* image;
	KlemmUI::UIButton* optionButton;
	KlemmUI::UIText* unnamed_0;
	KlemmUI::UIText* unnamed_1;
	SetupOption() : UIBox(true)
	{
	this->SetPadding((float)float(5));
	this->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	{
	optionButton = new KlemmUI::UIButton(true, 0, 1, nullptr);
	optionButton->SetColor(Vector3f(Color));
	optionButton->SetHoveredColor(Vector3f(Color) * 0.75f);
	optionButton->SetPressedColor(Vector3f(Color) * 0.5f);
	optionButton->BoxBorder = KlemmUI::UIBox::BorderType::DarkenedEdge;
	optionButton->BorderRadius = float(2);
	optionButton->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	optionButton->BorderColor = Vector3f(float(0.6), float(0.1), float(0.1));
	optionButton->SetMinSize(Vector2f(float(250), float(450)));
	optionButton->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	optionButton->SetHorizontal(false);
	this->AddChild(optionButton);
	{
	auto* e_2 = new KlemmUI::UIBackground(true, 0, 1);
	e_2->SetColor(Vector3f(float(0.05)));
	e_2->SetMinSize(Vector2f(float(250), float(175)));
	e_2->SetMaxSize(Vector2f(float(250), float(175)));
	e_2->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	e_2->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	optionButton->AddChild(e_2);
	{
	image = new KlemmUI::UIBackground(true, 0, 1);
	e_2->AddChild(image);
	}
	auto* e_4 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_4->SetText(Name);
	e_4->SetTextSize(float(12));
	e_4->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_4->WrapDistance = float(490);
	e_4->Wrap = true;
	e_4->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	e_4->SetPadding((float)float(3));
	e_4->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	optionButton->AddChild(e_4);
	unnamed_0 = e_4;
	auto* e_5 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_5->SetText(Description);
	e_5->SetColor(Vector3f(float(0.8)));
	e_5->SetTextSize(float(10));
	e_5->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_5->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_5->WrapDistance = float(490);
	e_5->Wrap = true;
	e_5->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	e_5->SetPadding((float)float(3));
	e_5->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	optionButton->AddChild(e_5);
	unnamed_1 = e_5;
	}
	}

	}

	void SetColor(Vector3f NewValue)
	{
		Color = NewValue;
		optionButton->SetColor(Vector3f(Color));
		optionButton->SetHoveredColor(Vector3f(Color) * 0.75f);
		optionButton->SetPressedColor(Vector3f(Color) * 0.5f);
	}
	void SetDescription(std::string NewValue)
	{
		Description = NewValue;
		unnamed_1->SetText(Description);
	}
	void SetName(std::string NewValue)
	{
		Name = NewValue;
		unnamed_0->SetText(Name);
	}
};
