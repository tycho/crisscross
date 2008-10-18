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
#include "compare.h"
#include "testutils.h"

#include <crisscross/compare.h>

using namespace CrissCross;

int TestComparison_CString()
{
	char        *greaterString = cc_strdup("beta");
	char        *lesserString = cc_strdup("alpha");
	char        *dupLesserString = cc_strdup(lesserString);
	char * const constString = cc_strdup(dupLesserString);

	TEST_ASSERT(Data::Compare(lesserString, greaterString) == -1);
	TEST_ASSERT(Data::Compare(greaterString, lesserString) == 1);
	TEST_ASSERT(Data::Compare(lesserString, dupLesserString) == 0);
	TEST_ASSERT(Data::Compare<char *>(dupLesserString, constString) == 0);

	free((char *)constString);
	free(dupLesserString);
	free(lesserString);
	free(greaterString);

	return 0;
}

int TestComparison_String()
{
	std::string greaterString("beta");
	std::string lesserString("alpha");
	std::string dupLesserString(lesserString);

	TEST_ASSERT(Data::Compare(lesserString, greaterString) == -1);
	TEST_ASSERT(Data::Compare(greaterString, lesserString) == 1);
	TEST_ASSERT(Data::Compare(lesserString, dupLesserString) == 0);

	return 0;
}

int TestComparison_Integer()
{
	int greaterInt = 200;
	int lesserInt = -200;
	int dupLesserInt = lesserInt;

	TEST_ASSERT(Data::Compare(lesserInt, greaterInt) == -1);
	TEST_ASSERT(Data::Compare(greaterInt, lesserInt) == 1);
	TEST_ASSERT(Data::Compare(lesserInt, dupLesserInt) == 0);

	return 0;
}
