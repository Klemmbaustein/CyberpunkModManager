#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class ModOption : public KlemmUI::UIBox
{
	std::string OptionName;

public:
	KlemmUI::UIButton* button;
	KlemmUI::UIText* unnamed_0;
	ModOption() : UIBox(true)
	{
	{
	button = new KlemmUI::UIButton(true, 0, 1, nullptr);
	button->SetColor(Vector3f(0.075));
	button->SetHoveredColor(Vector3f(0.075) * 0.75f);
	button->SetPressedColor(Vector3f(0.075) * 0.5f);
	button->BoxBorder = KlemmUI::UIBox::BorderType::DarkenedEdge;
	button->BorderRadius = 2;
	button->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	button->BorderColor = Vector3f(0.6,0.1,0.1);
	button->SetMinSize(Vector2f(300,30));
	button->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	button->SetPadding((float)6);
	button->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(button);
	{
	auto* e_2 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_2->SetText(OptionName);
	e_2->SetColor(Vector3f(1));
	e_2->SetTextSize(12);
	e_2->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_2->SetPadding((float)5);
	e_2->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->AddChild(e_2);
	unnamed_0 = e_2;
	}
	}

	}

	void SetOptionName(std::string NewValue)
	{
		OptionName = NewValue;
		unnamed_0->SetText(OptionName);
	}
};
