
///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpHandleMgr.h
//
//	Description:	 class implementation
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
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CMrcpHandleMgr_H
#define CMrcpHandleMgr_H

#include <boost/thread/mutex.hpp>

#include <map>
#include <string>
#include "Singleton.h"
#include "AMrcpObserverSubject.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{

	class CMrcpSession;
	class CMrcpAudioStream;
	class CMrcpHandleMgr  :public Singleton <CMrcpHandleMgr>,
					       public AMrcpObserverSubject
	{
		friend class Singleton<CMrcpHandleMgr>;

	public:
		virtual ~CMrcpHandleMgr();
		int RegisterPointer(CMrcpSession* a_objPointer);

		int UnregisterPointer(int a_handle,std::string a_pointerType);
		CMrcpSession* RetrievePointer(int a_handle);
		int Cleanup();

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

//		const std::string& Name(){return m_name;};
//		void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
		typedef std::map<int, CMrcpSession*> HandleToPointerMap;
		HandleToPointerMap m_hndlePtrMap;
		CMrcpHandleMgr();
		int m_handleID;
		boost::mutex m_criticalSection;
                boost::mutex m_loggerMutex;

		std::string m_className;
  	        std::string m_name;
                std::string m_signalPtr;

	};
} //end namespace


#endif //CMrcpHandleMgr_H

