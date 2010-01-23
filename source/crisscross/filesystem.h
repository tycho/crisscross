/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_filesystem_h
#define __included_cc_filesystem_h

#ifdef TARGET_OS_WINDOWS

struct dirent;
struct DIR;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#else
#include <dirent.h>
#endif

#endif
