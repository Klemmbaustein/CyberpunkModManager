#include "Error.h"
#include <signal.h>
#include <iostream>

#if __cpp_lib_stacktrace >= 202011L
#define HAS_CPP_STACKTRACE 1
#endif

#if HAS_CPP_STACKTRACE
#include <stacktrace>
#else
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <cxxabi.h>
#endif
#include <map>

bool Crashed = false;
static std::map<int, const char*> SignalTypes =
{
	{ SIGABRT, "SIGABRT" },
	{ SIGFPE, "SIGFPE" },
	{ SIGILL, "SIGILL" },
	{ SIGINT, "SIGINT" },
	{ SIGSEGV, "SIGSEGV" },
	{ SIGTERM, "SIGTERM" },
};

void SignalHandler(int SigType)
{
	if (Crashed)
	{
		return;
	}
	Crashed = true;
	std::cout << "Crashed. " << SignalTypes[SigType] << " Stack trace: " << std::endl;
#if HAS_CPP_STACKTRACE
	std::cout << std::stacktrace::current() << std::endl;
#else

	void* trace[64];
	char** messages;

	int TraceSize = backtrace(trace, 64);
	messages = backtrace_symbols(trace, TraceSize);
	for (int i = 2; i < TraceSize; i++)
	{
		// Format: /path/to/binary(functionName()+0x1234) []
		std::string Entry = messages[i];

		try
		{
			std::string Name = Entry.substr(Entry.find_last_of("(") + 1);
			Name = Name.substr(0, Name.find_first_of(")"));
			size_t PlusLocation = Name.find_last_of("+");
			std::string SymbolName = Name.substr(0, PlusLocation);

			std::string EntryName = SymbolName;

			if (!EntryName.empty())
			{
				int st;
				char* DeMangledName = abi::__cxa_demangle
				(
					SymbolName.c_str(),
					nullptr,
					0,
					&st
				);

				if (DeMangledName)
				{
					EntryName = DeMangledName;
					free(DeMangledName);
				}
				else
				{
					EntryName = EntryName + "()";
				}
			}
			std::cout << ("    at: " + (EntryName.empty() ? Entry : EntryName)) << std::endl;
		}
		catch (std::exception)
		{
			std::cout << ("    at: " + Entry) << std::endl;
		}
	}
	free(messages);
	exit(1);
#endif
}

void Error::RegisterErrorHandler()
{
	signal(SIGSEGV, &SignalHandler);
	signal(SIGABRT, &SignalHandler);
	signal(SIGFPE, &SignalHandler);
}
