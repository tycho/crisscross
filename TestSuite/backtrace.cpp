
#include "header.h"

#include "backtrace.h"

int TestBacktrace()
{
	CrissCross::Debug::PrintStackTrace(g_console);
	return 0;
}
