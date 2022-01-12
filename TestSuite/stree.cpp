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

#include "header.h"
#include "stree.h"
#include "testutils.h"

#include <cstdio>

#include <crisscross/stree.h>

using namespace CrissCross::Data;
using namespace CrissCross::System;

int TestSTree_CString()
{
	STree<const char *, const char *> *stree = new STree<const char *, const char *>();
	char *strings[TREE_ITEMS], *tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(stree != nullptr);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(stree->size() == 0);

	memset(strings, 0, sizeof(strings));

	/* Make sure the tree encapsulates keys properly */
	tmp = cc_strdup("testkey");
	stree->insert(tmp, "encapsulation test");
	free(tmp); tmp = nullptr;
	TEST_ASSERT(stree->size() == 1);
	TEST_ASSERT(stree->exists("testkey"));
	TEST_ASSERT(stree->erase("testkey"));
	TEST_ASSERT(!stree->exists("testkey"));
	TEST_ASSERT(!stree->erase("testkey"));
	TEST_ASSERT(stree->size() == 0);

	/* Simplest sanity checks done, now create some random data */
	for (unsigned int i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(strings[i] == nullptr);
		strings[i] = new char[20];
		TEST_ASSERT(strings[i] != nullptr);
		memset(strings[i], 0, 20);
		TEST_ASSERT(strlen(strings[i]) == 0);
		sprintf(strings[i], "%08x", i);
		TEST_ASSERT(strlen(strings[i]) > 0);
	}

	/* Fill the tree */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->insert(strings[i], strings[(TREE_ITEMS - 1) - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		const char *val = nullptr;
		TEST_ASSERT(stree->exists(strings[i]));
		TEST_ASSERT((val = stree->find(strings[i])) != nullptr);
		TEST_ASSERT(Compare(val, (const char *)strings[(TREE_ITEMS - 1) - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(stree->exists(strings[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->erase(strings[i]));
		TEST_ASSERT(!stree->exists(strings[i]));
	}

	/* Clean up the random data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		delete [] strings[i];
		strings[i] = nullptr;
	}

	/* And finally, clear the tree */
	delete stree;

	return 0;
}

int TestSTree_String()
{
	STree<std::string, std::string> *stree = new STree<std::string, std::string>();
	std::string strings[TREE_ITEMS], tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(stree != nullptr);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(stree->size() == 0);

	/* Make sure the tree encapsulates keys properly */
	tmp = std::string("testkey");
	stree->insert(tmp, std::string("encapsulation test"));
	tmp = std::string("");
	TEST_ASSERT(stree->size() == 1);
	TEST_ASSERT(stree->exists("testkey"));
	TEST_ASSERT(stree->erase("testkey"));
	TEST_ASSERT(!stree->exists("testkey"));
	TEST_ASSERT(!stree->erase("testkey"));
	TEST_ASSERT(stree->size() == 0);

	/* Simplest sanity checks done, now create some random data */
	for (unsigned int i = 0; i < TREE_ITEMS; i++)
	{
		char buf[20];
		buf[0] = (char)0;
		TEST_ASSERT(strlen(buf) == 0);
		sprintf(buf, "%08x", i);
		strings[i] = std::string(buf);
		TEST_ASSERT(strings[i].length() > 0);
	}

	/* Fill the tree */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->insert(strings[i], strings[(TREE_ITEMS - 1) - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		std::string val;
		val.clear();
		TEST_ASSERT(val.length() == 0);
		TEST_ASSERT(stree->exists(strings[i]));
		TEST_ASSERT((val = stree->find<std::string>(strings[i], "")) != std::string(""));
		TEST_ASSERT(val.length() > 0);
		TEST_ASSERT(Compare(val, strings[(TREE_ITEMS - 1) - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(stree->exists(strings[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->erase(strings[i]));
		TEST_ASSERT(!stree->exists(strings[i]));
	}

	/* And finally, clear the tree */
	delete stree;

	return 0;
}

int TestSTree_Int()
{
	STree<int, int> *stree = new STree<int, int>();
	int data[TREE_ITEMS], tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(stree != nullptr);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(stree->size() == 0);

	/* Make sure the tree encapsulates keys properly */
	tmp = 256;
	stree->insert(tmp, RandomNumber());
	tmp = 0;
	TEST_ASSERT(stree->size() == 1);
	TEST_ASSERT(stree->exists(256));
	TEST_ASSERT(stree->erase(256));
	TEST_ASSERT(!stree->exists(256));
	TEST_ASSERT(!stree->erase(256));
	TEST_ASSERT(stree->size() == 0);

	/* Simplest sanity checks done, now create some random data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		data[i] = i;
	}

	/* Fill the tree */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->insert(data[i], data[TREE_ITEMS - 1 - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		int val;
		TEST_ASSERT(stree->exists(data[i]));
		TEST_ASSERT((val = stree->find(data[i], -1)) != -1);
		TEST_ASSERT(Compare(val, data[TREE_ITEMS - 1 - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(stree->exists(data[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(stree->erase(data[i]));
		TEST_ASSERT(!stree->exists(data[i]));
	}

	/* And finally, clear the tree */
	delete stree;

	return 0;
}
