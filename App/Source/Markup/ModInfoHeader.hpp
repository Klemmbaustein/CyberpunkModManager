#pragma once
#include "KlemmUI/Markup/Markup.h"
#include "KlemmUI/UI/UIBackground.h"
#include "KlemmUI/UI/UIBox.h"
#include "KlemmUI/UI/UIText.h"
class ModInfoHeader : public KlemmUI::UIBox
{
	std::string Name = std::string("Mod title");
	std::string SubTitle = std::string("Mod subtitle");

public:
	KlemmUI::UIBackground* image;
	KlemmUI::UIText* name;
	KlemmUI::UIText* subTitle;
	ModInfoHeader() : UIBox(true)
	{
	{
	image = new KlemmUI::UIBackground(true, 0, 1);
	image->BoxBorder = KlemmUI::UIBox::BorderType::DarkenedEdge;
	image->BorderRadius = 2;
	image->BorderSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	image->BorderColor = Vector3f(0.6,0,0.1);
	image->SetMinSize(Vector2f(340,180));
	image->SetMaxSize(Vector2f(340,180));
	image->SetSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	image->SetPadding((float)5);
	image->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	this->AddChild(image);
	auto* e_2 = new KlemmUI::UIBox(true);
	e_2->SetHorizontal(false);
	this->AddChild(e_2);
	{
	name = new KlemmUI::UIText(1, 1, "", nullptr);
	name->SetText(Name);
	name->SetTextSize(15);
	name->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	name->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	name->WrapDistance = 800;
	name->Wrap = true;
	name->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	name->SetPadding((float)5);
	name->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->AddChild(name);
	subTitle = new KlemmUI::UIText(1, 1, "", nullptr);
	subTitle->SetText(SubTitle);
	subTitle->SetColor(Vector3f(0.9));
	subTitle->SetTextSize(11);
	subTitle->SetTextSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	subTitle->SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont(""));
	subTitle->WrapDistance = 800;
	subTitle->Wrap = true;
	subTitle->WrapSizeMode = KlemmUI::UIBox::SizeMode::PixelRelative;
	subTitle->SetPadding((float)5);
	subTitle->SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::PixelRelative);
	e_2->AddChild(subTitle);
	}
	}

	}

	void SetName(std::string NewValue)
	{
		Name = NewValue;
		name->SetText(Name);
	}
	void SetSubTitle(std::string NewValue)
	{
		SubTitle = NewValue;
		subTitle->SetText(SubTitle);
	}
};
