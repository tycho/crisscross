/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

/*
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Angus Leeming
 *
 * A pointer with copy-on-write semantics
 *
 * The original version of this class was written by Yonat Sharon
 * and is freely available at http://ootips.org/yonat/
 *
 * I modified it to use boost::shared_ptr internally, rather than use his
 * home-grown equivalent.
 */

#ifndef __included_cc_cow_ptr_h
#define __included_cc_cow_ptr_h

#include <crisscross/platform_detect.h>

#ifdef TARGET_SUPPORTS_TR1

#ifdef TARGET_COMPILER_GCC
#	include <tr1/memory>
#else
#	include <memory>
#endif

#include <crisscross/deprecate.h>
#include <crisscross/internal_mem.h>

namespace CrissCross {
	namespace Data {

		template <typename T>
		class cow_ptr {
		public:
			explicit cow_ptr(T * = 0);
			cow_ptr(cow_ptr const &);
			cow_ptr &operator=(cow_ptr const &);
			bool operator==(cow_ptr<T> const &) const;

			void reset();

			T const & operator*() const;
			T const * operator->() const;
			T const * get() const;

			T & operator*();
			T * operator->();
			T * get();

		private:
			std::tr1::shared_ptr<T> ptr_;
			void copy();
		};

		template <typename T>
		cow_ptr<T>::cow_ptr(T * p)
			: ptr_(p)
		{}

		template <typename T>
		cow_ptr<T>::cow_ptr(cow_ptr const & other)
			: ptr_(other.ptr_)
		{}

		template <typename T>
		cow_ptr<T> & cow_ptr<T>::operator=(cow_ptr const & other)
		{
			if (&other != this)
				ptr_ = other.ptr_;
			return *this;
		}

		template <typename T>
		void cow_ptr<T>::reset()
		{
			ptr_.reset();
		}

		template <typename T>
		T const & cow_ptr<T>::operator*() const
		{
			return *ptr_;
		}

		template <typename T>
		T const * cow_ptr<T>::operator->() const
		{
			return ptr_.get();
		}

		template <typename T>
		T const * cow_ptr<T>::get() const
		{
			return ptr_.get();
		}

		template <typename T>
		bool cow_ptr<T>::operator==(cow_ptr<T> const &rhs) const
		{
			return ptr_ == rhs.ptr_;
		}

		template <typename T>
		_CC_WOULD_COPY T & cow_ptr<T>::operator*()
		{
			copy();
			return *ptr_;
		}

		template <typename T>
		_CC_WOULD_COPY T * cow_ptr<T>::operator->()
		{
			copy();
			return ptr_.get();
		}

		template <typename T>
		_CC_WOULD_COPY T * cow_ptr<T>::get()
		{
			copy();
			return ptr_.get();
		}

		template <typename T>
		_CC_WOULD_COPY void cow_ptr<T>::copy()
		{
			if (!ptr_.unique())
				ptr_ = std::tr1::shared_ptr<T>(Duplicate<T *>(ptr_.get()));
		}
	}
}

#endif

#endif