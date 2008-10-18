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
#include "splaytree.h"
#include "testutils.h"

#include <crisscross/splaytree.h>

using namespace CrissCross::Data;

int TestSplayTree_CString()
{
	SplayTree<const char *, const char *> *splaytree = new SplayTree<const char *, const char *>();
	char                                  *tmp;

	TEST_ASSERT(splaytree->size() == 0);

	/* If the tree is properly encapsulated, this won't cause an error on test #1. */
	tmp = cc_strdup("first");
	splaytree->insert(tmp, "one");
	free(tmp); tmp = NULL;

	TEST_ASSERT(splaytree->size() == 1);

	splaytree->insert("second", "two");
	splaytree->insert("third", "three");
	splaytree->insert("fourth", "four");

	TEST_ASSERT(splaytree->size() == 4);

	const char *res = NULL;
	TEST_ASSERT(splaytree->find("first", res));

	const char *tmp1 = "one";
	TEST_ASSERT(Compare(res, tmp1) == 0);

	TEST_ASSERT(!splaytree->exists("fifth"));

	TEST_ASSERT(splaytree->exists("second"));
	TEST_ASSERT(splaytree->find("second", res));

	tmp1 = "two";
	TEST_ASSERT(Compare(res, tmp1) == 0);

	TEST_ASSERT(!splaytree->erase("fifth"));
	TEST_ASSERT(splaytree->size() == 4);

	TEST_ASSERT(splaytree->exists("first"));
	TEST_ASSERT(splaytree->erase("first"));
	TEST_ASSERT(splaytree->size() == 3);

	TEST_ASSERT(splaytree->exists("second"));
	TEST_ASSERT(splaytree->erase("second"));
	TEST_ASSERT(splaytree->size() == 2);

	TEST_ASSERT(splaytree->exists("third"));
	TEST_ASSERT(splaytree->erase("third"));
	TEST_ASSERT(splaytree->size() == 1);

	TEST_ASSERT(splaytree->exists("fourth"));
	TEST_ASSERT(splaytree->erase("fourth"));
	TEST_ASSERT(splaytree->size() == 0);

	delete splaytree;
	return 0;
}

int TestSplayTree_String()
{
	SplayTree<std::string, std::string> *splaytree = new SplayTree<std::string, std::string>();

	TEST_ASSERT(splaytree->size() == 0);

	splaytree->insert("first", "one");
	splaytree->insert("second", "two");
	splaytree->insert("third", "three");
	splaytree->insert("fourth", "four");

	std::string                          res;

	TEST_ASSERT(splaytree->size() == 4);

	TEST_ASSERT(splaytree->find("first", res));
	TEST_ASSERT(res == "one");

	TEST_ASSERT(!splaytree->exists("fifth"));

	TEST_ASSERT(splaytree->exists("second"));
	TEST_ASSERT(splaytree->find("second", res));

	TEST_ASSERT(res == "two");

	TEST_ASSERT(!splaytree->erase("fifth"));
	TEST_ASSERT(splaytree->size() == 4);

	TEST_ASSERT(splaytree->exists("first"));
	TEST_ASSERT(splaytree->erase("first"));
	TEST_ASSERT(splaytree->size() == 3);

	TEST_ASSERT(splaytree->exists("second"));
	TEST_ASSERT(splaytree->erase("second"));
	TEST_ASSERT(splaytree->size() == 2);

	TEST_ASSERT(splaytree->exists("third"));
	TEST_ASSERT(splaytree->erase("third"));
	TEST_ASSERT(splaytree->size() == 1);

	TEST_ASSERT(splaytree->exists("fourth"));
	TEST_ASSERT(splaytree->erase("fourth"));
	TEST_ASSERT(splaytree->size() == 0);

	delete splaytree;
	return 0;
}

int TestSplayTree_Int()
{
	SplayTree<int, int> *splaytree = new SplayTree<int, int>();

	TEST_ASSERT(splaytree->size() == 0);

	splaytree->insert(1, 1);
	splaytree->insert(2, 2);
	splaytree->insert(3, 3);
	splaytree->insert(4, 4);

	int                  res;
	TEST_ASSERT(splaytree->size() == 4);

	TEST_ASSERT(splaytree->find(1, res));
	TEST_ASSERT(res == 1);

	TEST_ASSERT(!splaytree->exists(5));

	TEST_ASSERT(splaytree->find(2, res));
	TEST_ASSERT(res == 2);

	TEST_ASSERT(!splaytree->erase(5));
	TEST_ASSERT(splaytree->size() == 4);

	TEST_ASSERT(splaytree->exists(1));
	TEST_ASSERT(splaytree->erase(1));
	TEST_ASSERT(splaytree->size() == 3);

	TEST_ASSERT(splaytree->exists(2));
	TEST_ASSERT(splaytree->erase(2));
	TEST_ASSERT(splaytree->size() == 2);

	TEST_ASSERT(splaytree->exists(3));
	TEST_ASSERT(splaytree->erase(3));
	TEST_ASSERT(splaytree->size() == 1);

	TEST_ASSERT(splaytree->exists(4));
	TEST_ASSERT(splaytree->erase(4));
	TEST_ASSERT(splaytree->size() == 0);

	delete splaytree;
	return 0;
}
