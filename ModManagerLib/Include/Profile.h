#include <string>
#include <vector>
#include <cstdint>

struct Profile
{
	static std::vector<Profile> GetAllProfiles();
	static Profile NewProfile(std::string Name);
	static Profile Current;
	std::string Path;
	std::string DisplayName;
	void MakeActive() const;

	uint32_t GetModsCount() const;

	void Rename(std::string NewName);

	bool IsCurrent() const;

	bool Copy() const;

	static void Init();
};
