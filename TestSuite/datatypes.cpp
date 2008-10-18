
#include "header.h"

#include "datatypes.h"

int TestDatatypes_Uint8()
{
	TEST_ASSERT(sizeof(cc_uint8_t) * 8 == 8);
	return 0;
}

int TestDatatypes_Uint16()
{
	TEST_ASSERT(sizeof(cc_uint16_t) * 8 == 16);
	return 0;
}

int TestDatatypes_Uint32()
{
	TEST_ASSERT(sizeof(cc_uint32_t) * 8 == 32);
	return 0;
}

int TestDatatypes_Uint64()
{
	TEST_ASSERT(sizeof(cc_uint64_t) * 8 == 64);
	return 0;
}

int TestDatatypes_Ulong()
{
	TEST_ASSERT(sizeof(cc_ulong_t) * 8 == TARGET_CPU_BITS);
	return 0;
}

int TestDatatypes_Bool()
{
	TEST_ASSERT(sizeof(cc_bool_t) * 8 == 8);
	return 0;
}

int TestDatatypes_Float()
{
	TEST_ASSERT(sizeof(cc_float32_t) * 8 == 32);
	return 0;
}

int TestDatatypes_Double()
{
	TEST_ASSERT(sizeof(cc_float64_t) * 8 == 64);
	return 0;
}

int TestDatatypes_Pointer()
{
	TEST_ASSERT(sizeof(cc_ptr_t) * 8 == TARGET_CPU_BITS);
	TEST_ASSERT(sizeof(cc_ptr_t) == sizeof(cc_ulong_t));
	return 0;
}
