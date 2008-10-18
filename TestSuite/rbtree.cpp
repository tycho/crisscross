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
#include "rbtree.h"
#include "testutils.h"

#include <crisscross/rbtree.h>

using namespace CrissCross::Data;

int TestRedBlackTree_CString()
{
	RedBlackTree<const char *, const char *> *rbtree = new RedBlackTree<const char *, const char *>();
	char                                     *tmp;

	TEST_ASSERT(rbtree->size() == 0);

	/* If the tree is properly encapsulated, this won't cause an error on test #1. */
	tmp = cc_strdup("first");
	rbtree->insert(tmp, "one");
	free(tmp); tmp = NULL;

	TEST_ASSERT(rbtree->size() == 1);

	rbtree->insert("second", "two");
	rbtree->insert("third", "three");
	rbtree->insert("fourth", "four");

	TEST_ASSERT(rbtree->size() == 4);

	const char *res = NULL;
	TEST_ASSERT(rbtree->find("first", res));

	const char *tmp1 = "one";
	TEST_ASSERT(Compare(res, tmp1) == 0);

	TEST_ASSERT(!rbtree->exists("fifth"));

	TEST_ASSERT(rbtree->exists("second"));
	TEST_ASSERT(rbtree->find("second", res));

	tmp1 = "two";
	TEST_ASSERT(Compare(res, tmp1) == 0);

	TEST_ASSERT(!rbtree->erase("fifth"));
	TEST_ASSERT(rbtree->size() == 4);

	TEST_ASSERT(rbtree->exists("first"));
	TEST_ASSERT(rbtree->erase("first"));
	TEST_ASSERT(rbtree->size() == 3);

	TEST_ASSERT(rbtree->exists("second"));
	TEST_ASSERT(rbtree->erase("second"));
	TEST_ASSERT(rbtree->size() == 2);

	TEST_ASSERT(rbtree->exists("third"));
	TEST_ASSERT(rbtree->erase("third"));
	TEST_ASSERT(rbtree->size() == 1);

	TEST_ASSERT(rbtree->exists("fourth"));
	TEST_ASSERT(rbtree->erase("fourth"));
	TEST_ASSERT(rbtree->size() == 0);

	delete rbtree;
	return 0;
}

int TestRedBlackTree_String()
{
	RedBlackTree<std::string, std::string> *rbtree = new RedBlackTree<std::string, std::string>();

	TEST_ASSERT(rbtree->size() == 0);

	rbtree->insert("first", "one");
	rbtree->insert("second", "two");
	rbtree->insert("third", "three");
	rbtree->insert("fourth", "four");

	std::string                             res;

	TEST_ASSERT(rbtree->size() == 4);

	TEST_ASSERT(rbtree->find("first", res));
	TEST_ASSERT(res == "one");

	TEST_ASSERT(!rbtree->exists("fifth"));

	TEST_ASSERT(rbtree->exists("second"));
	TEST_ASSERT(rbtree->find("second", res));

	TEST_ASSERT(res == "two");

	TEST_ASSERT(!rbtree->erase("fifth"));
	TEST_ASSERT(rbtree->size() == 4);

	TEST_ASSERT(rbtree->exists("first"));
	TEST_ASSERT(rbtree->erase("first"));
	TEST_ASSERT(rbtree->size() == 3);

	TEST_ASSERT(rbtree->exists("second"));
	TEST_ASSERT(rbtree->erase("second"));
	TEST_ASSERT(rbtree->size() == 2);

	TEST_ASSERT(rbtree->exists("third"));
	TEST_ASSERT(rbtree->erase("third"));
	TEST_ASSERT(rbtree->size() == 1);

	TEST_ASSERT(rbtree->exists("fourth"));
	TEST_ASSERT(rbtree->erase("fourth"));
	TEST_ASSERT(rbtree->size() == 0);

	delete rbtree;
	return 0;
}

int TestRedBlackTree_Int()
{
	RedBlackTree<int, int> *rbtree = new RedBlackTree<int, int>();

	TEST_ASSERT(rbtree->size() == 0);

	rbtree->insert(1, 1);
	rbtree->insert(2, 2);
	rbtree->insert(3, 3);
	rbtree->insert(4, 4);

	int                     res;
	TEST_ASSERT(rbtree->size() == 4);

	TEST_ASSERT(rbtree->find(1, res));
	TEST_ASSERT(res == 1);

	TEST_ASSERT(!rbtree->exists(5));

	TEST_ASSERT(rbtree->find(2, res));
	TEST_ASSERT(res == 2);

	TEST_ASSERT(!rbtree->erase(5));
	TEST_ASSERT(rbtree->size() == 4);

	TEST_ASSERT(rbtree->exists(1));
	TEST_ASSERT(rbtree->erase(1));
	TEST_ASSERT(rbtree->size() == 3);

	TEST_ASSERT(rbtree->exists(2));
	TEST_ASSERT(rbtree->erase(2));
	TEST_ASSERT(rbtree->size() == 2);

	TEST_ASSERT(rbtree->exists(3));
	TEST_ASSERT(rbtree->erase(3));
	TEST_ASSERT(rbtree->size() == 1);

	TEST_ASSERT(rbtree->exists(4));
	TEST_ASSERT(rbtree->erase(4));
	TEST_ASSERT(rbtree->size() == 0);

	delete rbtree;
	return 0;
}
