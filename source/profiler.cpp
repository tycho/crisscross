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


#ifdef ENABLE_OPENGL
#ifdef TARGET_OS_MACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#include <crisscross/compare.h>
#include <crisscross/profiler.h>
#include <crisscross/system.h>

#include <float.h>

namespace CrissCross
{
	namespace System
	{
		ProfiledElement::ProfiledElement(char const *_name, ProfiledElement *_parent, Profiler *_profiler)
		:	m_profiler(_profiler),
			m_currentTotalTime(0.0),
			m_currentNumCalls(0),
			m_lastTotalTime(0.0),
			m_lastNumCalls(0),
			m_historyTotalTime(0.0),
			m_historyNumSeconds(0.0),
			m_historyNumCalls(0),
			m_shortest(DBL_MAX),
			m_longest(DBL_MIN),
			m_callStartTime(0.0),
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
			m_callStartTime = CrissCross::System::GetHighResTime();
		}

		void ProfiledElement::End()
		{
			double const timeNow = CrissCross::System::GetHighResTime();

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

			float thisMax = m_lastTotalTime;
			if( thisMax > m_profiler->m_maxFound ) m_profiler->m_maxFound = thisMax;

			for (size_t i = 0; i < m_children.Size(); ++i) {
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
			m_currentElement(NULL)
		{
			m_rootElement = new ProfiledElement("Root", NULL, this);
			m_rootElement->m_isExpanded = true;
			m_currentElement = m_rootElement;
			m_doGlFinish = m_inRenderSection = false;
			m_lastFrameStart = -1.0;
			m_endOfSecond = CrissCross::System::GetHighResTime() + 1.0f;
			SetMasterThread();
		}

		Profiler::~Profiler()
		{
			delete m_rootElement;
		}

		void Profiler::Advance()
		{
			double timeNow = CrissCross::System::GetHighResTime();
			if (timeNow > m_endOfSecond)
			{
				m_lengthOfLastSecond = timeNow - (m_endOfSecond - 1.0);
				m_endOfSecond = timeNow + 1.0;

				m_maxFound = 0.0f;
				m_rootElement->Advance();
			}

			if( m_lastFrameStart >= 0 )
			{
				double lastFrameTime = timeNow - m_lastFrameStart;
				m_frameTimes.insert_front( int( lastFrameTime * 1000 ) );
			}

			while( m_frameTimes.valid(200) )
				m_frameTimes.remove(200);

			m_lastFrameStart = timeNow;
		}

		void Profiler::ResetHistory()
		{
			m_rootElement->ResetHistory();
		}

		void Profiler::SetMasterThread()
		{
	#ifdef TARGET_OS_WINDOWS
			m_masterThread = GetCurrentThreadId();
	#else
			m_masterThread = pthread_self();
	#endif
		}

		bool Profiler::IsMasterThread()
		{
	#ifdef TARGET_OS_WINDOWS
			return m_masterThread == GetCurrentThreadId();
	#else
			return pthread_equal(m_masterThread, pthread_self());
	#endif
		}

		void Profiler::StartProfile(char const *_name)
		{
			if (!IsMasterThread()) return;

			ProfiledElement *pe = m_currentElement->m_children.GetData(_name);
			if (!pe) {
				pe = new ProfiledElement(_name, m_currentElement, this);
				m_currentElement->m_children.PutData(_name, pe);
			}

			CoreAssert(m_rootElement->m_isExpanded);

			bool wasExpanded = m_currentElement->m_isExpanded;

			if (m_currentElement->m_isExpanded) {
				if (m_doGlFinish && m_inRenderSection) {
#ifdef ENABLE_OPENGL
					glFinish();
#endif
				}
				pe->Start();
			}

			m_currentElement = pe;

			m_currentElement->m_wasExpanded = wasExpanded;
		}

		void Profiler::EndProfile(char const *_name)
		{
			if (!IsMasterThread()) return;

			//CoreAssert(m_currentElement->m_wasExpanded == m_currentElement->m_parent->m_isExpanded);

			if (m_currentElement &&
				m_currentElement->m_parent )
			{
				if (m_currentElement->m_parent->m_isExpanded) {
					if (m_doGlFinish && m_inRenderSection) {
#ifdef ENABLE_OPENGL
						glFinish();
#endif
					}

					CoreAssert(m_currentElement != m_rootElement);
					CoreAssert(CrissCross::Data::Compare(_name, (const char *)m_currentElement->m_name) == 0);

					m_currentElement->End();
				}

				CoreAssert(CrissCross::Data::Compare(m_currentElement->m_name, m_currentElement->m_parent->m_name) != 0);
				m_currentElement = m_currentElement->m_parent;
			}
		}
	}
}
