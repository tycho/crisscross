/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_version_h
#define __included_cc_version_h

namespace CrissCross
{
	namespace Version
	{
		//! The short version string.
		/*!
		 * The short version format is %d.%d.%d (<major>.<minor>.<revision>).
		 * \return The version number of the CrissCross library.
		 */
		const char *ShortVersion();

		//! The long version string.
		/*!
		 * The long version can currently be in a couple different forms, and
		 * the formats may change across versions, so don't make much effort to
		 * parse these out:
		 *    %d.%d.%d-%d-g%07x (<major>.<minor>.<revision>-<build>-g<commit id>)
		 *    %d.%d.%d-rc%d-%d-g%07x (<major>.<minor>.<revision>-rc<rc number>-<build>-g<commit id>)
		 * \return The version number of the CrissCross library.
		 */
		const char *LongVersion();

		/*!
		 * \return The major version number (i.e. in v1.2.3.4, it would be '1').
		 */
		int Major();

		/*!
		 * \return The minor version number (i.e. in v1.2.3.4, it would be '2').
		 */
		int Minor();

		/*!
		 * \return The revision version number (i.e. in v1.2.3.4, it would be '3').
		 */
		int Revision();

		/*!
		 * \return The build number (i.e. in v1.2.3.4, it would be '4').
		 */
		int Build();
	}
}

#endif
