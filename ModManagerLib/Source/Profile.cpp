#include "Profile.h"
#include <filesystem>
#include "ModInfo.h"
#include <iostream>
const std::string ProfileRootPath = "app/profiles/";

Profile Profile::Current; 

void Profile::Init()
{
	Current = NewProfile("test");
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

void Profile::MakeActive() const
{
	if (Current.Path == Path)
	{
		// Don't unload/reload profiles if the new profile is the same as the old one.
		return;
	}

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

uint32_t Profile::GetModsCount() const
{
	std::string FilesPath = Path + "/mod_files";
	
	if (!std::filesystem::is_directory(FilesPath))
	{
		return 0;
	}

	uint32_t Found = 0;
	
	for (const auto& File : std::filesystem::directory_iterator(FilesPath))
	{
		Found++;
	}

	return Found;
}

void Profile::Rename(std::string NewName)
{
	if (NewName == DisplayName)
	{
		return;
	}

	if (!std::filesystem::exists(Path))
	{
		return;
	}

	bool IsCurrent = Path == Current.Path;

	std::string NewPath = ProfileRootPath + NewName + "/";

	std::filesystem::rename(Path, NewPath);

	Path = NewPath;
	DisplayName = NewName;

	if (IsCurrent)
	{
		Current = *this;
	}
}

bool Profile::Copy() const
{
	std::string NewName = DisplayName + " - Copy";
	std::string NewPath = ProfileRootPath + NewName;

	if (std::filesystem::exists(NewPath))
	{
		return false;
	}

	try
	{
		std::filesystem::copy(Path, NewPath, std::filesystem::copy_options::recursive);
	}
	catch (std::filesystem::filesystem_error e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}
