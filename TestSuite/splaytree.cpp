/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"
#include "splaytree.h"
#include "testutils.h"

#include <cstdio>

#include <crisscross/splaytree.h>

using namespace CrissCross::Data;
using namespace CrissCross::System;

int TestSplayTree_CString()
{
	SplayTree<const char *, const char *> *splaytree = new SplayTree<const char *, const char *>();
	char *strings[TREE_ITEMS], *tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(splaytree != NULL);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(splaytree->size() == 0);

	memset(strings, 0, sizeof(strings));

	/* Make sure the tree encapsulates keys properly */
	tmp = cc_strdup("testkey");
	splaytree->insert(tmp, "encapsulation test");
	free(tmp); tmp = NULL;
	TEST_ASSERT(splaytree->size() == 1);
	TEST_ASSERT(splaytree->exists("testkey"));
	TEST_ASSERT(splaytree->erase("testkey"));
	TEST_ASSERT(!splaytree->exists("testkey"));
	TEST_ASSERT(!splaytree->erase("testkey"));
	TEST_ASSERT(splaytree->size() == 0);

	/* Simplest sanity checks done, now create some random data */
	for (unsigned int i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(strings[i] == NULL);
		strings[i] = new char[20];
		TEST_ASSERT(strings[i] != NULL);
		memset(strings[i], 0, 20);
		TEST_ASSERT(strlen(strings[i]) == 0);
		sprintf(strings[i], "%08x", i);
		TEST_ASSERT(strlen(strings[i]) > 0);
	}

	/* Fill the tree */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(splaytree->insert(strings[i], strings[(TREE_ITEMS - 1) - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		const char *val = NULL;
		TEST_ASSERT(splaytree->exists(strings[i]));
		TEST_ASSERT((val = splaytree->find(strings[i])) != NULL);
		TEST_ASSERT(Compare(val, (const char *)strings[(TREE_ITEMS - 1) - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(splaytree->exists(strings[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(splaytree->erase(strings[i]));
		TEST_ASSERT(!splaytree->exists(strings[i]));
	}

	/* Clean up the random data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		delete [] strings[i];
		strings[i] = NULL;
	}

	/* And finally, clear the tree */
	delete splaytree;

	return 0;
}

int TestSplayTree_String()
{
	SplayTree<std::string, std::string> *splaytree = new SplayTree<std::string, std::string>();
	std::string strings[TREE_ITEMS], tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(splaytree != NULL);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(splaytree->size() == 0);

	/* Make sure the tree encapsulates keys properly */
	tmp = std::string("testkey");
	splaytree->insert(tmp, std::string("encapsulation test"));
	tmp = std::string("");
	TEST_ASSERT(splaytree->size() == 1);
	TEST_ASSERT(splaytree->exists("testkey"));
	TEST_ASSERT(splaytree->erase("testkey"));
	TEST_ASSERT(!splaytree->exists("testkey"));
	TEST_ASSERT(!splaytree->erase("testkey"));
	TEST_ASSERT(splaytree->size() == 0);

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
		TEST_ASSERT(splaytree->insert(strings[i], strings[(TREE_ITEMS - 1) - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		std::string val;
		val.clear();
		TEST_ASSERT(val.length() == 0);
		TEST_ASSERT(splaytree->exists(strings[i]));
		TEST_ASSERT((val = splaytree->find<std::string>(strings[i], "")) != std::string(""));
		TEST_ASSERT(val.length() > 0);
		TEST_ASSERT(Compare(val, strings[(TREE_ITEMS - 1) - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(splaytree->exists(strings[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(splaytree->erase(strings[i]));
		TEST_ASSERT(!splaytree->exists(strings[i]));
	}

	/* And finally, clear the tree */
	delete splaytree;

	return 0;
}

int TestSplayTree_Int()
{
	SplayTree<int, int> *splaytree = new SplayTree<int, int>();
	int data[TREE_ITEMS], tmp;

	/* Make sure allocation worked */
	TEST_ASSERT(splaytree != NULL);

	/* Make sure the size starts at 0 */
	TEST_ASSERT(splaytree->size() == 0);

	/* Make sure the tree encapsulates keys properly */
	tmp = 256;
	splaytree->insert(tmp, RandomNumber());
	tmp = 0;
	TEST_ASSERT(splaytree->size() == 1);
	TEST_ASSERT(splaytree->exists(256));
	TEST_ASSERT(splaytree->erase(256));
	TEST_ASSERT(!splaytree->exists(256));
	TEST_ASSERT(!splaytree->erase(256));
	TEST_ASSERT(splaytree->size() == 0);

	/* Simplest sanity checks done, now create some random data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		data[i] = i;
	}

	/* Fill the tree */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(splaytree->insert(data[i], data[TREE_ITEMS - 1 - i]));
	}

	/* Verify existence of all the added data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		int val;
		TEST_ASSERT(splaytree->exists(data[i]));
		TEST_ASSERT((val = splaytree->find(data[i], -1)) != -1);
		TEST_ASSERT(Compare(val, data[TREE_ITEMS - 1 - i]) == 0);
	}

	/* Verify existence of all the added data, in a different order */
	for (size_t i = TREE_ITEMS - 1; i < TREE_ITEMS; i--)
	{
		TEST_ASSERT(splaytree->exists(data[i]));
	}

	/* Try to remove all the data */
	for (size_t i = 0; i < TREE_ITEMS; i++)
	{
		TEST_ASSERT(splaytree->erase(data[i]));
		TEST_ASSERT(!splaytree->exists(data[i]));
	}

	/* And finally, clear the tree */
	delete splaytree;

	return 0;
}
