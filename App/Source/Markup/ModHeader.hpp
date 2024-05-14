#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIText.h"
class ModHeader : public KlemmUI::UIBox
{
	std::string Name = std::string("Trending");

public:
	KlemmUI::UIText* unnamed_0;
	ModHeader() : UIBox(true)
	{
	this->SetTryFill((bool)true);
	this->SetHorizontal(false);
	{
	auto* e_1 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_1->SetText(Name);
	e_1->SetColor(Vector3f(1));
	e_1->SetTextSize(15);
	e_1->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_1->SetPadding((float)5);
	e_1->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->SetDownPadding((float)0);
	e_1->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(e_1);
	unnamed_0 = e_1;
	auto* e_2 = new KlemmUI::UIBackground(true, 0, 1);
	e_2->SetColor(Vector3f(0.6,0,0.1));
	e_2->SetMinSize(Vector2f(2));
	e_2->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetPadding((float)4);
	e_2->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetTryFill((bool)true);
	this->AddChild(e_2);
	}

	}

	void SetName(std::string NewValue)
	{
		Name = NewValue;
		unnamed_0->SetText(Name);
	}
};
