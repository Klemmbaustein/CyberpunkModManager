#include "Profile.h"
#include <filesystem>
#include "ModInfo.h"
#include "FileUtil.h"
#include <iostream>
#include <fstream>
const std::string ProfileRootPath = "app/profiles/";
const std::string ActiveProfileFile = "app/saved/profile.txt";
Profile Profile::Current;

static void SaveCurrentProfile()
{
	std::ofstream ProfileInfoFile = std::ofstream(ActiveProfileFile);
	ProfileInfoFile << Profile::Current.DisplayName;
	ProfileInfoFile.close();
}

void Profile::Init()
{
	std::string LastProfile = FileUtil::ReadFile(ActiveProfileFile);

	if (LastProfile.empty())
	{
		// Make the default profile 'test' if it exists and no other profile is saved.
		// Before profile support, all mod files were put into app/profiles/test/
		// And things might break if we just create a new profile and ignore 'test' until it's switched back to.
		if (std::filesystem::exists("app/profiles/test/"))
		{
			LastProfile = "test";
		}
		else
		{
			LastProfile = "Default";
		}
	}

	Current = NewProfile(LastProfile);
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
	if (IsCurrent())
	{
		// Don't unload/reload mods if the new one is the already active one.
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

	SaveCurrentProfile();
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

	std::string NewPath = ProfileRootPath + NewName + "/";

	if (std::filesystem::exists(NewPath))
	{
		return;
	}

	bool IsCurrent = Path == Current.Path;

	std::filesystem::rename(Path, NewPath);

	Path = NewPath;
	DisplayName = NewName;

	if (IsCurrent)
	{
		Current = *this;
		SaveCurrentProfile();
	}
}

bool Profile::IsCurrent() const
{
	return Current.Path == Path;
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
