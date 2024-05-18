#define _GLIBCXX_HAVE_STACKTRACE
#include "Error.h"
#include <signal.h>
#include <iostream>

#if __cpp_lib_stacktrace >= 202011L
#define HAS_CPP_STACKTRACE 1
#endif

#if HAS_CPP_STACKTRACE
#include <stacktrace>
#endif

void SignalHandler(int Signal)
{
	std::cout << "Crashed. Stack trace: " << std::endl;
#if HAS_CPP_STACKTRACE
	std::cout << std::stacktrace::current() << std::endl;
#endif
}

void Error::RegisterErrorHandler()
{
	signal(SIGSEGV, &SignalHandler);
	signal(SIGABRT, &SignalHandler);
}
