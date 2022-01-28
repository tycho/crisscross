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

#ifndef __included_internal_mem_h
#define __included_internal_mem_h

#include <cstring>

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
			if (!_data) return nullptr;
			char *dup = (char *)::malloc(strlen(_data) + 1);
			if (!dup) return nullptr;
			return strcpy(dup, _data);
		}

		template <>
		__inline const char *Duplicate(const char * const &_data)
		{
			if (!_data) return nullptr;
			char *dup = (char *)::malloc(strlen(_data) + 1);
			if (!dup) return nullptr;
			return strcpy(dup, _data);
		}

		template <class T>	__inline T NullKey(){ return T(); }
		template <>		__inline int8_t NullKey(){ return 0; }
		template <>		__inline uint8_t NullKey(){ return 0; }
		template <>		__inline int16_t NullKey(){ return 0; }
		template <>		__inline uint16_t NullKey(){ return 0; }
		template <>		__inline int32_t NullKey(){ return 0; }
		template <>		__inline uint32_t NullKey(){ return 0; }
		template <>		__inline int64_t NullKey(){ return 0; }
		template <>		__inline uint64_t NullKey(){ return 0; }
		template <>		__inline char * NullKey(){ return nullptr; }
		template <>		__inline const char * NullKey(){ return nullptr; }
		template <>		__inline std::string NullKey(){ return std::string(""); }

		template <class T>
		__inline void Dealloc(cc_unused_param T &_data)
		{
		}

		template <>
		__inline void Dealloc(char * &_data)
		{
			if (_data) {
				::free(_data);
				_data = nullptr;
			}
		}

		template <>
		__inline void Dealloc(const char * &_data)
		{
			if (_data) {
				::free((char *)_data);
				_data = nullptr;
			}
		}

		/*! @endcond */
	}
}

#endif
