#if _WIN32
#include "WindowsFunctions.h"
#include <Windows.h>
#include <wtsapi32.h>
#include <filesystem>
#include <StrUtil.h>
#include <iostream>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

std::wstring GetExecutableDir()
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
	RegStatus = RegCreateKeyW(NxmKey, L"shell\\open\\command", &CommandKey);
	if (RegStatus)
	{
		ErrorMessageFromStatus(RegStatus);
		return;
	}
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
#endif
