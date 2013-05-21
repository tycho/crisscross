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

#include "header.h"
#include "hashtable.h"

#include <cstdio>

#include <crisscross/hashtable.h>

using namespace CrissCross::Data;

int TestHashTable_CString()
{
	HashTable<const char *> ht;
	DArray<char *> keys;

	char buffer1[32];
	char buffer2[32];
	const unsigned long max = 1 << 14;
	unsigned long i;

	for (i = 0; i < max; i++) {
		char *key;
		sprintf(buffer1, "%lu", i);
		sprintf(buffer2, "%lu", max - i);
		key = cc_strdup(buffer1);
		keys.insert(key);
		ht.insert(key, (const char *)cc_strdup(buffer2));
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.find(buffer1) != 0);
	}

	for (i = 0; i < keys.size(); i+=2) {
		if (!keys.valid(i)) continue;
		char *key = keys[i];
		keys.remove(i);
		free((void *)ht.find(key));
		TEST_ASSERT(ht.erase(key));
		Dealloc(key);
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.find(buffer1) == 0);
	}

	for (i = 1; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		sprintf(buffer2, "%lu", max - i);
		TEST_ASSERT(strcmp(ht.find(buffer1), buffer2) == 0);
	}

	/* Rest of the cleanup */
	for (i = 0; i < keys.size(); i++) {
		if (!keys.valid(i)) continue;
		char *key = keys[i];
		keys.remove(i);
		free((void *)ht.find(key));
		TEST_ASSERT(ht.erase(key));
		Dealloc(key);
	}

	return 0;
}
