
///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpEvent.h
//
//	Description:	class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CMrcpEvent_H
#define CMrcpEvent_H

#include "MrcpTasks.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{
	class AMrcpEventHandler;
	class MrcpTasks;

	class CMrcpEvent : public MrcpTasks
	{
	public:
		CMrcpEvent();
		virtual ~CMrcpEvent();

		void Execute();
		int HandleEvent();
		
		AMrcpEventHandler* EventHandler() const {return m_eventObserver;};
		void EventHandler(AMrcpEventHandler* a_eventObserver){m_eventObserver = a_eventObserver;};

		int EventID() const { return m_eventID;};
		void EventID(int a_eventID){m_eventID = a_eventID;};

                void* EventData() {return m_eventData;};
                void EventData(void* a_eventData){m_eventData = a_eventData;};

	    const char* StateDesc() { return "N/A";}		
		const std::string& ClassName(){return m_className;};
		void ClassName(const std::string& a_className) {m_className = a_className;};
	        const std::string& Name(){{ // scope for lock
                   boost::mutex::scoped_lock l_controlLock (m_loggerMutex);
                   return m_name;
                   }};
                void Name( const std::string& a_name){ { // scope for lock
                   boost::mutex::scoped_lock l_controlLock( m_loggerMutex);
                   m_name = a_name;
                   }};

//        	const std::string& Name(){return m_name;};
//		void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
		AMrcpEventHandler* m_eventObserver;
		int m_eventID;
		std::string m_className;
   	        std::string m_name;
                std::string m_signalPtr;
                boost::mutex m_loggerMutex;
                void* m_eventData;
	};

}//end namespace
#endif

