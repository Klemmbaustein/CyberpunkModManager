#pragma once
#include <string>

namespace Windows
{
	void SetWorkingDirectory();
	void Open(std::string Path);
	bool IsProcessRunning(std::string Name);
	std::string GetCurrentProcessName();

	void ErrorBox(std::string Content);

	void RegisterSelfAsUriHandler();
}