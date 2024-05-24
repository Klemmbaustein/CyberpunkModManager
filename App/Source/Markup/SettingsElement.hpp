#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIText.h"
class SettingsElement : public KlemmUI::UIBox
{
	std::string SettingName;

public:
	KlemmUI::UIBox* content;
	KlemmUI::UIText* unnamed_0;
	SettingsElement() : UIBox(true)
	{
	this->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	this->SetPadding((float)10);
	this->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	{
	auto* e_1 = new KlemmUI::UIBox(true);
	e_1->SetMinSize(Vector2f(300,0));
	e_1->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->SetRightPadding((float)50);
	e_1->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(e_1);
	{
	auto* e_2 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_2->SetText(SettingName);
	e_2->SetTextSize(12);
	e_2->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_2->WrapDistance = 600;
	e_2->Wrap = true;
	e_2->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	e_1->AddChild(e_2);
	unnamed_0 = e_2;
	}
	content = new KlemmUI::UIBox(true);
	this->AddChild(content);
	}

	}

	void SetSettingName(std::string NewValue)
	{
		SettingName = NewValue;
		unnamed_0->SetText(SettingName);
	}
};
