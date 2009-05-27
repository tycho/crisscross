
#include "header.h"

#include "sanity.h"

#ifdef TARGET_LITTLE_ENDIAN

int TestSanity_TypeBasedAliasing()
{
	unsigned long a;
	a = 4;
	*(unsigned short *)&a = 5;
	TEST_ASSERT(a == 5);
	return 0;
}

#endif
