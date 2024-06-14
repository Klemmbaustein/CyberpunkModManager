#include "Profile.h"
#include <filesystem>
#include "ModInfo.h"
const std::string ProfileRootPath = "app/profiles/";

Profile Profile::Current; 

void Profile::Init()
{
	Current = NewProfile("Default");
}

Profile Profile::NewProfile(std::string Name)
{
	Profile New;
	New.DisplayName = Name;
	New.Path = ProfileRootPath + Name + "/";
	return New;
}

std::vector<Profile> Profile::GetAllProfiles()
{
	std::filesystem::create_directories(ProfileRootPath);
	std::vector<Profile> Found;

	for (const auto& File : std::filesystem::directory_iterator(ProfileRootPath))
	{
		Found.push_back({
			.Path = File.path().string() + "/",
			.DisplayName = File.path().filename().string()
			});
	}

	return Found;
}

void Profile::MakeActive()
{
	std::vector<ModInfo> ActiveMods = ModInfo::GetAllInstalledMods();

	for (auto& i : ActiveMods)
	{
		i.Unload();
	}
	Current = *this;

	ActiveMods = ModInfo::GetAllInstalledMods();

	for (auto& i : ActiveMods)
	{
		i.Load();
	}
}
