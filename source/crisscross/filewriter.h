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

#ifndef __included_cc_filewriter_h
#define __included_cc_filewriter_h

#include <crisscross/error.h>

namespace CrissCross
{
	namespace IO
	{
		/*! \brief The file output class. */
		/*!
		 *  Inherits CoreIO, so functions like CoreIO::Write and CoreIO::WriteLine are accessible to the programmer.
		 */
		class FileWriter : public CoreIOWriter
		{
			protected:
				/*! \brief The path to the file being written. */
				const char *m_filePath;

			public:
				/*! \brief The constructor. */
				FileWriter();

				/*! \brief The destructor */
				/*!
				 *  Flushes the output buffer, closes the file, and deallocates memory used by FileWriter.
				 */
				~FileWriter();

				/*! \brief Opens the provided file. */
				/*!
				 *  Opens the file for reading.
				 * \param _file The path of the file to be read.
				 * \param _writeMode The mode with which to open the file.
				 * \param _lnEnding The line ending type to use.
				 * \return Standard CrissCross::Errors values.
				 */
				CrissCross::Errors Open(const char *_file, FileWriteMode _writeMode = CC_FILE_CREATE, LineEndingType _lnEnding = CC_LN_NATIVE);

				/*! \brief Closes the file. */
				/*!
				 *  Flushes the output buffer, closes the file, and deallocates memory used by FileWriter.
				 * \return Standard CrissCross::Errors values.
				 */
				CrissCross::Errors Close();
		};
	}
}
#endif
