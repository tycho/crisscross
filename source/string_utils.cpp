/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/string_utils.h>

#include <cstring>
#include <cstdlib>

char *cc_strdup(const char *x)
{
	if (!x) return nullptr;

	char *dup = (char *)malloc(strlen(x) + 1);
	if (!dup) return nullptr;

	return strcpy(dup, x);
}

char *cc_newstr(const char *x)
{
	if (!x) return nullptr;

	char *dup = new char[strlen(x) + 1];
	if (!dup) return nullptr;

	return strcpy(dup, x);
}
