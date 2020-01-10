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

#include <crisscross/universal_include.h>

#include "build_number.h"
#include <crisscross/version.h>

namespace CrissCross
{
	namespace Version
	{
		const char *Short()
		{
			return CC_LIB_VERSION_TAG;
		}

		const char *Long()
		{
			return CC_LIB_VERSION_LONG;
		}

		int Major()
		{
			return CC_LIB_VERSION_MAJOR;
		}

		int Minor()
		{
			return CC_LIB_VERSION_MINOR;
		}

		int Revision()
		{
			return CC_LIB_VERSION_REVISION;
		}

		int Build()
		{
			return CC_LIB_VERSION_BUILD;
		}
	}
}
