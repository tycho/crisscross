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

#include <cstdio>
#include <cstring>

#include <crisscross/endian.h>
#include <crisscross/debug.h>
#include <crisscross/core_io.h>
#include <crisscross/string_utils.h>
#include <crisscross/system.h>

using namespace CrissCross::System;

namespace CrissCross
{
	namespace IO
	{
		CoreIOReader::CoreIOReader(FILE *_fileBuffer, LineEnding _lnEnding, Endian _inputEndianness)
			: m_fileInputPointer(_fileBuffer)
		{
			SetLineEndings(_lnEnding);
			SetEndian(_inputEndianness);
		}

		CoreIOReader::~CoreIOReader()
		{
		}

		bool CoreIOReader::EndOfFile()
		{
			if (!m_fileInputPointer)
				return true;

			return (feof(m_fileInputPointer) != 0);
		}

		void CoreIOReader::Flush()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);
			if (!IsOpen())
				return;
			fflush(m_fileInputPointer);
		}

		bool CoreIOReader::IsOpen()
		{
			if (m_fileInputPointer == nullptr)
				return false;
			else
				return true;
		}

		int CoreIOReader::Forward(int64_t _position)
		{
			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			int res = Seek(_position, SEEK_CUR);
			return (res == 0);
		}

		int64_t CoreIOReader::Position()
		{
			CoreAssert(IsOpen());
#ifdef HAS_FPOS64
			fpos64_t lastpos;
#if defined(TARGET_COMPILER_VC) || defined(TARGET_COMPILER_BORLAND) || (defined(TARGET_COMPILER_ICC) && defined(TARGET_OS_WINDOWS))
			lastpos = _ftelli64(m_fileInputPointer);
			return lastpos;
#elif defined(TARGET_OS_MACOSX) || defined(TARGET_OS_NETBSD) || defined(TARGET_OS_FREEBSD) || defined(TARGET_OS_OPENBSD) || defined(TARGET_COMPILER_MINGW)
			fgetpos(m_fileInputPointer, &lastpos);
			return lastpos;
#else
			fgetpos64(m_fileInputPointer, &lastpos);
			return lastpos.__pos;
#endif
#else
			fpos_t lastpos;
			lastpos = ftell(m_fileInputPointer);
			return lastpos;
#endif
		}

		int64_t CoreIOReader::Length()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			CoreAssert(IsOpen());

#ifdef HAS_FPOS64
			fpos64_t lastpos, endpos;
#if defined(TARGET_COMPILER_VC) || defined(TARGET_COMPILER_BORLAND) || (defined(TARGET_COMPILER_ICC) && defined(TARGET_OS_WINDOWS))
			lastpos = _ftelli64(m_fileInputPointer);
			_fseeki64(m_fileInputPointer, 0, SEEK_END);
			endpos = _ftelli64(m_fileInputPointer);
			_fseeki64(m_fileInputPointer, lastpos, SEEK_SET);
#elif defined(TARGET_OS_MACOSX) || defined(TARGET_OS_NETBSD) || defined(TARGET_OS_FREEBSD) || defined(TARGET_OS_OPENBSD) || defined(TARGET_COMPILER_MINGW)
			fgetpos(m_fileInputPointer, &lastpos);
			fseek(m_fileInputPointer, 0, SEEK_END);
			fgetpos(m_fileInputPointer, &endpos);
			fsetpos(m_fileInputPointer, &lastpos);
#else
			fgetpos64(m_fileInputPointer, &lastpos);
			fseeko64(m_fileInputPointer, 0, SEEK_END);
			fgetpos64(m_fileInputPointer, &endpos);
			fsetpos64(m_fileInputPointer, &lastpos);
#endif
#else
			fpos_t lastpos, endpos;
			lastpos = ftell(m_fileInputPointer);
			fseek(m_fileInputPointer, 0, SEEK_END);
			endpos = ftell(m_fileInputPointer);
			fseek(m_fileInputPointer, lastpos, SEEK_SET);
#endif

#if defined(TARGET_OS_WINDOWS) || defined(TARGET_OS_MACOSX) || defined(TARGET_OS_FREEBSD) || defined(TARGET_OS_NETBSD) || defined(TARGET_OS_OPENBSD) || defined(TARGET_COMPILER_CYGWIN) || defined(TARGET_OS_NDSFIRMWARE)
			return endpos;
#elif defined(TARGET_OS_LINUX)
			return endpos.__pos;
