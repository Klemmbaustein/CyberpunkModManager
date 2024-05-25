#include "FOMOD.h"
#include "StrUtil.h"
#include <pugixml.hpp>
#include <iostream>
#include <filesystem>

FOMOD FOMOD::Parse(std::string FolderPath)
{
	FOMOD Out;
	Out.FromPath = FolderPath;

	pugi::xml_document Doc;
	Doc.load_file((FolderPath + "/fomod/ModuleConfig.xml").c_str());

	for (const auto& i : Doc.first_child().child("requiredInstallFiles").children())
	{
		Out.Files.push_back(ConditionalFile{
			.Files = {InstallFile(
			i.attribute("source").as_string(),
			i.attribute("destination").as_string()
			),
		} });
	}

	for (const auto& i : Doc.first_child().child("conditionalFileInstalls").child("patterns").children())
	{
		std::vector<InstallFile> Files;
		for (const auto& f : i.child("files").children())
		{
			Files.push_back(InstallFile(
				f.attribute("source").as_string(),
				f.attribute("destination").as_string()
			));
		}

		FlagCondition Cond;
		Cond.Operation = i.child("dependencies").attribute("operator").as_string();
		
		for (const auto& dep : i.child("dependencies").children())
		{
			Cond.Flags.push_back(FlagValue{
				// It's called 'flag' here, but 'name' elsewhere.
				// I wish they'd name their things consistently.
				.Name = dep.attribute("flag").as_string(),
				.Value = dep.attribute("value").as_string(),
			});
		}


		Out.Files.push_back(ConditionalFile{
			.Condition = Cond,
			.Files = Files,
		});
	}

	for (const auto& i : Doc.first_child().child("installSteps").children())
	{
		std::vector<InstallGroup> Groups;

		for (const auto& g : i.child("optionalFileGroups").children())
		{
			std::vector<InstallPlugin> Plugins;

			for (const auto& p : g.child("plugins").children())
			{
				std::vector<InstallFile> Files;
				std::vector<FlagValue> ConditionalFlags;

				for (const auto& i : p.child("files").children())
				{
					Files.push_back(InstallFile(
						i.attribute("source").as_string(),
						i.attribute("destination").as_string()
					));
				}

				for (const auto& i : p.child("conditionFlags").children())
				{
					ConditionalFlags.push_back(FlagValue{
						.Name = i.attribute("name").as_string(),
						.Value = i.text().as_string()
						});
				}

				Plugins.push_back(InstallPlugin{
					.Name = p.attribute("name").as_string(),
					.ImageFile = Out.FromPath + "/" + StrUtil::Replace(p.child("image").attribute("path").as_string(), "\\", "/"),
					.Description = p.child("description").text().as_string(),
					.Files = Files,
					.Flags = ConditionalFlags,
					});
			}

			Groups.push_back(InstallGroup{
				.Name = g.attribute("name").as_string(),
				.Type = g.attribute("type").as_string(),
				.Plugins = Plugins,
				});
		}

		FlagCondition VisibleCondition;

		VisibleCondition.Operation = i.child("visible").child("dependencies").attribute("operator").as_string();

		for (auto& cond : i.child("visible").child("dependencies").children())
		{
			VisibleCondition.Flags.push_back(FlagValue{
				.Name = cond.attribute("flag").as_string(),
				.Value =  cond.attribute("value").as_string(),
				});
		}

		Out.Steps.push_back(InstallStep
			{
				.Name = i.attribute("name").as_string(),
				.Groups = Groups,
				.Visible = VisibleCondition,
			});
	}


	return Out;
}

void FOMOD::Install(std::string Path)
{
	ToPath = Path;
	for (auto& i : Files)
	{
		if (!i.Condition.Evaluate(*this))
		{
			continue;
		}
		std::cout << "FILES FOR: " << i.Files.size() << std::endl;
		for (auto& f : i.Files)
		{
			size_t DirPos = f.ToPath.find_last_of("/");
			std::string ToDirName = "";
			if (DirPos != std::string::npos)
			{
				ToDirName = f.ToPath.substr(0, DirPos);
			}
			std::filesystem::create_directories(ToPath + "/" + ToDirName);
			std::cout << ToPath + "/" + ToDirName << " -> " << ToPath + f.ToPath << std::endl;
			std::filesystem::copy(FromPath + "/" + f.FromPath, ToPath + "/" + f.ToPath, std::filesystem::copy_options::overwrite_existing);
		}
	}
}

std::string FOMOD::GetFlagValue(std::string FlagName)
{
	for (const auto& i : Flags)
	{
		if (i.Name == FlagName)
		{
			return i.Value;
		}
	}
	return "";
}

FOMOD::InstallFile::InstallFile(std::string From, std::string To)
{
	// The mods I've tested use backslashes in paths instead of forward slashes.
	// Why? This doesn't work on Linux and Windows supports forward slashes in paths.
	FromPath = StrUtil::Replace(From, "\\", "/");
	ToPath = StrUtil::Replace(To, "\\", "/");
}

void FOMOD::InstallPlugin::Enable(FOMOD& Parent)
{
	for (const auto& i : Flags)
	{
		Parent.Flags.push_back(i);
	}

	if (!Files.empty())
	{
		Parent.Files.push_back(ConditionalFile{
			.Files = Files
			});
	}
}

bool FOMOD::FlagCondition::Evaluate(FOMOD& Parent)
{
	if (Flags.empty() || Operation.empty())
	{
		return true;
	}

	bool Value = false;
	bool Initialized = false;

	for (auto& i : Flags)
	{
		if (!Initialized)
		{
			Value = Parent.GetFlagValue(i.Name) == i.Value;
			Initialized = true;
			continue;
		}

		if (Operation == "And")
		{
			Value = Value && Parent.GetFlagValue(i.Name) == i.Value;
		}
		else if (Operation == "Or")
		{
			Value = Value || Parent.GetFlagValue(i.Name) == i.Value;
		}
	}
	return Value;
}

void FOMOD::InstallGroup::DeSelectAll()
{
	for (auto& i : Plugins)
	{
		i.Selected = false;
	}
}

bool FOMOD::InstallGroup::HasValidSelection()
{
	if (Type == "SelectAll"
		|| Type == "SelectAny")
	{
		return true;
	}

	int NumberSelected = 0;

	for (auto& i : Plugins)
	{
		if (i.Selected)
		{
			NumberSelected++;
		}
	}

	if (Type == "SelectExactlyOne" && NumberSelected != 1)
	{
		return false;
	}
	if (Type == "SelectAtMostOne" && NumberSelected > 1)
	{
		return false;
	}
	if (Type == "SelectAtLeastOne" && NumberSelected == 0)
	{
		return false;
	}

	return true;
}
