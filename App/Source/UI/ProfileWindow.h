#include "UI.h"
#include "Popup.h"

class ProfileWindow : public Popup
{
public:
	void Init() override;

	void Update() override;

	void GenerateList();

	std::string GetWindowTitle() override;
};
