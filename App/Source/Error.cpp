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
#endif

bool Crashed = false;

void SignalHandler(int)
{
	if (Crashed)
	{
		return;
	}
	Crashed = true;
	std::cout << "Crashed. Stack trace: " << std::endl;
#if HAS_CPP_STACKTRACE
	std::cout << std::stacktrace::current() << std::endl;
#else
	void* trace[32];
	char** messages = (char **)NULL;
	int i = 0, trace_size = 0;

	trace_size = backtrace(trace, 32);
	messages = backtrace_symbols(trace, trace_size);
	for (i = 1; i < trace_size; ++i)
	{
		printf("\tat: %s\n", messages[i]);
	}
	exit(1);
#endif
}

void Error::RegisterErrorHandler()
{
	signal(SIGSEGV, &SignalHandler);
	signal(SIGABRT, &SignalHandler);
}
