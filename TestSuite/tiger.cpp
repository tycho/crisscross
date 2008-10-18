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
#include "tiger.h"

using namespace CrissCross::Crypto;

int TestTiger()
{
	TigerHash   tiger;

	const char *teststring = NULL;
	size_t      length = 0;

	/* These tests are from http://www.cs.technion.ac.il/~biham/Reports/Tiger/tiger2-test-vectors-nessie-format.dat */

	teststring = ""; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "3293ac630c13f0245f92bbb1766e16167a4e58492dde73f3") == 0);

	teststring = "a"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "77befbef2e7ef8ab2ec8f93bf587a7fc613e247f5f247809") == 0);

	teststring = "abc"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "2aab1484e8c158f2bfb8c5ff41b57a525129131c957b5f93") == 0);

	teststring = "message digest"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "d981f8cb78201a950dcf3048751e441c517fca1aa55a29f6") == 0);

	teststring = "abcdefghijklmnopqrstuvwxyz"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "1714a472eee57d30040412bfcc55032a0b11602ff37beee9") == 0);

	teststring = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "8dcea680a17583ee502ba38a3c368651890ffbccdc49a8cc") == 0);

	teststring = "12345678901234567890123456789012345678901234567890123456789012345678901234567890"; length = strlen(teststring);
	tiger.Process(teststring, length);
	TEST_ASSERT(strcmp(tiger.ToString(), "1c14795529fd9f207a958f84c52f11e887fa0cabdfd91bfd") == 0);

	TigerHash otherhash;
	otherhash.Process("cheese", 6);
	TEST_ASSERT(otherhash != tiger && tiger != otherhash);

	otherhash.Process(teststring, length);
	TEST_ASSERT(otherhash == tiger && tiger == otherhash);

	return 0;
}
