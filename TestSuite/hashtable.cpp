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
#include "hashtable.h"

#include <crisscross/hashtable.h>

using namespace CrissCross::Data;

int TestHashTable_Int()
{
	HashTable<unsigned long, unsigned long> ht;

	const unsigned long                     max = 1000;
	unsigned long                           i;
	for (i = 0; i < max; i++) {
		ht.insert(i, max - i);
	}

	for (i = 0; i < max; i += 2) {
		TEST_ASSERT(ht.find(i) != 0);
	}

	for (i = 0; i < max; i += 2) {
		TEST_ASSERT(ht.erase(i));
	}

	for (i = 0; i < max; i += 2) {
		TEST_ASSERT(ht.find(i) == 0);
	}

	for (i = 1; i < max; i += 2) {
		TEST_ASSERT(ht.find(i) == max - i);
	}

	return 0;
}

int TestHashTable_CString()
{
	HashTable<const char *, const char *> ht;

	char                                  buffer1[32];
	char                                  buffer2[32];
	const unsigned long                   max = 1000;
	unsigned long                         i;

	for (i = 0; i < max; i++) {
		sprintf(buffer1, "%lu", i);
		sprintf(buffer2, "%lu", max - i);
		ht.insert(buffer1, (const char *)cc_strdup(buffer2));
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.find(buffer1) != 0);
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		free((void *)ht.find(buffer1));
		TEST_ASSERT(ht.erase(buffer1));
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
	for (i = 1; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		free((void *)ht.find(buffer1));
		TEST_ASSERT(ht.erase(buffer1));
	}

	return 0;
}

int TestHashTable_String()
{
	HashTable<std::string, std::string> ht;

	char                                buffer1[32];
	char                                buffer2[32];
	const unsigned long                 max = 1000;
	unsigned long                       i;

	for (i = 0; i < max; i++) {
		sprintf(buffer1, "%lu", i);
		sprintf(buffer2, "%lu", max - i);
		ht.insert(std::string(buffer1), std::string(buffer2));
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.exists(std::string(buffer1)));
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.erase(std::string(buffer1)));
	}

	for (i = 0; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(!ht.exists(std::string(buffer1)));
	}

	std::string rec;
	for (i = 1; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		sprintf(buffer2, "%lu", max - i);
		TEST_ASSERT(ht.find(std::string(buffer1), rec));
		TEST_ASSERT(std::string(rec) == std::string(buffer2));
	}

	/* Rest of the cleanup */
	for (i = 1; i < max; i += 2) {
		sprintf(buffer1, "%lu", i);
		TEST_ASSERT(ht.erase(std::string(buffer1)));
	}

	return 0;
}
