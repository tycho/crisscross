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

#ifndef __included_cc_core_io_h
#define __included_cc_core_io_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif

namespace CrissCross
{
	namespace IO
	{
		/*! \brief File write modes. */
		/*!
		 * Provides the choice of creating a file when opening it or appending to it.
		 */
		enum FileWriteMode
		{
			CC_FILE_CREATE,                   /*! \brief< Default. Will create a new file if the one opened does not exist. */
			CC_FILE_APPEND                    /*! \brief< Write operations will append data to the end of the file. */
		};

		/*! \brief Line ending conventions. */
		/*!
		 * An enumeration of different ending types.
		 */
		enum LineEndingType
		{
			CC_LN_NATIVE,                     /*! \brief< Automatically selects the appropriate line ending for the running platform. */
			CC_LN_CR,                         /*! \brief< Carriage return only. (default for Commodore machines, Apple II family and Mac OS through version 9) */
			CC_LN_LF,                         /*! \brief< Line feed only. (default for UNIX and UNIX-like systems, Linux, AIX, Xenix, Mac OS X, BeOS, Amiga, RISC OS and others) */
			CC_LN_CRLF                        /*! \brief< Carriage return and line feed. (default for CP/M, MP/M, DOS, OS/2, Microsoft Windows) */
		};
	}
}

#include <crisscross/core_io_reader.h>
#include <crisscross/core_io_writer.h>

#endif
