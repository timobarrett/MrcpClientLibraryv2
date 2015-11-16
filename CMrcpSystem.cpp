///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSystem.cpp
//
//	Description:	CMrcpSystem class implementation
//				This is the interface into the library
//	Declared in this file:
//		none
//
//	Implemented in this file:
//	  CMrcpSystem::~CMrcpSystem();
//	  int CMrcpSystem::Start();
//	  int CMrcpSystem::Stop();
//	  int CMrcpSystem::OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg);
//	  int CMrcpSystem::AddSessionResources(MrcpSessionHandle a_sessionHandle,MrcpResourceType a_resourceToAdd);
//	  int CMrcpSystem::CloseSession(MrcpSessionHandle a_sessionHandle);
//	  int CMrcpSystem::DefineGrammar(MrcpSessionHandle a_sessionHandle,DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams);
//	  int CMrcpSystem::Recognize(MrcpSessionHandle a_sessionHandle,RECOGNIZE_STRUCT* a_recognizeParams);
//	  int CMrcpSystem::Interpret(MrcpSessionHandle a_sessionHandle,INTERPRET_STRUCT* a_interpretParams);
//	  int CMrcpSystem::StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack, int& a_referenceNum);
//	  int CMrcpSystem::GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::Speak(MrcpSessionHandle a_sessionHandle,SPEAK_STRUCT* a_speakParams);
//	  int CMrcpSystem::StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBack,int& a_referenceNum);
//	  int CMrcpSystem::GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackFunction a_callBack, GET_PARAMETERS a_parameters, int& a_referenceNum);
//	  int CMrcpSystem::SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackFunction a_callBack, SET_PARAMETERS a_parameters, int& a_referenceNum);
//	  void CMrcpSystem::HandleEvent(CMrcpEvent* a_event);

///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "CMrcpSystem.h"
#include "CMrcpSessionMgr.h"
#include "CMrcpServerMgr.h"
#include "CMrcpTaskProcessor.h"
#include "CMrcpCmdIdMgr.h"
#include "CMrcpHandleMgr.h"
#include "CMrcpSession.h"
#include "CMrcpEvent.h"
#include "CMrcpSrvrWatcher.h"
#include "MrcpClientEvents.h"
#include "MrcpUtils.h"

namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - 
//         
// Output - 
//          
///////////////////////////////////////////////////////////////////////////

