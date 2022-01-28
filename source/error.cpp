/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/error.h>

#include <cstring>
#include <errno.h>

#ifndef TARGET_OS_WINDOWS
#include <netdb.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif

namespace CrissCross
{
	struct tl
	{
		int w;
		const char *s;
		int e;
	};

	// TODO: Translate the possible errno values for CoreIOReader and CoreIOWriter
	const struct tl errmap [] =
	{
		{0, "NOERROR", CC_ERR_NONE},
		{0, "UNKNOWN_ERROR", CC_ERR_INTERNAL}
	};

	int TranslateError(int why)
	{
		for (int i = 0; errmap[i].s != nullptr; ++i)
			if (why == errmap[i].w)
				return errmap[i].e;
		return why;
	}

	const char *GetErrorDescription(int why)
	{
		static const char *unknown = "Unknown Error";

#if !defined (TARGET_OS_WINDOWS) && !defined (TARGET_COMPILER_CYGWIN)
		if (why >= 0) {
			static char buffer[128];
			memset(buffer, 0, sizeof(buffer));
			strerror_r(why, buffer, 128);
			if (strlen(buffer))
				return buffer;
		}

#endif

		for (int i = 0; errmap[i].e != CC_ERR_NONE; ++i)
			if (why == errmap[i].e)
				return errmap[i].s;

		return unknown;
	}
}
