/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>

#include <crisscross/version.h>

namespace CrissCross
{
	namespace Version
	{
		const char *Version()
		{
			return CC_LIB_VERSION;
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