#endif
		}

		int CoreIOReader::ReadBlock(void *_buffer, size_t _count)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;
			if (!_count)
				return CC_ERR_INVALID_BUFFER;

			int err = fread(_buffer, _count, 1, m_fileInputPointer);
			if (err < 0)
				return CC_ERR_READ;

			return err;
		}

		int CoreIOReader::ReadU8(uint8_t *_buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;

			int err = fread(_buffer, sizeof(uint8_t), 1, m_fileInputPointer);
			if (err < 0)
				return CC_ERR_READ;

			return err;
		}

		int CoreIOReader::ReadU16(uint16_t *_buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;

			int retval;
			retval = fread(_buffer, sizeof(uint16_t), 1, m_fileInputPointer);

			if (retval != sizeof(uint16_t))
				return CC_ERR_READ;

			switch (m_endianness) {
			case Endian::Little:
				*_buffer = CC_SwapLE16(*_buffer);
				break;
			case Endian::Big:
				*_buffer = CC_SwapBE16(*_buffer);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return retval;
		}

		int CoreIOReader::ReadU32(uint32_t *_buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;

			int retval;
			retval = fread(_buffer, sizeof(uint32_t), 1, m_fileInputPointer);

			if (retval != sizeof(uint32_t))
				return CC_ERR_READ;

			switch (m_endianness) {
			case Endian::Little:
				*_buffer = CC_SwapLE32(*_buffer);
				break;
			case Endian::Big:
				*_buffer = CC_SwapBE32(*_buffer);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return retval;
		}

		int CoreIOReader::ReadU64(uint64_t *_buffer)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;

			int retval;
			retval = fread(_buffer, sizeof(uint64_t), 1, m_fileInputPointer);

			if (retval != sizeof(uint64_t))
				return CC_ERR_READ;

			switch (m_endianness) {
			case Endian::Little:
				*_buffer = CC_SwapLE64(*_buffer);
				break;
			case Endian::Big:
				*_buffer = CC_SwapBE64(*_buffer);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return retval;
		}

		int CoreIOReader::ReadLine(char *_buffer, size_t _bufferLength)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;
			if (!_buffer)
				return CC_ERR_INVALID_BUFFER;
			if (!_bufferLength)
				return CC_ERR_INVALID_BUFFER;

			char c = (char)fgetc(m_fileInputPointer);

			if (c == (char)EOF)
				return 0;

			std::string buffer;

			while (c != (char)EOF && c != '\n') {
				buffer += c;
				c = (char)fgetc(m_fileInputPointer);
			}

			int len = (int)buffer.length();

			if (len && buffer[len - 1] == '\r')
				buffer.resize(len - 1);

			return cc_strlcpy(_buffer, buffer.c_str(), _bufferLength);
		}

		/* TODO: This function uses fgetc() which incurs unnecessary function call overhead. Find a suitable replacement. */
		int CoreIOReader::ReadLine(std::string &_string)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			char c = (char)fgetc(m_fileInputPointer);

			if (c == (char)EOF)
				return 0;

			std::string buffer;

			while (c != (char)EOF && c != '\n') {
				buffer += c;
				c = (char)fgetc(m_fileInputPointer);
			}

			int len = (int)buffer.length();

			if (len && buffer[len - 1] == '\r')
				buffer.resize(len - 1);

			_string = buffer;

			return _string.length() * sizeof(char);
		}

		int CoreIOReader::Seek(int64_t _position, int _origin)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

#ifdef HAS_FPOS64
#if defined(TARGET_COMPILER_VC) || defined(TARGET_COMPILER_BORLAND) || (defined(TARGET_COMPILER_ICC) && defined(TARGET_OS_WINDOWS))
			int res = _fseeki64(m_fileInputPointer, _position, _origin);
#elif defined(TARGET_OS_MACOSX) || defined(TARGET_OS_NETBSD) || defined(TARGET_OS_FREEBSD) || defined(TARGET_OS_OPENBSD)
			int res = fseek(m_fileInputPointer, _position, _origin);
#else
			int res = fseeko64(m_fileInputPointer, _position, _origin);
#endif
#else
			int res = fseek(m_fileInputPointer, _position, _origin);
#endif
			return res;
		}

		int CoreIOReader::Seek(int64_t _position)
		{
			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			int res = Seek(_position, SEEK_SET);
			if (res < 0)
				return CC_ERR_READ;
			return 0;
		}

		CrissCross::Errors CoreIOReader::SetLineEndings(LineEnding _ending)
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
				m_lineEnding = "\r";
				break;
			case LineEnding::LF:
				m_lineEnding = "\n";
				break;
			case LineEnding::CRLF:
				m_lineEnding = "\r\n";
				break;
			default:
				return CC_ERR_BADPARAMETER;
			}
			return CC_ERR_NONE;
		}

		void CoreIOReader::SetEndian(Endian _inputEndianness)
		{
			m_endianness = _inputEndianness;
		}
	}
}
