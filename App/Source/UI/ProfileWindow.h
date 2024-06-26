#include "UI.h"
#include "Popup.h"
#include <KlemmUI/UI/UIScrollBox.h>

class ProfileWindow : public Popup
{
public:
	KlemmUI::UIScrollBox* ProfilesList = nullptr;

	void Init() override;
	void Update() override;
	void GenerateList();

	std::string GetWindowTitle() override;
};
