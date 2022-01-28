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
		CoreIOWriter::CoreIOWriter(FILE *_fileBuffer, bool _isUnicode, LineEnding _lnEnding, Endian _outputEndianness)
			: m_fileOutputPointer(_fileBuffer)
			, m_unicode(_isUnicode)
		{
			SetLineEndings(_lnEnding);
			SetEndian(_outputEndianness);
		}

		CoreIOWriter::~CoreIOWriter()
		{
		}

		void CoreIOWriter::Flush()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);
			if (!IsOpen())
				return;
			fflush(m_fileOutputPointer);
#ifdef TARGET_OS_NDSFIRMWARE
			swiWaitForVBlank();
#endif
		}

		bool CoreIOWriter::IsOpen()
		{
			if (m_fileOutputPointer == nullptr)
				return false;
			else
				return true;
		}

		CrissCross::Errors CoreIOWriter::SetLineEndings(LineEnding _ending)
		{
			if (_ending == LineEnding::Native) {
#if defined(TARGET_OS_WINDOWS)
				_ending = LineEnding::CRLF;
#else
				_ending = LineEnding::LF;
#endif
			}

			switch (_ending) {
			case LineEnding::CR:
				sprintf(m_lineEnding, "\r");
				break;
			case LineEnding::LF:
				sprintf(m_lineEnding, "\n");
				break;
			case LineEnding::CRLF:
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
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_format == nullptr)
				return CC_ERR_BADPARAMETER;

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
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

			if (fprintf(m_fileOutputPointer, "%s%s", _string.c_str(), m_lineEnding) < 0)
				return CC_ERR_WRITE;

			Flush();

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(std::string &_string)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

			if (fprintf(m_fileOutputPointer, "%s", _string.c_str()) < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::WriteLine()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (fprintf(m_fileOutputPointer, "%s", m_lineEnding) < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(const char *_format, ...)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_format == nullptr)
				return CC_ERR_BADPARAMETER;

			va_list args;

			va_start(args, _format);

			/* Print out the string */
			if (vfprintf(m_fileOutputPointer, _format, args) < 0)
				return CC_ERR_WRITE;

			fflush(m_fileOutputPointer);

			va_end(args);

			return CC_ERR_NONE;
		}

		size_t CoreIOWriter::WriteBlock(const void *_buffer, size_t _count)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;

			if (_buffer == nullptr)
				return 0;

			return fwrite(_buffer, _count, 1, m_fileOutputPointer);
		}

		size_t CoreIOWriter::WriteU8(uint8_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;
			return fwrite(&_data, sizeof(uint8_t), 1, m_fileOutputPointer);
		}

		size_t CoreIOWriter::WriteU16(uint16_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE16(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE16(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}
			return fwrite(&_data, sizeof(uint16_t), 1, m_fileOutputPointer);
		}

		size_t CoreIOWriter::WriteU32(uint32_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE32(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE32(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}
			return fwrite(&_data, sizeof(uint32_t), 1, m_fileOutputPointer);
		}

		size_t CoreIOWriter::WriteU64(uint64_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE64(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE64(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}
			return fwrite(&_data, sizeof(uint64_t), 1, m_fileOutputPointer);
		}
	}
}
