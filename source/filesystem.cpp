/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2015 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/debug.h>
#include <crisscross/filesystem.h>

#ifdef TARGET_COMPILER_VC

#include <windows.h>

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

/*
 * Open directory stream DIRNAME for read and return a pointer to the
 * internal working area that is used to retrieve individual directory
 * entries.
 */
DIR *opendir(const char *dirname)
{
	DIR *dirp;
	CoreAssert(dirname);
	CoreAssert(strlen(dirname) < MAX_PATH);

	/* construct new DIR structure */
	dirp = (DIR *)malloc(sizeof(struct DIR));
	if (dirp != NULL) {
		char *p;

		/* take directory name... */
		strncpy(dirp->patt, dirname, sizeof(dirp->patt));
		dirp->patt[MAX_PATH] = '\0';

		/* ... and append search pattern to it */
		p = strchr(dirp->patt, '\0');
		if (dirp->patt < p  &&  *(p - 1) != '\\'  &&  *(p - 1) != ':') {
			*p++ = '\\';
		}
		*p++ = '*';
		*p = '\0';

		/* open stream and retrieve first file */
		dirp->search_handle = FindFirstFileA(dirp->patt, &dirp->current.data);
		if (dirp->search_handle == INVALID_HANDLE_VALUE) {
			/* invalid search pattern? */
			free(dirp);
			return NULL;
		}

		/* there is an un-processed directory entry in memory now */
		dirp->cached = 1;
	}
	return dirp;
}


/*
 * Read a directory entry, and return a pointer to a dirent structure
 * containing the name of the entry in d_name field.  Individual directory
 * entries returned by this very function include regular files,
 * sub-directories, pseudo-directories "." and "..", but also volume labels,
 * hidden files and system files may be returned.
 */
struct dirent *readdir(DIR *dirp)
{
	CoreAssert(dirp);

	if (dirp->search_handle == INVALID_HANDLE_VALUE) {
		/* directory stream was opened/rewound incorrectly or it ended normally */
		return NULL;
	}

	/* get next directory entry */
	if (dirp->cached != 0) {
		/* a valid directory entry already in memory */
		dirp->cached = 0;
	} else {
		/* read next directory entry from disk */
		if (FindNextFileA(dirp->search_handle, &dirp->current.data) == FALSE) {
			/* the very last file has been processed or an error occured */
			FindClose(dirp->search_handle);
			dirp->search_handle = INVALID_HANDLE_VALUE;
			return NULL;
		}
	}

	/* copy as a multibyte character string */
	strncpy(dirp->current.d_name, dirp->current.data.cFileName, sizeof(dirp->current.d_name));
	dirp->current.d_name[MAX_PATH] = '\0';

	return &dirp->current;
}


/*
 * Close directory stream opened by opendir() function.  Close of the
 * directory stream invalidates the DIR structure as well as any previously
 * read directory entry.
 */
int closedir(DIR *dirp)
{
	CoreAssert(dirp);

	/* release search handle */
	if (dirp->search_handle != INVALID_HANDLE_VALUE) {
		FindClose(dirp->search_handle);
		dirp->search_handle = INVALID_HANDLE_VALUE;
	}

	/* release directory handle */
	free(dirp);
	return 0;
}

#endif
