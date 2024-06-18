#include "UI.h"
#include "Popup.h"
#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/UI/UITextField.h>

class ProfileWindow : public Popup
{
public:
	KlemmUI::UITextField* NewNameTextField = nullptr;
	KlemmUI::UIScrollBox* ProfilesList = nullptr;

	void Init() override;
	void Update() override;
	void GenerateList();

	std::string GetWindowTitle() override;
};
