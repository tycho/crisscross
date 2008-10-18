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

#ifndef __included_cc_dstack_h
#define __included_cc_dstack_h

#include <crisscross/mutex.h>

namespace CrissCross
{
	namespace Data
	{
		/*! \brief A dynamic stack implementation. */
		template <class dataType>
		class DStack
		{
			private:
				/*! \brief The step size for which to increase the stack size by. */
				size_t    m_stepSize;

				/*! \brief The actual stack itself. */
				dataType *m_bottom;

				/*! \brief The pointer to the current position in the stack. */
				dataType *m_top;

				/*! \brief The size of the stack. */
				size_t    m_size;

				/*! \brief The original size of the stack (used on an empty() call to reset things to defaults). */
				size_t    m_origSize;

				/*! \brief Sets the stack size to the given size. */
				/*!
				 * \param _size The size to resize to.
				 */
				void setSize(size_t _size);

			public:
				/*! \brief The constructor. */
				/*!
				 * \param _size The step size to use.
				 */
				DStack(size_t _size = -1);

				/*! \brief The destructor. */
				~DStack();

				/*! \brief Increases the array size by the number in m_stepSize */
				inline void grow();

				/*! \brief Pushes a value onto the stack. */
				/*!
				 * \param _val The value to put on the stack.
				 */
				inline void push(dataType _val);

				/*! \brief Pops a value off the stack. */
				/*!
				 * \return The topmost value on the stack.
				 */
				inline dataType pop();

				/*! \brief Returns whatever value is on the top of the stack without removing it from the stack. */
				/*!
				 * \return The topmost value on the stack.
				 */
				dataType const &peek();

				/*! \brief Indicates the number of items on the stack. */
				/*!
				 * \return Number of items on the stack.
				 */
				size_t count() const;

				/*! \brief Empties the stack. */
				/*!
				 *  If pointers are stored in the stack, they must be freed manually via a series of pop() and delete or free() calls.
				 */
				void empty();
		};
	}
}

#include <crisscross/dstack.cpp>

#endif
