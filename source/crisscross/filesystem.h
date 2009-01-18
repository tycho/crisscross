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

typedef struct dirent {
	/* name of current directory entry (a multi-byte character string) */
	char d_name[MAX_PATH + 1];

	/* file attributes */
	WIN32_FIND_DATAA data;
} dirent;


typedef struct DIR {
	/* current directory entry */
	dirent current;

	/* is there an un-processed entry in current? */
	int cached;

	/* file search handle */
	HANDLE search_handle;

	/* search pattern (3 = zero terminator + pattern "\\*") */
	char patt[MAX_PATH + 3];
} DIR;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#else
#include <dirent.h>
#endif

#endif
