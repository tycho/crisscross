/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2018 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <crisscross/debug.h>
#include <crisscross/endian.h>
#include <crisscross/core_io.h>
#include <crisscross/system.h>

using namespace CrissCross::System;

namespace CrissCross
{
	namespace IO
	{
		CoreIOWriter::CoreIOWriter(FILE * _fileBuffer, bool _isUnicode, LineEndingType _lnEnding, Endian _outputEndianness)
		: m_fileOutputPointer(_fileBuffer),
		  m_unicode(_isUnicode)
		{
			SetLineEndings(_lnEnding);
			SetEndian(_outputEndianness);
		}

		CoreIOWriter::~CoreIOWriter()
		{
		}

		void CoreIOWriter::Flush()
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif
			fflush(m_fileOutputPointer);
		#ifdef TARGET_OS_NDSFIRMWARE
			swiWaitForVBlank();
		#endif
		}

		bool CoreIOWriter::IsOpen()
		{
			CoreAssert(this != NULL);

			if (m_fileOutputPointer == NULL)
				return false;
			else
				return true;
		}

		CrissCross::Errors CoreIOWriter::SetLineEndings(LineEndingType _ending)
		{
			CoreAssert(this != NULL);

			if (_ending == CC_LN_NATIVE) {
#if defined (TARGET_OS_WINDOWS)
				_ending = CC_LN_CRLF;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
				defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || defined (TARGET_OS_NDSFIRMWARE) || \
				defined (TARGET_OS_HAIKU)
				_ending = CC_LN_LF;
#else
#error You are not using a supported OS.
#endif
			}

			switch (_ending)
			{
			case CC_LN_CR:
				sprintf(m_lineEnding, "\r");
				break;
			case CC_LN_LF:
				sprintf(m_lineEnding, "\n");
				break;
			case CC_LN_CRLF:
				sprintf(m_lineEnding, "\r\n");
				break;
			default:
				return CC_ERR_BADPARAMETER;
			}
			return CC_ERR_NONE;
		}

		void CoreIOWriter::SetEndian(Endian _inputEndianness)
		{
			m_endianness = _inputEndianness;
		}

		CrissCross::Errors CoreIOWriter::WriteLine(const char *_format, ...)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			if (_format == NULL)
				return CC_ERR_BADPARAMETER;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			va_list args;

			va_start(args, _format);

			/* Print out the string */
			vfprintf(m_fileOutputPointer, _format, args);

			if (fprintf(m_fileOutputPointer, "%s", m_lineEnding) < 0)
				return CC_ERR_WRITE;

			va_end(args);

			Flush();

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::WriteLine(std::string &_string)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			if (fprintf(m_fileOutputPointer, "%s%s", _string.c_str(), m_lineEnding) < 0)
				return CC_ERR_WRITE;

			Flush();

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(std::string &_string)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			if (fprintf(m_fileOutputPointer, "%s", _string.c_str()) < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}


		CrissCross::Errors CoreIOWriter::WriteLine()
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			if (fprintf(m_fileOutputPointer, "%s", m_lineEnding) < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(const char *_format, ...)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			if (_format == NULL)
				return CC_ERR_BADPARAMETER;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			va_list args;

			va_start(args, _format);

			/* Print out the string */
			if (vfprintf(m_fileOutputPointer, _format, args) < 0)
				return CC_ERR_WRITE;

			fflush(m_fileOutputPointer);

			va_end(args);

			return CC_ERR_NONE;
		}

		int CoreIOWriter::WriteBlock(const void *_buffer, size_t _count)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return 0;

			if (_buffer == NULL)
				return 0;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif

			size_t ret = fwrite(_buffer, _count, 1, m_fileOutputPointer);

			return ret;
		}

		int CoreIOWriter::WriteU8(uint8_t _data)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return 0;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif
			size_t ret = fwrite(&_data, sizeof(uint8_t), 1, m_fileOutputPointer);

			return ret;
		}

		int CoreIOWriter::WriteU16(uint16_t _data)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return 0;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif
			switch (m_endianness)
			{
			case CC_ENDIAN_LITTLE:
				_data = CC_SwapBE16(_data);
				break;
			case CC_ENDIAN_BIG:
				_data = CC_SwapLE16(_data);
				break;
			case CC_ENDIAN_NATIVE:
				/* Do nothing */
				break;
			}
			size_t ret = fwrite(&_data, sizeof(uint16_t), 1, m_fileOutputPointer);

			return ret;
		}

		int CoreIOWriter::WriteU32(uint32_t _data)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return 0;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif
			switch (m_endianness)
			{
			case CC_ENDIAN_LITTLE:
				_data = CC_SwapBE32(_data);
				break;
			case CC_ENDIAN_BIG:
				_data = CC_SwapLE32(_data);
				break;
			case CC_ENDIAN_NATIVE:
				/* Do nothing */
				break;
			}
			size_t ret = fwrite(&_data, sizeof(uint32_t), 1, m_fileOutputPointer);

			return ret;
		}

		int CoreIOWriter::WriteU64(uint64_t _data)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return 0;

#ifndef __GNUC__
			MutexHolder mh(&m_ioMutex);
#endif
			switch (m_endianness)
			{
			case CC_ENDIAN_LITTLE:
				_data = CC_SwapBE64(_data);
				break;
			case CC_ENDIAN_BIG:
				_data = CC_SwapLE64(_data);
				break;
			case CC_ENDIAN_NATIVE:
				/* Do nothing */
				break;
			}
			size_t ret = fwrite(&_data, sizeof(uint64_t), 1, m_fileOutputPointer);

			return ret;
		}
	}
}
