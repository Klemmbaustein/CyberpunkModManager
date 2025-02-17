#include "WindowsFunctions.h"
#include <iostream>
#include <filesystem>
#include <kui/KlemmUI.h>
#if _WIN32
#include <Windows.h>
#include <wtsapi32.h>
#include <StrUtil.h>
#include <Shlobj.h>
#include <shobjidl.h>
// clang-format off
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// clang-format on

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
	// For CMake builds, the executable wont be in the root directory (like rootDir/build/App instead of rootDir/app)
	if (std::filesystem::exists(PathString + L"/../../../App/app"))
	{
		PathString.append(L"/../../../App/");
	}
	else if (std::filesystem::exists(PathString + L"/../../App/app"))
	{
		PathString.append(L"/../../App/");
	}
	if (!std::filesystem::is_directory(PathString + L"/app"))
	{
		PathString.append(L"/../");
	}
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

	for (DWORD i = 0; i < pCount; i++)
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

bool Windows::YesNoBox(std::string Content)
{
	int Answer = MessageBoxA(0, Content.c_str(), "Cyberpunk Mod Manager", MB_YESNO);

	return Answer == IDYES;
}

static void ErrorMessageFromStatus(LSTATUS Status)
{
	LPWSTR OutString;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, Status, 0, (LPWSTR)&OutString, 0, NULL);
	MessageBoxW(NULL, OutString, L"Error", MB_ICONERROR);
}

static std::string wstrtostr(const std::wstring& wstr)
{
	std::string strTo;
	char* szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}

std::string Windows::OpenFileDialog(bool PickFolders)
{
	try
	{
		std::string FilePath = "";
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
			if (PickFolders)
			{
				pFileOpen->SetOptions(FOS_PICKFOLDERS);
			}

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);
				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							FilePath = wstrtostr(pszFilePath);
							return FilePath;
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		return FilePath;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return "";
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
	RegSetValueW(CommandKey, NULL, REG_SZ, (ExePath + L" uri=%1").c_str(), DWORD((ExePath.size() + 1) * sizeof(wchar_t)));
	RegCloseKey(ClassesKey);
	RegCloseKey(NxmKey);
	RegCloseKey(CommandKey);
}
#else
#include "StrUtil.h"
#include "FileUtil.h"
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <pwd.h>

static std::string GetProcessName(int pid)
{
	FILE* SelfLinkCommand = popen(("readlink /proc/" + std::to_string(pid) + "/exe").c_str(), "r");

	if (SelfLinkCommand == nullptr)
	{
		std::cout << "failed to call popen() to read process name." << std::endl;
		return "";
	}

	while (!feof(SelfLinkCommand))
	{
		char ExecutableBuffer[8000];
		size_t Read = fread(ExecutableBuffer, 1, sizeof(ExecutableBuffer) - 1, SelfLinkCommand);
		ExecutableBuffer[Read] = 0;
		if (Read > 0 && ExecutableBuffer[Read - 1] == '\n')
		{
			ExecutableBuffer[Read - 1] = 0;
		}
		pclose(SelfLinkCommand);
		return ExecutableBuffer;
	}
	pclose(SelfLinkCommand);
	return "";
}

void Windows::SetWorkingDirectory()
{
	std::string PathString = GetProcessName(getpid());

	PathString = PathString.substr(0, PathString.find_last_of("/\\"));
	if (std::filesystem::exists(PathString + "/../../App/app/shaders"))
	{
		PathString.append("/../../App/");
	}
	else if (std::filesystem::exists(PathString + "/../App/app/shaders"))
	{
		PathString.append("/../App/");
	}
	if (!std::filesystem::is_directory(PathString + "/app"))
	{
		// For CMake builds, the executable wont be in the root directory (like rootDir/build/App instead of rootDir/app)
		PathString.append("/../");
	}

	std::filesystem::current_path(PathString);
}

void Windows::RegisterSelfAsUriHandler()
{
	auto uid = geteuid();
	auto pw = getpwuid(uid);
	std::string DesktopFilePath = std::string(pw->pw_dir) + "/.local/share/applications/CyberpunkModManager.desktop";
	std::ofstream DesktopFile = std::ofstream(DesktopFilePath);

	std::string FileContent = FileUtil::ReadFile("app/CyberpunkModManager.desktop");
	DesktopFile << StrUtil::Replace(FileContent, "$appPath", GetCurrentProcessName());
	DesktopFile.close();

	system(("xdg-mime default CyberpunkModManager.desktop x-scheme-handler/nxm"));
}

constexpr char* APP_NAME = "Cyberpunk 2077 mod manager";

void Windows::ErrorBox(std::string Content)
{
	kui::app::MessageBox(Content, APP_NAME, kui::app::MessageBoxType::Error);
}

bool Windows::YesNoBox(std::string Content)
{
	return kui::app::YesNoBox(Content, APP_NAME);
}

std::string Windows::OpenFileDialog(bool PickFolders)
{
	return kui::app::SelectFileDialog(PickFolders);
}

bool Windows::IsProcessRunning(std::string Name)
{
	try
	{
		std::string CurrentName = GetCurrentProcessName();
		for (auto& i : std::filesystem::directory_iterator("/proc"))
		{
			if (!std::filesystem::is_directory(i))
			{
				continue;
			}

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
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
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
