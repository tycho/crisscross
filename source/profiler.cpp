/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/profiler.h>

#ifdef PROFILER_ENABLED

namespace CrissCross
{
	namespace System
	{
		// TODO: Make this thread detection not dependent on SDL.
#ifdef SINGLE_THREADED_PROFILER
		static Uint32 s_profileThread;
#  define MAIN_THREAD_ONLY { if (SDL_ThreadID() != s_profileThread) return; }
#else
#  define MAIN_THREAD_ONLY {}
#endif

		ProfiledElement::ProfiledElement(char const *_name, ProfiledElement *_parent)
		:	m_currentTotalTime(0.0),
			m_currentNumCalls(0),
			m_lastTotalTime(0.0),
			m_lastNumCalls(0),
			m_longest(DBL_MIN),
			m_shortest(DBL_MAX),
			m_callStartTime(0.0),
			m_historyTotalTime(0.0),
			m_historyNumSeconds(0.0),
			m_historyNumCalls(0),
			m_parent(_parent),
			m_isExpanded(false)
		{
			m_name = strdup(_name);
		}

		ProfiledElement::~ProfiledElement()
		{
			free(m_name);
			m_children.EmptyAndDelete();
		}

		void ProfiledElement::Start()
		{
			m_callStartTime = GetHighResTime();
		}

		void ProfiledElement::End()
		{
			double const timeNow = GetHighResTime();

			m_currentNumCalls++;
			double const duration = timeNow - m_callStartTime;
			m_currentTotalTime += duration;

			if (duration > m_longest) {
				m_longest = duration;
			}

			if (duration < m_shortest) {
				m_shortest = duration;
			}
		}

		void ProfiledElement::Advance()
		{
			m_lastTotalTime = m_currentTotalTime;
			m_lastNumCalls = m_currentNumCalls;
			m_currentTotalTime = 0.0;
			m_currentNumCalls = 0;
			m_historyTotalTime += m_lastTotalTime;
			m_historyNumSeconds += 1.0;
			m_historyNumCalls += m_lastNumCalls;

			for (int i = 0; i < m_children.Size(); ++i) {
				if (m_children.ValidIndex(i)) {
					m_children[i]->Advance();
				}
			}
		}

		void ProfiledElement::ResetHistory()
		{
			m_historyTotalTime = 0.0;
			m_historyNumSeconds = 0.0;
			m_historyNumCalls = 0;
			m_longest = DBL_MIN;
			m_shortest = DBL_MAX;

			for (unsigned int i = 0; i < m_children.Size(); ++i) {
				if (m_children.ValidIndex(i)) {
					m_children[i]->ResetHistory();
				}
			}
		}

		double ProfiledElement::GetMaxChildTime()
		{
			double rv = 0.0;

			short  first = m_children.StartOrderedWalk();
			if (first == -1) {
				return 0.0;
			}

			short  i = first;
			while (i != -1)
			{
				float            val = m_children[i]->m_historyTotalTime;

				if (val > rv) {
					rv = val;
				}

				i = m_children.GetNextOrderedIndex();
			}

			return rv / m_children[first]->m_historyNumSeconds;
		}

		Profiler::Profiler()
		:
			m_currentElement(NULL),
			m_insideRenderSection(false)
		{
			m_rootElement = new ProfiledElement("Root", NULL);
			m_rootElement->m_isExpanded = true;
			m_currentElement = m_rootElement;
			m_endOfSecond = GetHighResTime() + 1.0f;
		#ifdef SINGLE_THREADED_PROFILER
			s_profileThread = SDL_ThreadID();
		#endif
		}

		Profiler::~Profiler()
		{
			delete m_rootElement;
		}

		void Profiler::Advance()
		{
			double timeNow = GetHighResTime();
			if (timeNow > m_endOfSecond) {
				m_lengthOfLastSecond = timeNow - (m_endOfSecond - 1.0);
				m_endOfSecond = timeNow + 1.0;

				m_rootElement->Advance();
			}
		}

		void Profiler::RenderStarted()
		{
			m_insideRenderSection = true;
		}

		void Profiler::RenderEnded()
		{
			m_insideRenderSection = false;
		}

		void Profiler::ResetHistory()
		{
			m_rootElement->ResetHistory();
		}

		void Profiler::StartProfile(char const *_name)
		{
			MAIN_THREAD_ONLY;

			ProfiledElement *pe = m_currentElement->m_children.GetData(_name);
			if (!pe) {
				pe = new ProfiledElement(_name, m_currentElement);
				m_currentElement->m_children.PutData(_name, pe);
			}

			CoreAssert(m_rootElement->m_isExpanded);

			bool             wasExpanded = m_currentElement->m_isExpanded;

			if (m_currentElement->m_isExpanded) {
				pe->Start();
			}

			m_currentElement = pe;

			m_currentElement->m_wasExpanded = wasExpanded;
		}

		void Profiler::EndProfile(char const *_name)
		{
			MAIN_THREAD_ONLY;

			CoreAssert(m_currentElement->m_wasExpanded == m_currentElement->m_parent->m_isExpanded);

			if (m_currentElement->m_parent->m_isExpanded) {
				CoreAssert(m_currentElement != m_rootElement);
				CoreAssert(stricmp(_name, m_currentElement->m_name) == 0);

				m_currentElement->End();
			}

			CoreAssert(strcmp(m_currentElement->m_name, m_currentElement->m_parent->m_name) != 0);
			m_currentElement = m_currentElement->m_parent;
		}
	}
}
#endif
