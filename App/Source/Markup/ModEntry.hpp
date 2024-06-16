#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
class ModEntry : public KlemmUI::UIBox
{
	std::string Description;
	std::string Info;
	std::string Name;

public:
	KlemmUI::UIBackground* imageBackground;
	KlemmUI::UIButton* button;
	KlemmUI::UIText* imageText;
	KlemmUI::UIText* infoText;
	KlemmUI::UIText* unnamed_0;
	KlemmUI::UIText* unnamed_1;
	ModEntry() : UIBox(true)
	{
	{
	button = new KlemmUI::UIButton(true, 0, 1, nullptr);
	button->SetColor(Vector3f(float(0.1)));
	button->SetHoveredColor(Vector3f(float(0.1)) * 0.75f);
	button->SetPressedColor(Vector3f(float(0.1)) * 0.5f);
	button->BoxBorder = KlemmUI::UIBox::BorderType::DarkenedEdge;
	button->BorderRadius = float(2);
	button->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	button->BorderColor = Vector3f(float(0.6), float(0.0), float(0.1));
	button->SetMinSize(Vector2f(float(200), float(0)));
	button->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	button->SetPadding((float)float(4));
	button->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->SetHorizontal(false);
	this->AddChild(button);
	{
	imageBackground = new KlemmUI::UIBackground(true, 0, 1);
	imageBackground->SetColor(Vector3f(float(0.05)));
	imageBackground->SetMinSize(Vector2f(float(196), float(112)));
	imageBackground->SetMaxSize(Vector2f(float(196), float(112)));
	imageBackground->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	imageBackground->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	imageBackground->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	imageBackground->SetPadding((float)float(2));
	imageBackground->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	button->AddChild(imageBackground);
	{
	imageText = new KlemmUI::UIText(1, 1, "", nullptr);
	imageText->SetText("Loading");
	imageText->SetColor(Vector3f(float(1)));
	imageText->SetTextSize(float(15));
	imageText->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	imageText->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	imageBackground->AddChild(imageText);
	}
	auto* e_4 = new KlemmUI::UIBox(true);
	e_4->SetMinSize(Vector2f(float(200), float(180)));
	e_4->SetMaxSize(Vector2f(float(200), float(180)));
	e_4->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->SetHorizontal(false);
	button->AddChild(e_4);
	{
	auto* e_5 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_5->SetText(Name);
	e_5->SetTextSize(float(11));
	e_5->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_5->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_5->WrapDistance = float(380);
	e_5->Wrap = true;
	e_5->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	e_5->SetPadding((float)float(4));
	e_5->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->AddChild(e_5);
	unnamed_0 = e_5;
	auto* e_6 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_6->SetText(Description);
	e_6->SetColor(Vector3f(float(0.9)));
	e_6->SetTextSize(float(9));
	e_6->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_6->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_6->WrapDistance = float(370);
	e_6->Wrap = true;
	e_6->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	e_6->SetLeftPadding((float)float(4));
	e_6->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_4->AddChild(e_6);
	unnamed_1 = e_6;
	}
	auto* e_7 = new KlemmUI::UIBackground(true, 0, 1);
	e_7->SetColor(Vector3f(float(0.05)));
	e_7->SetMinSize(Vector2f(float(0), float(30)));
	e_7->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_7->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	e_7->SetPadding((float)float(2));
	e_7->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_7->SetTryFill((bool)true);
	button->AddChild(e_7);
	{
	infoText = new KlemmUI::UIText(1, 1, "", nullptr);
	infoText->SetText(Info);
	infoText->SetColor(Vector3f(float(0.7)));
	infoText->SetTextSize(float(9));
	infoText->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	infoText->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	infoText->WrapDistance = float(370);
	infoText->Wrap = true;
	infoText->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	infoText->SetLeftPadding((float)float(4));
	infoText->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_7->AddChild(infoText);
	}
	}
	}

	}

	void SetDescription(std::string NewValue)
	{
		Description = NewValue;
		unnamed_1->SetText(Description);
	}
	void SetInfo(std::string NewValue)
	{
		Info = NewValue;
		infoText->SetText(Info);
	}
	void SetName(std::string NewValue)
	{
		Name = NewValue;
		unnamed_0->SetText(Name);
	}
};
