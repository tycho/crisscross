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

#ifndef __included_internal_mem_h
#define __included_internal_mem_h

#include <crisscross/cc_attr.h>
#include <crisscross/debug.h>
#include <crisscross/string_utils.h>

namespace CrissCross
{
	namespace Data
	{
		/*! @cond */

		/* These functions are designed for internal use. */
		/* You generally shouldn't use them unless you know what you're doing. */

		template <class T>
		__inline T Duplicate(T const &_data)
		{
			return _data;
		}

		template <>
		__inline char *Duplicate(char * const &_data)
		{
			if (!_data) return NULL;

			return ::cc_strdup(_data);
		}

		template <>
		__inline const char *Duplicate(const char * const &_data)
		{
			if (!_data) return NULL;

			return ::cc_strdup(_data);
		}

		template <class T>	__inline T NullKey() { return T(); }
		template <>		__inline int8_t NullKey() { return 0; }
		template <>		__inline uint8_t NullKey() { return 0; }
		template <>		__inline int16_t NullKey() { return 0; }
		template <>		__inline uint16_t NullKey() { return 0; }
		template <>		__inline int32_t NullKey() { return 0; }
		template <>		__inline uint32_t NullKey() { return 0; }
		template <>		__inline int64_t NullKey() { return 0; }
		template <>		__inline uint64_t NullKey() { return 0; }
		template <>		__inline char * NullKey() { return NULL; }
		template <>		__inline const char * NullKey() { return NULL; }
		template <>		__inline std::string NullKey() { return std::string(""); }

		template <class T>
		__inline void Dealloc(__unused_param T &_data)
		{
		}

		template <>
		__inline void Dealloc(char * &_data)
		{
			if (_data) {
				::free(_data);
				_data = NULL;
			}
		}

		template <>
		__inline void Dealloc(const char * &_data)
		{
			if (_data) {
				::free((char *)_data);
				_data = NULL;
			}
		}

		/*! @endcond */
	}
}

#endif
