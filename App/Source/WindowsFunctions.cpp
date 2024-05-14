#include "WindowsFunctions.h"
#include <Windows.h>

#if _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

void Windows::Open(std::string Path)
{
	ShellExecuteA(0, 0, Path.c_str(), 0, 0, SW_HIDE);
}

void Windows::ShowDialogBox(std::string Content)
{
	MessageBoxA(0, Content.c_str(), "", MB_ICONERROR);
}
