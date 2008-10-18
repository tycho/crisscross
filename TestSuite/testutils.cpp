/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of IO.IN Research.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"
#include "testutils.h"

using namespace CrissCross;

#ifndef TARGET_OS_NDSFIRMWARE
#define COLUMN_LIMIT 69
#else
#define COLUMN_LIMIT 54
#endif

void WritePrefix(const char *_prefix)
{
	char temp[128];
	sprintf(temp, "%s...", _prefix);
	while (strlen(temp) < COLUMN_LIMIT)
		strcat(temp, " ");
	g_console->Write(temp);
}

int WriteResult(int _result)
{
	g_console->Write("[ ");
	if (_result) {
		g_console->SetColour(IO::Console::FG_RED | IO::Console::FG_INTENSITY);
		g_console->Write("FAILED");
	} else {
		g_console->SetColour(IO::Console::FG_GREEN | IO::Console::FG_INTENSITY);
		g_console->Write("PASSED");
	}

	g_console->SetColour();
	g_console->WriteLine(" ]");

	return _result;
}

char *newStr(const char *_string)
{
	char *retval = new char[strlen(_string) + 1];
	strcpy(retval, _string);
	return retval;
}
