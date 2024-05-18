#include "WindowsFunctions.h"
#include <iostream>
#include <filesystem>
#if _WIN32
#include <Windows.h>
#include <wtsapi32.h>
#include <StrUtil.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static std::wstring GetExecutableDir()
{
	wchar_t Path[MAX_PATH];
	GetModuleFileNameW(NULL, Path, MAX_PATH);

	return Path;
}

void Windows::SetWorkingDirectory()
{
	std::wstring PathString = GetExecutableDir();

	PathString = PathString.substr(0, PathString.find_last_of(L"/\\"));

	std::filesystem::current_path(PathString);
}

void Windows::Open(std::string Path)
{
	ShellExecuteA(0, 0, Path.c_str(), 0, 0, SW_HIDE);
}

bool Windows::IsProcessRunning(std::string Name)
{
	DWORD pCount = 0;
	PWTS_PROCESS_INFOA ppProcessInfo, pProcess;
	WTSEnumerateProcessesA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &ppProcessInfo, &pCount);
	pProcess = ppProcessInfo;

	for (int i = 0; i < pCount; i++)
	{
		std::string pName = pProcess->pProcessName;
		if (pProcess->ProcessId == GetCurrentProcessId())
		{
			continue;
		}
		if (StrUtil::Lower(Name) == StrUtil::Lower(pName))
		{
			return true;
		}
		pProcess++;
	}
	return false;
}

std::string Windows::GetCurrentProcessName()
{
	char Path[MAX_PATH];
	GetModuleFileNameA(NULL, Path, MAX_PATH);
	std::string PathString = Path;
	return PathString.substr(PathString.find_last_of("/\\") + 1);
}

void Windows::ErrorBox(std::string Content)
{
	MessageBoxA(0, Content.c_str(), "Error", MB_ICONERROR);
}

void ErrorMessageFromStatus(LSTATUS Status)
{
	LPWSTR OutString;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, Status, 0, (LPWSTR)&OutString, 0, NULL);
	MessageBox(NULL, OutString, L"Error", MB_ICONERROR);
}

void Windows::RegisterSelfAsUriHandler()
{
	HKEY ClassesKey = nullptr;
	auto RegStatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Classes", 0, KEY_ALL_ACCESS, &ClassesKey);
	if (RegStatus)
	{
		ErrorMessageFromStatus(RegStatus);
		return;
	}

	HKEY NxmKey = nullptr;
	HKEY CommandKey = nullptr;
	RegStatus = RegCreateKeyW(ClassesKey, L"nxm", &NxmKey);
	if (RegStatus)
	{
		ErrorMessageFromStatus(RegStatus);
		return;
	}
	RegStatus = RegSetValueExW(NxmKey, L"URL Protocol", 0, REG_SZ, (BYTE*)L"", 1);
	if (RegStatus)
	{
		ErrorMessageFromStatus(RegStatus);
		return;
	}
	RegStatus = RegCreateKeyW(NxmKey, L"shell\\open\\command", &CommandKey);
	if (RegStatus)
	{
		ErrorMessageFromStatus(RegStatus);
		return;
	}

	RegSetValueW(NxmKey, NULL, REG_SZ, L"URL:nxm", 8 * sizeof(wchar_t));

	std::wstring ExePath = GetExecutableDir().c_str();
	RegSetValueW(CommandKey, NULL, REG_SZ, (ExePath + L" uri=%1").c_str(), (ExePath.size() + 1) * sizeof(wchar_t));
	RegCloseKey(ClassesKey);
	RegCloseKey(NxmKey);
	RegCloseKey(CommandKey);
}
//#else
#include "StrUtil.h"
#include "FileUtil.h"
#include <fstream>
#include <cstring>
#include <unistd.h>


static std::string GetProcessName(int pid)
{
	FILE* SelfLinkCommand = popen(("readlink /proc/" + std::to_string(pid) + "/exe").c_str(), "r");

	while (!feof(SelfLinkCommand))
	{
		char ExecutableBuffer[8000];
		size_t Read = fread(ExecutableBuffer, 1, sizeof(ExecutableBuffer) - 1, SelfLinkCommand);
		ExecutableBuffer[Read] = 0;
		if (Read > 0 && ExecutableBuffer[Read - 1] == '\n')
		{
			ExecutableBuffer[Read - 1] = 0;
		}
		return ExecutableBuffer;
	}
	pclose(SelfLinkCommand);
	return "";
}

void Windows::SetWorkingDirectory()
{
	std::string PathString = GetProcessName(getpid());

	PathString = PathString.substr(0, PathString.find_last_of("/\\"));

	if (!std::filesystem::is_directory(PathString + "/app"))
	{
		// For CMake builds, the executable wont be in the root directory (like rootDir/build/App instead of rootDir/app)
		PathString.append("/../");
	}
	std::cout << PathString << std::endl;
	std::filesystem::current_path(PathString);
}

void Windows::RegisterSelfAsUriHandler()
{
	char User[8000];

	getlogin_r(User, sizeof(User));

	std::string DesktopFilePath = "/home/" + std::string(User) + "/.local/share/applications/CyberpunkModManager.desktop";

	if (std::filesystem::exists(DesktopFilePath))
	{
		return;
	}

	std::ofstream DesktopFile = std::ofstream(DesktopFilePath);
	DesktopFile.exceptions(std::ios::failbit | std::ios::badbit);

	std::string FileContent = FileUtil::ReadFile("app/CyberpunkModManager.desktop");
	std::cout << FileContent << std::endl;
	std::cout << StrUtil::Replace(FileContent, "$appPath", GetCurrentProcessName()) << std::endl;

	DesktopFile << StrUtil::Replace(FileContent, "$appPath", GetCurrentProcessName());
	DesktopFile.close();

	system(("xdg-mime default CyberpunkModManager.desktop x-scheme-handler/nxm"));
}

void Windows::ErrorBox(std::string Content)
{
	system(("zenity --error --text \"" + StrUtil::Replace(Content, "\"", "\\\"") + "\"").c_str());
}

bool Windows::IsProcessRunning(std::string Name)
{
	std::string CurrentName = GetCurrentProcessName();
	for (auto& i : std::filesystem::directory_iterator("/proc"))
	{
		std::string FileNameString = i.path().filename().string();
		if (strspn(FileNameString.c_str(), "0123456789") == FileNameString.size())
		{
			int pid = std::stoi(FileNameString);

			if (pid != getpid() && GetProcessName(pid) == CurrentName)
			{
				return true;
			}
		}
	}
	return false;
}


std::string Windows::GetCurrentProcessName()
{
	return GetProcessName(getpid());
}

void Windows::Open(std::string Path)
{
	system(Path.c_str());
}

#endif
