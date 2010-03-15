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

#ifndef __included_cc_profiler_h
#define __included_cc_profiler_h

#include <crisscross/cc_attr.h>
#include <crisscross/hashtable.h>
#include <crisscross/llist.h>

namespace CrissCross
{
	namespace System
	{
		class Profiler;

		class ProfiledElement
		{
		private:
			Profiler  *m_profiler;

		public:
			/* Values used for accumulating the profile for the current second */
			double     m_currentTotalTime; /* In seconds */
			int        m_currentNumCalls;

			/* Values used for storing the profile for the previous second */
			double     m_lastTotalTime; /* In seconds */
			int        m_lastNumCalls;

			/* Values used for storing history data (accumulation of all m_lastTotalTime */
			/* and m_lastNumCalls values since last reset) */
			double     m_historyTotalTime; /* In seconds */
			double     m_historyNumSeconds;
			int        m_historyNumCalls;

			/* Values used for storing the longest and shortest duration spent inside */
			/* this elements profile. These values are reset when the history is reset */
			double     m_shortest;
			double     m_longest;

			double     m_callStartTime;
			char      *m_name;

			CrissCross::Data::SortingHashTable<ProfiledElement *> m_children;
			ProfiledElement                            *m_parent;

			bool       m_isExpanded; /* Bit of data that a tree view display can use */
			bool       m_wasExpanded;

		public:
			ProfiledElement(char const *_name, ProfiledElement *_parent, Profiler *_profiler);
			~ProfiledElement();

			void Start();
			void End();
			void Advance();
			void ResetHistory();

			double GetMaxChildTime();
		};


		class Profiler
		{
		protected:
			bool m_inRenderSection;

			bool IsMasterThread();

		// TODO: Making these public is a bad practice. Find a more appropriate way to do this.
		public:
			ProfiledElement	*m_currentElement;
			ProfiledElement	*m_rootElement;
			double           m_endOfSecond;
			double           m_lengthOfLastSecond;
			bool             m_doGlFinish;
			#ifdef TARGET_OS_WINDOWS
			unsigned int     m_masterThread;
			#else
			pthread_t        m_masterThread;
			#endif
			double           m_lastFrameStart;
			float            m_maxFound;

		    CrissCross::Data::LList<int> m_frameTimes;

		public:
			Profiler();
			~Profiler();

			void Advance();

			/*! \brief Sets the master thread for this instance of the Profiler. */
			/*!
			 * The Profiler works best if it's only used in one thread.
			 * This is used to set the thread that's allowed to use it.
			 */
			void SetMasterThread();

			void StartProfile(char const *_name);
			void EndProfile(char const *_name);

			__forceinline void RenderStarted() { m_inRenderSection = true; }
			__forceinline void RenderEnded() { m_inRenderSection = false; }

			void ResetHistory();
		};
	}
}

#ifdef PROFILER_ENABLED
#define SET_PROFILE(profiler, itemName, value) profiler->SetProfile(itemName, value)
#define START_PROFILE(profiler, itemName) profiler->StartProfile(itemName)
#define END_PROFILE(profiler, itemName) profiler->EndProfile(itemName)
#else
#define SET_PROFILE(profiler, name, value)
#define START_PROFILE(profiler, itemName)
#define END_PROFILE(profiler, itemName)
#endif

#endif
