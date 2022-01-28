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

size_t cc_strlcpy(char *dest, const char *source, size_t size)
{
	char *d = dest;
	const char *s = source;
	size_t n = size;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dest, add NUL and traverse rest of source */
	if (n == 0) {
		if (size != 0)
			*d = '\0'; /* NUL-terminate dest */
		while (*s++)
			;
	}

	return (s - source - 1); /* count does not include NUL */
}
