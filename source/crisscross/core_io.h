/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_core_io_h
#define __included_cc_core_io_h

#include <crisscross/platform_detect.h>

namespace CrissCross
{
	namespace IO
	{
		/*! \brief Line ending conventions. */
		/*!
		 * An enumeration of different ending types.
		 */
		enum class LineEnding
		{
			Native,                     /*! \brief< Automatically selects the appropriate line ending for the running platform. */
			CR,                         /*! \brief< Carriage return only. (default for Commodore machines, Apple II family and Mac OS through version 9) */
			LF,                         /*! \brief< Line feed only. (default for UNIX and UNIX-like systems, Linux, AIX, Xenix, Mac OS X, BeOS, Amiga, RISC OS and others) */
			CRLF                        /*! \brief< Carriage return and line feed. (default for CP/M, MP/M, DOS, OS/2, Microsoft Windows) */
		};

		/*! \brief The endianness to convert from/to. */
		enum class Endian
		{
			Native,
			Little,
			Big
		};
	}
}

#include <crisscross/core_io_reader.h>
#include <crisscross/core_io_writer.h>

#endif
