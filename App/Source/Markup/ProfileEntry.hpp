#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIButton.h"
#include "KlemmUI/UI/UIText.h"
#include "ProfileButton.hpp"
class ProfileEntry : public KlemmUI::UIBox
{
	Vector3f Color = Vector3f(float(0.6), float(0.1), float(0.1));
	std::string Description;

public:
	KlemmUI::UIBackground* activeIcon;
	KlemmUI::UIBox* nameBox;
	KlemmUI::UIButton* profileButton;
	KlemmUI::UIText* unnamed_0;
	ProfileButton* copyButton;
	ProfileButton* deleteButton;
	ProfileEntry() : UIBox(true)
	{
	this->SetHorizontalAlign(KlemmUI::UIBox::Align::Centered);
	this->SetTryFill((bool)true);
	{
	profileButton = new KlemmUI::UIButton(true, 0, 1, nullptr);
	profileButton->SetColor(Vector3f(Color));
	profileButton->SetHoveredColor(Vector3f(Color) * 0.75f);
	profileButton->SetPressedColor(Vector3f(Color) * 0.5f);
	profileButton->BoxBorder = KlemmUI::UIBox::BorderType::Rounded;
	profileButton->BorderRadius = float(5);
	profileButton->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	profileButton->SetMinSize(Vector2f(float(1.9), float(0)));
	profileButton->SetVerticalAlign(KlemmUI::UIBox::Align::Centered);
	profileButton->SetPadding((float)float(5));
	profileButton->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(profileButton);
	{
	activeIcon = new KlemmUI::UIBackground(true, 0, 1);
	activeIcon->SetUseTexture(true, "app/icons/enabled.png");
	activeIcon->SetMinSize(Vector2f(float(25)));
	activeIcon->SetMaxSize(Vector2f(float(25)));
	activeIcon->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	activeIcon->SetPadding((float)float(10));
	activeIcon->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	profileButton->AddChild(activeIcon);
	auto* e_3 = new KlemmUI::UIBox(true);
	e_3->SetMinSize(Vector2f(float(1.5), float(0)));
	e_3->SetHorizontal(false);
	profileButton->AddChild(e_3);
	{
	nameBox = new KlemmUI::UIBox(true);
	nameBox->SetPadding((float)float(5));
	nameBox->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_3->AddChild(nameBox);
	auto* e_5 = new KlemmUI::UIText(1, 1, "", nullptr);
	e_5->SetText(Description);
	e_5->SetColor(Vector3f(float(0.8)));
	e_5->SetTextSize(float(12));
	e_5->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_5->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	e_5->SetPadding((float)float(5));
	e_5->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_3->AddChild(e_5);
	unnamed_0 = e_5;
	}
	copyButton = new ProfileButton();
	copyButton->SetImage("app/icons/copy.png");
	profileButton->AddChild(copyButton);
	deleteButton = new ProfileButton();
	deleteButton->SetImage("app/icons/delete.png");
	profileButton->AddChild(deleteButton);
	}
	}

	}

	void SetColor(Vector3f NewValue)
	{
		Color = NewValue;
		profileButton->SetColor(Vector3f(Color));
		profileButton->SetHoveredColor(Vector3f(Color) * 0.75f);
		profileButton->SetPressedColor(Vector3f(Color) * 0.5f);
	}
	void SetDescription(std::string NewValue)
	{
		Description = NewValue;
		unnamed_0->SetText(Description);
	}
};
