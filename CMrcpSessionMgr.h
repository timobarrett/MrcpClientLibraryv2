///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSessionMgr.h
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
#ifndef CMrcpSessionMgr_H
#define CMrcpSessionMgr_H

#include "Singleton.h"
#include "CLogger.h"
#include "ClientInterfaceDefs.h"
#include "AMrcpEventHandler.h"
#include "AMrcpObserverSubject.h"

namespace MrcpV2RefLib
{
	typedef enum 
	{
	  SESSION_MANGR_STARTING,   
	  SESSION_MANGR_STARTED,
	  SESSION_MANGR_STOPPING,
	  SESSION_MANGR_STOPPED
	} SessionMgrStateEnum;

	class CMrcpSession;
	class CMrcpHandleMgr;
	class CMrcpSessionMgr : public Singleton<CMrcpSessionMgr>
						  , public AMrcpEventHandler
						  , public AMrcpObserverSubject
	{
		friend class Singleton<CMrcpSessionMgr>;

	public:
		virtual ~CMrcpSessionMgr();
		int OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg);
		int AddSessionResources(CMrcpSession* a_session,MrcpResourceType a_resourceToAdd);
		int CloseSession(MrcpSessionHandle a_sessionHandle);
		void Shutdown();

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
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
		void HandleEvent(CMrcpEvent* a_event);
		CMrcpSessionMgr();
		MrcpV2RefLib::CMrcpHandleMgr* m_handleMgr;
		SessionMgrStateEnum m_state;
	        std::string m_className;
	        std::string m_name;
                std::string m_signalPtr;
                boost::mutex m_loggerMutex;
	};


}//end namespace

#endif 

