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

#ifndef __included_cc_hash_h
#define __included_cc_hash_h

namespace CrissCross
{
	namespace Data
	{
		/*! \brief Hash function for use in a HashTable. */
		/*!
		 * Can hash any primitive type as well as C-style strings and STL strings.
		 * \param _data The data to hash.
		 * \return Hash of the data provided.
		 */
		template <class T>
		__inline size_t Hash(T const &_data)
		{
			size_t      hash = 0, length = sizeof(_data), i;
			const char *data = (char *)&_data;
			for (i = 0; i < length; i++) {
				hash += data[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}

		/*! \brief C-style string CrissCross::Data::Hash function. */
		template <>
		__inline size_t Hash<char *> (char * const &_data)
		{
			CoreAssert(_data);
			size_t hash = 0, length = strlen(_data), i;
			for (i = 0; i < length; i++) {
				hash += _data[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}

		/*! \brief C-style string CrissCross::Data::Hash function. */
		template <>
		__inline size_t Hash<const char *> (const char * const &_data)
		{
			CoreAssert(_data);
			size_t hash = 0, length = strlen(_data), i;
			for (i = 0; i < length; i++) {
				hash += _data[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}

		/*! \brief An STL string CrissCross::Data::Hash function. */
		template <>
		__inline size_t Hash<std::string> (std::string const &_data)
		{
			return Hash(_data.c_str());
		}
	}
}

#endif
