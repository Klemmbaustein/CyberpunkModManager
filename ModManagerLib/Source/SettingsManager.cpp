#include "SettingsManager.h"
#include "FileUtil.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
using namespace nlohmann;

json SettingsJson = json::object();
static const std::string SettingsFile = "app/saved/settings.json";

SettingsManager::SettingsManager()
{
	SettingsJson = json::object();
	try
	{
		if (std::filesystem::exists(SettingsFile))
		{
			SettingsJson = json::parse(FileUtil::ReadFile(SettingsFile));
		}
	}
	catch (json::exception e)
	{
		std::cout << "Failed to parse settings.json: " << e.what() << std::endl;
	}

	if (!SettingsJson.is_object())
	{
		SettingsJson = json::object();
	}
}

std::string SettingsManager::GetValue(std::string Name, std::string Default)
{
	if (SettingsJson.contains(Name))
	{
		return SettingsJson.at(Name);
	}
	SetValue(Name, Default);
	return Default;
}

void SettingsManager::SetValue(std::string Name, std::string Value)
{
	if (SettingsJson.contains(Name))
	{
		SettingsJson.at(Name) = Value;
		Save();
		return;
	}

	SettingsJson.push_back({ Name, Value });
	Save();
}

void SettingsManager::Save()
{
	std::filesystem::create_directories("app/saved");
	std::ofstream Out = std::ofstream(SettingsFile);
	Out << SettingsJson.dump(2) << std::endl;
	Out.close();
}
