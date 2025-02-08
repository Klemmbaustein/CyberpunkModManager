#include "UI.h"
#include "Popup.h"
#include <kui/UI/UIScrollBox.h>

class ProfileWindow : public Popup
{
public:
	kui::UIScrollBox* ProfilesList = nullptr;

	void Init() override;
	void Update() override;
	void GenerateList();

	std::string GetWindowTitle() override;
};
