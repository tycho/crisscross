
#include "header.h"

#include "datatypes.h"

int TestDatatypes_Uint8()
{
	TEST_ASSERT(sizeof(uint8_t) * 8 == 8);
	return 0;
}

int TestDatatypes_Uint16()
{
	TEST_ASSERT(sizeof(uint16_t) * 8 == 16);
	return 0;
}

int TestDatatypes_Uint32()
{
	TEST_ASSERT(sizeof(uint32_t) * 8 == 32);
	return 0;
}

int TestDatatypes_Uint64()
{
	TEST_ASSERT(sizeof(uint64_t) * 8 == 64);
	return 0;
}

int TestDatatypes_Ulong()
{
	TEST_ASSERT(sizeof(long) * 8 == TARGET_CPU_BITS);
	return 0;
}

int TestDatatypes_Bool()
{
#if defined(TARGET_OS_MACOSX) && defined(TARGET_CPU_PPC)
	TEST_ASSERT(sizeof(bool) * 8 == 32);
#else
	TEST_ASSERT(sizeof(bool) * 8 == 8);
#endif
	return 0;
}

int TestDatatypes_Float()
{
	TEST_ASSERT(sizeof(float) * 8 == 32);
	return 0;
}

int TestDatatypes_Double()
{
	TEST_ASSERT(sizeof(double) * 8 == 64);
	return 0;
}

int TestDatatypes_Pointer()
{
	TEST_ASSERT(sizeof(uintptr_t) * 8 == TARGET_CPU_BITS);
	TEST_ASSERT(sizeof(uintptr_t) == sizeof(long));
	return 0;
}
