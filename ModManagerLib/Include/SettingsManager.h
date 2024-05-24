#include <string>

class SettingsManager
{
public:
	SettingsManager();

	std::string GetValue(std::string Name, std::string Default);
	void SetValue(std::string Name, std::string Value);

	void Save();
};