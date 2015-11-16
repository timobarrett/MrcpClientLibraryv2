///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSessionMgr.cpp
//
//	Description:	CMrcpSessionMgr class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpSessionMgr::~CMrcpSessionMgr();
//		int OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg);
//		int AddSessionResources(CMrcpSession* a_session,MrcpResourceType a_resourceToAdd);
//		int CloseSession(MrcpSessionHandle a_sessionHandle);
//		void Shutdown();
//		void HandleEvent(CMrcpEvent* a_event);
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////
#include "CMrcpSessionMgr.h"
#include "CMrcpSession.h"
#include "ClientInterfaceDefs.h"
#include "CMrcpHandleMgr.h"
#include "MrcpClientEvents.h"
#include "CMrcpEvent.h"
#include "MrcpUtils.h"

namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          Register as observer for session objects stopping
///////////////////////////////////////////////////////////////////////////
CMrcpSessionMgr::CMrcpSessionMgr()
	: m_handleMgr(CMrcpHandleMgr::Instance())
{
	m_handleMgr->RegisterForNotification(this, SESSIONS_STOPPED);
	m_state = SESSION_MANGR_STARTING;
	ClassName("CMrcpSessionMgr");
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSessionMgr::~CMrcpSessionMgr()
{
	m_handleMgr->Unregister(this, SESSIONS_STOPPED);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StopAllSessions
// Input - None
//         
// Output - None
//          clean up - when in stopped state, notify observers
///////////////////////////////////////////////////////////////////////////
void CMrcpSessionMgr::Shutdown()
{
	Name("Shutdown");

    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Entering Shutdown");		
	m_handleMgr->Cleanup();
	while (m_state != SESSION_MANGR_STOPPED)
	{
		MrcpUtils::MsSleep(500);
	}
    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Exiting Shutdown");				
	NotifyObservers(SESSION_MGR_STOPPED);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OpenSession
// Input - resource config structure
//         
// Output - None
//      instantiate a CMrcpSession object if same server address
//      else 2 separate session objects
//      instantiate audiostream object
//		instantiate rtp stream object 
//      start CMrcpSession(audio stream ptr, rtp stream ptr)
// 
///////////////////////////////////////////////////////////////////////////
int CMrcpSessionMgr::OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg)
{
	Name("OpenSession");
	bool l_ttsServer = true;
	bool l_asrServer = true;
	CMrcpSession* l_sessionPtr;


	CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering OpenSession");
        char outVal[128];
        //sprintf(outVal,"client_address = %s", a_resourceCfg->clientAddress);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, a_resourceCfg->clientAddress); 

	if (!strcmp(a_resourceCfg->asrServerConfig->serverAddress, "\n"))
		l_asrServer = false;
	if (!strcmp(a_resourceCfg->ttsServerConfig->serverAddress, "\n"))
		l_ttsServer = false;

	if ((l_asrServer && l_ttsServer)&& (!strcmp(a_resourceCfg->asrServerConfig->serverAddress, a_resourceCfg->ttsServerConfig->serverAddress )))
	{
		{
			l_sessionPtr = new CMrcpSession(a_resourceCfg, l_asrServer, l_ttsServer);
                        l_sessionPtr->MultiResourceServer(true);
			l_sessionPtr->StartSession();
	                l_sessionPtr->RegisterForNotification(this, SESSION_COMPLETE);
                        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "BOTH SERVERS THE SAME"); 
		}
	}
//	else
//        if((l_asrServer && l_ttsServer)|| l_asrServer)
        if (l_asrServer && strcmp(a_resourceCfg->asrServerConfig->serverAddress, a_resourceCfg->ttsServerConfig->serverAddress ))
	{
               CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, "instantiating ASRMrcp Session");
//	       l_sessionPtr = new CMrcpSession(a_resourceCfg, l_asrServer,l_ttsServer);
	       l_sessionPtr = new CMrcpSession(a_resourceCfg, l_asrServer,false);
               CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, "starting session");
	       l_sessionPtr->StartSession();
	       l_sessionPtr->RegisterForNotification(this, SESSION_COMPLETE);
	}
//         else
//         if ((l_asrServer && l_ttsServer) || l_ttsServer)
        if (l_ttsServer && strcmp(a_resourceCfg->asrServerConfig->serverAddress, a_resourceCfg->ttsServerConfig->serverAddress ))
	{				
              CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, "instantiating TTSMrcp Session");
//	      l_sessionPtr = new CMrcpSession(a_resourceCfg, l_asrServer, l_ttsServer);
	      l_sessionPtr = new CMrcpSession(a_resourceCfg, false, l_ttsServer);
	      l_sessionPtr->StartSession();
	      l_sessionPtr->RegisterForNotification(this, SESSION_COMPLETE);
	}
  
//	l_sessionPtr->RegisterForNotification(this, SESSION_COMPLETE);
	m_state = SESSION_MANGR_STARTED;
	return MrcpSuccess;
  }

////////////////////////////////////////////////////////////////////////// 
//
// Description - AddSessionResources
// Input - session pointer
//         resource type
//
// Output - status of call to add resource to session
//
// validate that at least and only one session pointer is sent
// use that session ptr to add configured resource to the session
///////////////////////////////////////////////////////////////////////////
int CMrcpSessionMgr::AddSessionResources(CMrcpSession* a_session,MrcpResourceType a_resourceToAdd)
{
	Name("AddSessionResources");
    CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Entering AddSessionResources");

	return a_session->AddResourceToSession(a_resourceToAdd);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - CloseSession
// Input - Session handle
//         
// Output - success or failure
//      get object ptr using session handle
//      stop CMrcpSession
//      unregister session ptr with handle manager
//		delete object - will need observer notification
///////////////////////////////////////////////////////////////////////////
int CMrcpSessionMgr::CloseSession(MrcpSessionHandle a_sessionHandle)
{
	Name("CloseSession");
	CMrcpSession* l_mrcpSession = NULL;
        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Entering CloseSession");
	int l_status;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{			
	   std::string l_errorInfo = "Handle not found " + MrcpUtils::itos(a_sessionHandle);
    	   CLogger::Instance()->Log( LOG_LEVEL_ERROR, *this,l_errorInfo);
	   return -1;
	}

	l_status = l_mrcpSession->StopSession();

	if (MrcpSuccess == l_status)
        {
           CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, "Session stopped");
//           delete l_mrcpSession;
        }

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - HandleEvent
// Input - pointer to event object
//         
// Output - None
//      notify observers that the signaling session has been established.
///////////////////////////////////////////////////////////////////////////
void CMrcpSessionMgr::HandleEvent(CMrcpEvent* a_event)
{
	Name("HandleEvent");
        CMrcpSession* l_eventSessionPtr;

	switch (a_event->EventID())
	{
	case SESSIONS_STOPPED:
     	        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Sessions Stopped received");
		m_state = SESSION_MANGR_STOPPED;
		break;
        case SESSION_COMPLETE:
//               CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Session Complete received");
               l_eventSessionPtr = (CMrcpSession*)a_event->EventData();
               CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Session Complete received",l_eventSessionPtr);
               l_eventSessionPtr->Unregister(this, SESSION_COMPLETE);
               delete l_eventSessionPtr;
               break;
	}
}
}//end namespace