CMrcpSystem::CMrcpSystem()
	: m_cmdIdMgr(0)
	, m_handleMgr(0)
	, m_sessionMgr(0)
	, m_state(SYSTEM_STOPPED)
{
	CLogger::Instance()->Start();
	ClassName("CMrcpSystem");
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - 
//         
// Output - 
//          
///////////////////////////////////////////////////////////////////////////

CMrcpSystem::~CMrcpSystem()
{
	delete m_sessionMgr;
	delete m_handleMgr;
	delete m_cmdIdMgr;
        delete m_serverWatcher;
	delete m_mrcpTaskProcessor;
	delete m_mrcpCallbackProcessor;
        delete m_mrcpSipSignalProcessor;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Start
// Input - 
//         
// Output - success or failure
//          checks state. instantiates and starts all singleton managers
//			registers as observer for notification of events
///////////////////////////////////////////////////////////////////////////

int CMrcpSystem::Start(int a_logLevel)
{
	Name("Start");
        if (a_logLevel > 5) a_logLevel = 5;
        CLogger::Instance()->LogLevel((LogLevelEnum)a_logLevel);
	{
	boost::mutex::scoped_lock l_lock (m_controlMutex);
	  if (m_state != SYSTEM_STOPPED)
	  {
	     std::string l_errorInfo = "System already started ";
             CLogger::Instance()->Log( LOG_LEVEL_ERROR, *this,l_errorInfo);
	     return -1;
	  }
	}

	m_state = SYSTEM_STARTING;
	m_sessionMgr = MrcpV2RefLib::CMrcpSessionMgr::Instance();
        m_serverMgr = MrcpV2RefLib::CMrcpServerMgr::Instance();
	m_cmdIdMgr = MrcpV2RefLib::CMrcpCmdIdMgr::Instance();
	m_handleMgr = MrcpV2RefLib::CMrcpHandleMgr::Instance();
        m_serverWatcher = MrcpV2RefLib::CMrcpSrvrWatcher::Instance();
	m_mrcpTaskProcessor = MrcpV2RefLib::CMrcpTaskProcessor::Instance("TASK");
	m_mrcpCallbackProcessor = MrcpV2RefLib::CMrcpTaskProcessor::Instance("CALLBACK");
        m_mrcpSipSignalProcessor = MrcpV2RefLib::CMrcpTaskProcessor::Instance("SIP");
	m_mrcpTaskProcessor->Start();
	m_mrcpCallbackProcessor->Start();
        m_mrcpSipSignalProcessor->Start();
	m_state = SYSTEM_RUNNING;

	m_sessionMgr->RegisterForNotification(this, SESSION_MGR_STOPPED);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Stop
// Input - 
//         
// Output - success
//      signals signletons to shutdown and tracks status.     
///////////////////////////////////////////////////////////////////////////

int CMrcpSystem::Stop()
{
	Name("Stop");

	boost::mutex::scoped_lock l_controlLock(m_controlMutex);
        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this,"System Stopping");
	

	if (m_state == SYSTEM_RUNNING)
	{
		m_state = SYSTEM_STOPPING;
		m_sessionMgr->Shutdown();
                m_serverWatcher->Shutdown();
		m_mrcpTaskProcessor->Stop();
		m_mrcpCallbackProcessor->Stop();
                m_mrcpSipSignalProcessor->Stop();
		CLogger::Instance()->Stop();
	}
	m_state = SYSTEM_STOPPED;
        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this,"System Stopped");
	return MrcpSuccess;

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OpenSession
// Input - resource config parameters
//         
// Output - success
//         called by client application to open a session to a server 
///////////////////////////////////////////////////////////////////////////

int CMrcpSystem::OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg)
{
	Name("OpenSession");

	{
    	CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Opening Session");
		boost::mutex::scoped_lock l_controlLock(m_controlMutex);
		if (m_state != SYSTEM_RUNNING)
		{
			std::string l_errorInfo = "Error System not running: " + MrcpUtils::itos(m_state);
    	   	        CLogger::Instance()->Log( LOG_LEVEL_ERROR, *this,l_errorInfo);
			return -1;
		}
		m_sessionMgr->OpenSession(a_resourceCfg);
	}
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - AddSessionResources
// Input - session handle
//         resource type
// Output - failure or results of addsessionresources method call
//          called to add an asr or tts resource to an existing MRCP session
///////////////////////////////////////////////////////////////////////////

int CMrcpSystem::AddSessionResources(MrcpSessionHandle a_sessionHandle,MrcpResourceType a_resourceToAdd)
{
	   Name("AddSessionResources");
	    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Adding Session Resources");
	   {//scope for lock
		boost::mutex::scoped_lock l_controlLock(m_controlMutex);
		if (m_state != SYSTEM_RUNNING)
		{
		   std::string l_errorInfo = "Error System not running: " + MrcpUtils::itos(m_state);
    		   CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, l_errorInfo);
		   return -1;
		}
	   }
		CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{
    	   CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
	   return -1;
	}

	return m_sessionMgr->AddSessionResources(l_mrcpSession, a_resourceToAdd);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - CloseSession
// Input - session handle
//         
// Output - results of closesession method call
//          called by client application to close a session to an MRCP server
///////////////////////////////////////////////////////////////////////////

int CMrcpSystem::CloseSession(MrcpSessionHandle a_sessionHandle)
{
   Name("CloseSession");

    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Close Session Entered");
	return m_sessionMgr->CloseSession(a_sessionHandle);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - HandleEvent
// Input - pointer to event object
//         
// Output - None
//      process observer notifications received.  all events associated with shutdown 
///////////////////////////////////////////////////////////////////////////
	void CMrcpSystem::HandleEvent(CMrcpEvent* a_event)
	{
	   Name("HandleEvent");

		switch (a_event->EventID())
		{
		case SESSION_MGR_STOPPED:
    		CLogger::Instance()->Log( LOG_LEVEL_INFO,*this,"Session Mgr Stopped");
			m_handleMgr->Unregister(this, SESSION_MGR_STOPPED);
		break;

		}
	}
////////////////////////////////////////////////////////////////////////// 
//
// Description - DefineGrammar
// Input - session handle
//         define grammar parameter structure
// Output - failure or success
//          library interface for define grammar command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::DefineGrammar(MrcpSessionHandle a_sessionHandle, DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams)
{
	Name("DefineGrammar");
        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Define Grammar Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->DefineGrammar(a_defineGrammarParams);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - session handle
//         recognize parameter structure
// Output - failure or success
//          library interface for recognize command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::Recognize(MrcpSessionHandle a_sessionHandle,RECOGNIZE_STRUCT* a_recognizeParams)
{
      Name("Recognize");

        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Recognize Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->Recognize(a_recognizeParams);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpret
// Input - session handle
//         interpret parameter structure
// Output - failure or success
//          library interface for interpret command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::Interpret(MrcpSessionHandle a_sessionHandle,INTERPRET_STRUCT* a_interpretParams)
{
     Name("Interpret");

     CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Interpret Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->Interpret(a_interpretParams);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartInputTimers
// Input - session handle
//         callback routine
//         reference number
// Output - failure or success
//          library interface for start input timers command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued. 
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
   Name("StartInputTimers");

    CLogger::Instance()->Log( LOG_LEVEL_INFO, *this, "Start Input Timers Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->StartInputTimers(a_callBackInfo,a_referenceNum);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetResult
// Input - session handle
//         callback function pointer
//		   reference number
// Output - failure or success
//          library interface for GetResult command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.  
//          
///////////////////////////////////////////////////////////////////////////
//int CMrcpSystem::GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo, int& a_referenceNum)
int CMrcpSystem::GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
   Name("GetResult");

	CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Get Result Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->GetResult(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopRec
// Input - session handle
//         callback function pointer
//         reference number
// Output - failure or success
//          library interface for stop for recognition command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
	Name("StopRecog");
        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Stop Recog Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->StopRecog(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speak
// Input - session handle
//         speak parameter structure
// Output - failure or success
//          library interface for speak command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::Speak(MrcpSessionHandle a_sessionHandle,SPEAK_STRUCT* a_speakParams)
{
	   Name("Speak");

        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Speak Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->Speak(a_speakParams);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSpeak
// Input - session handle
//         callback function
//         reference number
// Output - failure or success
//          library interface for stopping speak command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("StopSpeak");
	CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Stop speak Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->StopSpeak(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Pause
// Input - session handle
//         callback function
//         reference number
// Output - failure or success
//          library interface for pause command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
//int CMrcpSystem::Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo, int& a_referenceNum)
int CMrcpSystem::Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("Pause");
        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Pause Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->Pause(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Resume
// Input - session handle
//         callback function
//         reference Number
// Output - failure or success
//          library interface for resume command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.  
//          
///////////////////////////////////////////////////////////////////////////
//int CMrcpSystem::Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo, int& a_referenceNum)
int CMrcpSystem::Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
    Name("Resume");
    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this,"Resume Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->Resume(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - BargeIn
// Input - session handle
//         callback function
//         reference number
// Output - failure or success
//          library interface for barge in command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("BargeIn");

        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Barge In Command");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->BargeIn(a_callBackInfo, a_referenceNum);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetParams
// Input - session handle
//         callback function
//		   get parameters structure		
//	       reference number
// Output - failure or success
//          library interface for get parameters command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, GET_PARAMETERS a_parameters, int& a_referenceNum)
{
    Name("GetParams");
    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Get Parameters");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->GetParams(a_callBackInfo,a_parameters, a_referenceNum);
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SetParams
// Input - session handle
//         callback function
//         set parameters structure
//		   reference number
// Output - failure or success
//          library interface for set parameters command.  Session handle comes
//			from opensession call.   Reference ID returned in struct to tie
//			event received to command issued.
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSystem::SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, SET_PARAMETERS a_parameters, int& a_referenceNum)
{
	Name("SetParams");
        CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, "Set Parameters");
	CMrcpSession* l_mrcpSession = NULL;

	l_mrcpSession = m_handleMgr->RetrievePointer(a_sessionHandle);
	if (NULL == l_mrcpSession)
	{		    		
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this, "Session is NULL");
		return -1;
	}
	l_mrcpSession->SetParams(a_callBackInfo,a_parameters, a_referenceNum);

	return MrcpSuccess;
}

}//end namespace


