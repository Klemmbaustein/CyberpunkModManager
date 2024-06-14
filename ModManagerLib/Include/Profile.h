#include <string>
#include <vector>

struct Profile
{
	static std::vector<Profile> GetAllProfiles();
	static Profile NewProfile(std::string Name);
	static Profile Current;
	std::string Path;
	std::string DisplayName;
	void MakeActive();

	static void Init();
};
