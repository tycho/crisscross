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

#ifndef __included_cc_string_utils_h
#define __included_cc_string_utils_h

char *cc_strdup(const char *x);
char *cc_newstr(const char *x);
size_t cc_strlcpy(char *dest, const char *source, size_t size);

#endif
