#pragma once
#include <string>
#include <vector>

struct FOMOD
{
	std::string FromPath;
	std::string ToPath;

	static FOMOD Parse(std::string FolderPath);

	void Install(std::string Path);

	struct InstallFile
	{
		InstallFile(std::string From, std::string To);
		std::string FromPath;
		std::string ToPath;
	};

	struct FlagValue
	{
		std::string Name;
		std::string Value;
	};


	struct FlagCondition
	{
		std::string Operation;
		std::vector<FlagValue> Flags;

		bool Evaluate(FOMOD& Parent);
	};

	struct InstallPlugin
	{
		std::string Name;
		std::string ImageFile;
		std::string Description;
		std::vector<InstallFile> Files;
		std::vector<FlagValue> Flags;
		bool Selected = false;

		void Enable(FOMOD& Parent);
	};

	struct InstallGroup
	{
		std::string Name;
		std::string Type;
		std::vector<InstallPlugin> Plugins;

		void DeSelectAll();
		bool HasValidSelection();
	};

	struct InstallStep
	{
		std::string Name;
		std::vector<InstallGroup> Groups;
		FlagCondition Visible;
	};

	struct ConditionalFile
	{
		FlagCondition Condition;
		std::vector<InstallFile> Files;
	};

	std::string GetFlagValue(std::string FlagName);

	std::vector<ConditionalFile> Files;
	std::vector<InstallStep> Steps;
	std::vector<FlagValue> Flags;
};
