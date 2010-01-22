/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <cstring>

char *cc_strdup(const char *x)
{
	if (!x) return NULL;

	char *dup = (char *)malloc(strlen(x) + 1);
	if (!dup) return NULL;

	return strcpy(dup, x);
}

char *cc_newstr(const char *x)
{
	if (!x) return NULL;

	char *dup = new char[strlen(x) + 1];
	if (!dup) return NULL;

	return strcpy(dup, x);
}
