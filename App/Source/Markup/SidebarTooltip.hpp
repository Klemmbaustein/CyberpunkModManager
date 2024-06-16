#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIText.h"
class SidebarTooltip : public KlemmUI::UIBox
{
	std::string Text;

public:
	KlemmUI::UIText* unnamed_0;
	SidebarTooltip() : UIBox(true)
	{
	{
	auto* e_1 = new KlemmUI::UIBackground(true, 0, 1);
	e_1->SetColor(Vector3f(0));
	e_1->SetOpacity(0.9);
	e_1->SetMinSize(Vector2f(200,70));
	e_1->SetMaxSize(Vector2f(200,70));
	e_1->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	this->AddChild(e_1);
	{
	auto* e_2 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_2->SetText(Text);
	e_2->SetTextSize(14);
	e_2->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_2->SetPadding((float)15);
	e_2->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_1->AddChild(e_2);
	unnamed_0 = e_2;
	}
	}

	}

	void SetText(std::string NewValue)
	{
		Text = NewValue;
		unnamed_0->SetText(Text);
	}
};
