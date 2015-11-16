///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSession.cpp
//
//	Description:	CMrcpSession class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpSession(RESOURCE_CFG_STRUCT* a_resourceCfg, bool l_asrResource, bool l_ttsResource);
//		virtual ~CMrcpSession();
//		int StartSession();
//		int StopSession();
//		int AddResourceToSession(MrcpResourceType a_resourceToAdd);
//		int DefineGrammar(DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams);
//		int Recognize(RECOGNIZE_STRUCT* a_recognizeParams);
//		int Interpret(INTERPRET_STRUCT* a_interpretParams);
//		int StartInputTimers(MrcpCallBackFunction a_callBack, int& a_referenceNum);
//		int GetResult(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int StopRecog(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int Speak(SPEAK_STRUCT* a_speakParams);
//		int StopSpeak(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int Pause(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int Resume(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int BargeIn(MrcpCallBackFunction a_callBack,int& a_referenceNum);
//		int GetParams(MrcpCallBackFunction a_callBack, GET_PARAMETERS a_parameters, int& a_referenceNumber);
//		int SetParams(MrcpCallBackFunction a_callBack, SET_PARAMETERS a_parameters, int& a_referenceNumber);
//		void HandleEvent(CMrcpEvent* a_event);
//		void PerformSessionCallback();
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "CMrcpSession.h"
#include "CMrcpSipSignal.h"
#include "CMrcpRtspSignal.h"
#include "CMrcpEvent.h"
#include "CMrcpHandleMgr.h"
#include "MrcpClientEvents.h"
#include "ClientInterfaceDefs.h"
#include "CRecognitionCommands.h"
#include "CSpeakCommands.h"
#include "CMrcpCmdIdMgr.h"
#include "CMrcpTaskProcessor.h"
#include "CSpeechRecognizer.h"
#include "CSpeechSynthesizer.h"
#include "CGenericCommands.h"
#include "CMrcpGeneric.h"
#include "CMrcpSrvrWatcher.h"
#include "MrcpUtils.h"
//#include <sys/time.h>
//#include <sys/resource.h>

 
namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - config structure,
//		   bool indicating asr
//		   bool indicating tts
//         
// Output - None
//			Can support asr and tts resource per session	       
///////////////////////////////////////////////////////////////////////////
CMrcpSession::CMrcpSession(RESOURCE_CFG_STRUCT* a_resourceCfg, bool a_asrResource, bool a_ttsResource)
	: m_handleMgr(CMrcpHandleMgr::Instance())
	, m_resourceCfg(a_resourceCfg)
	, m_clientAddress(a_resourceCfg->clientAddress)
	, m_cmdIdMgr(CMrcpCmdIdMgr::Instance())
	, m_sessionCmdID(0)
	, m_asrResource(a_asrResource)
	, m_ttsResource(a_ttsResource)
	, m_sessionHandle(0)
        , m_asrSessionHandle(0)
        , m_ttsSessionHandle(0)
	, m_speechSynthResource(NULL)
	, m_speechRecResource(NULL)
        , m_multiResourceServer(false)
        , m_byeReceived(false)
        , m_signalCommFail(false)
        , m_resourceAdded(false)
        , m_signalObj(0)
        , m_keepAliveInterval(a_resourceCfg->keepAliveInterval)
        , m_keepAliveCnt(a_resourceCfg->keepAliveCnt)
        , m_stopping(false)
        , m_sessionCompleteSent(false)
{
	ClassName("CMrcpSession");
	Name("Constructor");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering session constructor");

	if (m_asrResource)
	{
		m_serverAddress = a_resourceCfg->asrServerConfig->serverAddress;
		m_serverPort = a_resourceCfg->asrServerConfig->serverPort;
	//added IPMS
		m_clientPort = a_resourceCfg->clientPort;
		m_asrCallbackInfo.callbackFunction = a_resourceCfg->asrServerConfig->callBackInfo.callbackFunction;
		m_asrCallbackInfo.callbackHandle = a_resourceCfg->asrServerConfig->callBackInfo.callbackHandle;
                m_codec = a_resourceCfg->asrServerConfig->rtpCodec;
	}
	if (m_ttsResource)
	{
		m_serverAddress = a_resourceCfg->ttsServerConfig->serverAddress;
		m_serverPort = a_resourceCfg->ttsServerConfig->serverPort;
	//added IPMS
		m_clientPort = a_resourceCfg->clientPort;
		m_ttsCallbackInfo.callbackFunction = a_resourceCfg->ttsServerConfig->callBackInfo.callbackFunction;
		m_ttsCallbackInfo.callbackHandle = a_resourceCfg->ttsServerConfig->callBackInfo.callbackHandle;
                m_codec = a_resourceCfg->ttsServerConfig->rtpCodec;
	}

	m_state= SESSION_STARTING;
	m_mrcpAsrVersion = a_resourceCfg->asrServerConfig->mrcpVersion;
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"MRCP Asr version = ",m_mrcpAsrVersion);
	m_mrcpTtsVersion = a_resourceCfg->ttsServerConfig->mrcpVersion;
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"MRCP Tts version =",m_mrcpTtsVersion);
        if(m_mrcpAsrVersion == 0 && m_mrcpTtsVersion !=0)
           m_mrcpAsrVersion = m_mrcpTtsVersion;
        if(m_mrcpTtsVersion == 0 && m_mrcpAsrVersion != 0)
           m_mrcpTtsVersion = m_mrcpAsrVersion;
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"MRCP Asr version = ",m_mrcpAsrVersion);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"MRCP Tts version =",m_mrcpTtsVersion);


}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSession::~CMrcpSession()
{
	Name("Destructor");
        
        int tmp = m_pendingList.size();
        if (tmp)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this," PENDING list cnt = ",tmp);
           { // scope for lock
             boost::mutex::scoped_lock l_controlLock (m_listMutex);
             m_pendingList.clear();
           }
        }
       if (m_speechSynthResource)
       {
         m_speechSynthResource->CleanUp();
         delete m_speechSynthResource;
         CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this," After Speech Synth cleanup");
       }

       if(m_speechRecResource)
       {
         m_speechRecResource->CleanUp();
         delete m_speechRecResource;
         CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this," After Speech Reco cleanup");
       }

        int l_stat = m_handleMgr->UnregisterPointer(m_sessionHandle,"Session");
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Unregistered Handle = ",m_sessionHandle);
        if (0> l_stat)
        {
                std::string l_errorInfo = "Unregistering Handle failed " + MrcpUtils::itos(m_sessionHandle);
                CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this,l_errorInfo);
        }

        CMrcpSrvrWatcher::Instance()->Unregister(this, MRCP_SERVER_UNREACHABLE);
        m_signalObj->Unregister(this, SIGNALING_PROTOCOL_FAILED);
        m_signalObj->Unregister(this, COMMAND_SOCKET_FAILED);

        if (m_genericCommandResource)
          delete m_genericCommandResource;

//        m_mrcpProcessor->Stop();
//        delete m_mrcpProcessor;

        if(m_signalObj)
        {
          m_signalObj->Unregister(this, SIGNALING_PROTOCOL_DISCONNECTED);
          delete m_signalObj;
        }

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartSession
// Input - None
//         
// Output - success
//    start the signaling 
//    register for notification from signaling when started
//    return a status 
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::StartSession()
{
	Name("StartSession");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering start session");

	MrcpV2RefLib::CSignalingStartCommand* l_startCommand = NULL;
	m_state = SESSION_CONNECTING;
	
	m_sessionHandle = m_handleMgr->RegisterPointer(this);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Registered Handle = ",m_sessionHandle);
	if ( m_ttsResource && m_asrResource  )
        {
           m_resourceCfg->asrServerConfig->sessionHandle = m_resourceCfg->ttsServerConfig->sessionHandle = m_sessionHandle;
           m_asrSessionHandle = m_ttsSessionHandle = m_sessionHandle;
        }
	else
	if (m_asrResource)
        {
           m_resourceCfg->asrServerConfig->sessionHandle = m_sessionHandle;
           m_asrSessionHandle = m_sessionHandle;
        }
	else
	if (m_ttsResource)
        {
           m_resourceCfg->ttsServerConfig->sessionHandle = m_sessionHandle;
           m_ttsSessionHandle = m_sessionHandle;
        }

	if (m_mrcpAsrVersion  == 2 || m_mrcpTtsVersion == 2)
		m_signalObj = new MrcpV2RefLib::CMrcpSipSignal(this);
        else if (m_mrcpAsrVersion == 1 || m_mrcpTtsVersion == 1)
                m_signalObj = new MrcpV2RefLib::CMrcpRtspSignal(this);

        SignalPtr(MrcpUtils::itos(int(m_signalObj)));
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SIGNAL PTR1 = ",m_signalPtr);

        m_signalObj->TtsSessionHandle(m_resourceCfg->ttsServerConfig->sessionHandle);
        m_signalObj->AsrSessionHandle(m_resourceCfg->asrServerConfig->sessionHandle);
        m_signalObj->ConnectBackoff(m_resourceCfg->sockConnectBackoff);
	
	m_genericCommandResource = new CMrcpGeneric(m_resourceCfg, m_signalObj);

	if (m_asrResource)
	{
	   m_speechRecResource = new CSpeechRecognizer(m_resourceCfg, m_signalObj); 
	   m_sessionCmdID = m_cmdIdMgr->GetReferenceId();
    	   int l_status = m_cmdIdMgr->RegisterCommandObj(m_sessionCmdID, m_resourceCfg->asrServerConfig->callBackInfo);
	}
	if (m_ttsResource)
	{
	   m_speechSynthResource = new CSpeechSynthesizer(m_resourceCfg, m_signalObj);
	   m_sessionCmdID = m_cmdIdMgr->GetReferenceId();
//TMP
           int foo = (int)m_resourceCfg->ttsServerConfig->callBackInfo.callbackHandle;
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "callback handle = ",foo);
           int l_status = m_cmdIdMgr->RegisterCommandObj(m_sessionCmdID, m_resourceCfg->ttsServerConfig->callBackInfo);
	}
	
	m_signalObj->RegisterForNotification(this, SIGNALING_PROTOCOL_CONNECTED);
	m_signalObj->RegisterForNotification(this, SIGNALING_PROTOCOL_DISCONNECTED);
	m_signalObj->RegisterForNotification(this, SIGNALING_PROTOCOL_FAILED);
        m_signalObj->RegisterForNotification(this, COMMAND_SOCKET_FAILED);
        CMrcpSrvrWatcher::Instance()->RegisterForNotification(this, MRCP_SERVER_UNREACHABLE);

        clock_gettime(CLOCK_REALTIME, &m_sessionStart);

	l_startCommand = new CSignalingStartCommand(m_signalObj);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_startCommand);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "PRIMSTART Queued");

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSession
// Input - None
//         
// Output - Success
//    stop signaling 
//    state  unregister for notification from signaling when started
//    return a status 
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::StopSession()
{
	Name("StopSession");
    
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Entering StopSession");

	time_t   cmdIssued, elapsedTime;
        m_ttsComplete = false;
        m_asrComplete = false;

        if (SignalCommFailed()|| SignalUnsolictBye())
           PerformSessionCallback(1);

        if (m_stopping)
           return MrcpSuccess;
        else
           m_stopping = true;

	m_signalObj->Unregister(this, SIGNALING_PROTOCOL_CONNECTED);
        time(&cmdIssued);
        if(m_state != SESSION_DISCONNECTED)
        {
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"SESSION not disconnected");
           MrcpV2RefLib::CSignalingDisconnectCommand* l_disconnectCommand = new CSignalingDisconnectCommand(m_signalObj);
           CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_disconnectCommand);
        }
        else
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"SESSION DISCONNECTED");
	
        if (m_ttsResource)
        {
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"TTS RESOURCE");
           if (m_speechSynthResource->IsStateIdle())
           {
              CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"TTS RESOURCE IDLE");
              m_ttsComplete = true;
              if((!m_asrResource || m_asrComplete) && m_state==SESSION_DISCONNECTED && !m_sessionCompleteSent)
              {
                  CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Notify sent TTS StopSession");
                  NotifyObservers(SESSION_COMPLETE,this);
                  m_sessionCompleteSent = true;
              }
           }
           else
           {
              CCmdStop* l_stopCmds = new CCmdStop(m_speechSynthResource);
              CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask(l_stopCmds);
              m_speechSynthResource->RegisterForNotification(this,SYNTH_IDLE_STATE_REACHED);
           }
        }
        if(m_asrResource)
        {
            CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"ASR RESOURCE");
            if (m_speechRecResource->IsStateIdle())
            {
               CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"ASR RESOURCE IDLE");
               m_asrComplete = true;
               if ((!m_ttsResource || m_ttsComplete) && m_state==SESSION_DISCONNECTED && !m_sessionCompleteSent)
               {
                   NotifyObservers(SESSION_COMPLETE, this);
                   m_sessionCompleteSent = true;
               }
            }
            else
            {
               m_speechRecResource->RegisterForNotification(this,RECOG_IDLE_STATE_REACHED);
               CCmdStop* l_stopCmds = new CCmdStop(m_speechRecResource);
               CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask(l_stopCmds);
            }
        }

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - AddResourcesToSession
// Input - resource type
//         
// Output - success or failure
//    verify session exists and resource not already in use
//    Can only have 1 resource of a type per session.
//    return a status 
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::AddResourceToSession(MrcpResourceType a_resourceToAdd)
{
	Name("AddResourceToSession");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering AddResourceToSession");

        if (SignalUnsolictBye())
           return -2;
        if (SignalCommFailed())
          return -3;

        m_resourceAdded = true;
        std::string tmpLogMsg;
        tmpLogMsg = "Asr Handle = " + MrcpUtils::itos( m_asrSessionHandle) + "Tts Handle = " + MrcpUtils::itos (m_ttsSessionHandle);
        CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, tmpLogMsg);

        std::string tmpLogMsg2;
        tmpLogMsg2 = "Asr Handle CFG = " + MrcpUtils::itos(m_resourceCfg->asrServerConfig->sessionHandle) + "Tts Handle = " + MrcpUtils::itos (m_resourceCfg->ttsServerConfig->sessionHandle);
        CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, tmpLogMsg2);

	//only 1 instance of a resource type per session
	if((a_resourceToAdd == speechsynth && m_ttsResource && !SignalUnsolictBye()) ||
		(a_resourceToAdd == speechrecog && m_asrResource && !SignalUnsolictBye()))
	{
 	     std::string l_errorInfo = "Resource already attached to session ";
    	     CLogger::Instance()->Log( LOG_LEVEL_ERROR, *this,l_errorInfo);
	     return -1;
	}
        
        if(a_resourceToAdd == speechsynth)
        {
	    m_speechSynthResource = new CSpeechSynthesizer(m_resourceCfg, m_signalObj);
	    m_sessionCmdID = m_cmdIdMgr->GetReferenceId();
	    int l_status = m_cmdIdMgr->RegisterCommandObj(m_sessionCmdID, m_resourceCfg->ttsServerConfig->callBackInfo);
            m_ttsSessionHandle = m_sessionHandle;
            m_ttsResource=true;
        }
        if(a_resourceToAdd == speechrecog)
        {
	    m_speechRecResource = new CSpeechRecognizer(m_resourceCfg, m_signalObj); 
	    m_sessionCmdID = m_cmdIdMgr->GetReferenceId();
    	    int l_status = m_cmdIdMgr->RegisterCommandObj(m_sessionCmdID, m_resourceCfg->asrServerConfig->callBackInfo);
            m_asrSessionHandle = m_sessionHandle;
            m_asrResource=true;
        }

        m_signalObj->TtsSessionHandle(m_ttsSessionHandle);
        m_signalObj->AsrSessionHandle(m_asrSessionHandle);

//	if (m_mrcpAsrVersion == 2 || m_mrcpTtsVersion == 2)
	{
                m_state = SESSION_CONNECTING;
		CSignalingReinviteCommand* l_reinviteTask = new CSignalingReinviteCommand(m_signalObj, a_resourceToAdd);
                CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_reinviteTask);
	}
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - HandleEvent
// Input - pointer to event object
//         
// Output - None
//      process observer notification.
///////////////////////////////////////////////////////////////////////////
void CMrcpSession::HandleEvent(CMrcpEvent* a_event)
{
	Name("HandleEvent");

        long sec, msec ;
        std::string rptStr;
	MrcpV2RefLib::CSignalingStartCommand* l_startCommand = NULL;
        CMrcpSipSignal* l_eventSigPtr;
        AMrcpSignaling* l_absSigPtr;
        const char* l_eventAddress;
        int l_connectBackoff = m_signalObj->ConnectBackoff();

	switch (a_event->EventID())
	{
	case SIGNALING_PROTOCOL_CONNECTED:
           l_eventSigPtr = (CMrcpSipSignal*)a_event->EventData();
           if(l_eventSigPtr == m_signalObj)
           {
             CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Signaling connected received ");
             if(!m_resourceAdded)
//             CMrcpSrvrWatcher::Instance()->RegisterServer(m_serverAddress,m_serverPort,m_keepAliveInterval,m_keepAliveCnt,l_connectBackoff);
               clock_gettime(CLOCK_REALTIME, &m_sessionStop);
               sec = m_sessionStop.tv_sec - m_sessionStart.tv_sec ;
               msec= (sec * 1000) + m_sessionStop.tv_nsec / 1000000 ;
               msec -= m_sessionStart.tv_nsec / 1000000 ;

                std::string l_message = "Protocol Connected - SETUP time = " + MrcpUtils::itos(msec) + " msec";
    	        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,l_message);
		m_state = SESSION_CONNECTED;
//try adding resource for MrcpV1
                if (m_multiResourceServer && (m_mrcpAsrVersion == 1 || m_mrcpTtsVersion == 1) && !m_resourceAdded)
                {
//Byte me
                   MrcpResourceType a_resourceToAdd = speechsynth;
		   CSignalingReinviteCommand* l_reinviteTask = new CSignalingReinviteCommand(m_signalObj, a_resourceToAdd);
                   CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_reinviteTask);
                   m_resourceAdded = true;
                   break;
                }
                else if (m_multiResourceServer && (m_mrcpAsrVersion == 1 || m_mrcpTtsVersion == 1)&& m_resourceAdded)
                {
                    PerformSessionCallback(0);
                    break;
                }
                PerformSessionCallback(0);
           }
		break;
	case SIGNALING_PROTOCOL_FAILED:
              l_eventSigPtr = (CMrcpSipSignal*)a_event->EventData();
              if(l_eventSigPtr == m_signalObj)
              {
//                CMrcpSrvrWatcher::Instance()->UnregisterServer(m_serverAddress, false);
		CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Signaling Socket Failed");
//                m_state = SESSION_DISCONNECTED;
		PerformSessionCallback(-1);
              }
		break;
        case COMMAND_SOCKET_FAILED:
              l_eventSigPtr = (CMrcpSipSignal*)a_event->EventData();
              if(l_eventSigPtr == m_signalObj)
              {
//                CMrcpSrvrWatcher::Instance()->UnregisterServer(m_serverAddress, false);
		CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Command Socket Failed");
		PerformSessionCallback(-4);
              }
		break;        
	case SIGNALING_PROTOCOL_DISCONNECTED:
           l_eventSigPtr = (CMrcpSipSignal*)a_event->EventData();
           if (l_eventSigPtr == m_signalObj)
           {
//                CMrcpSrvrWatcher::Instance()->UnregisterServer(m_serverAddress, false);
    	        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Protocol Disconnected");
		m_state = SESSION_DISCONNECTED;
		PerformSessionCallback(1);
                if (m_speechSynthResource)
                   m_speechSynthResource->Unregister(this, SYNTH_IDLE_STATE_REACHED);
                if (m_speechRecResource)
                   m_speechRecResource->Unregister(this, RECOG_IDLE_STATE_REACHED);
//                if((m_asrResource && m_ttsResource && m_asrComplete && m_ttsComplete) ||
//                   (m_asrResource && !m_ttsResource && m_asrComplete) ||
//                   (m_ttsResource && !m_asrResource && m_ttsComplete) && !m_sessionCompleteSent)
                {
                   NotifyObservers(SESSION_COMPLETE, this);
                   m_sessionCompleteSent = true;
                }
           }
		break;
      case SIGNALING_PROTOCOL_UNSOLICT_BYE:
           l_eventSigPtr = (CMrcpSipSignal*)a_event->EventData();
           if (l_eventSigPtr == m_signalObj)
           {
//                CMrcpSrvrWatcher::Instance()->UnregisterServer(m_serverAddress, false);
                CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"Protocol Disconnected");
                m_state = SESSION_DISCONNECTED;
                if (m_speechSynthResource)
                   m_speechSynthResource->Unregister(this, SYNTH_IDLE_STATE_REACHED);
                if (m_speechRecResource)
                   m_speechRecResource->Unregister(this, RECOG_IDLE_STATE_REACHED);
           }
           break;
        case MRCP_SERVER_UNREACHABLE:
           l_eventAddress = (const char*)a_event->EventData();
           if(!strncmp(l_eventAddress,m_serverAddress.c_str(),sizeof(m_serverAddress)))
           {
             CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Received MRCP Server Unreachable message");
             m_state = SESSION_DISCONNECTED;
//             CMrcpSrvrWatcher::Instance()->UnregisterServer(m_serverAddress, false);
             CSignalingCommsFailed* l_commsFailed = new CSignalingCommsFailed(m_signalObj);
             CMrcpTaskProcessor::Instance()->QueueTask(l_commsFailed);
           }
           break;
        case SYNTH_IDLE_STATE_REACHED:
           l_absSigPtr = (AMrcpSignaling*)a_event->EventData();
           if (l_absSigPtr == (AMrcpSignaling*)m_signalObj)
           {
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"SYNTH_IDLE_STATE_REACHED received");
              m_ttsComplete = true;
/*              if (!m_sessionCompleteSent && m_state != SESSION_DISCONNECTED)
              {
                CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(m_signalObj);
                CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);
//                CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_stopCommand);
                PerformSessionCallback(1);
                NotifyObservers(SESSION_COMPLETE, this);
                m_sessionCompleteSent = true;
              }

              if (!m_asrResource && m_state == SESSION_DISCONNECTED && !m_sessionCompleteSent)
              {
                   NotifyObservers(SESSION_COMPLETE, this);
                   m_sessionCompleteSent = true;
              }
*/
              m_speechSynthResource->Unregister(this, SYNTH_IDLE_STATE_REACHED);

           }
           break;
        case RECOG_IDLE_STATE_REACHED:
          l_absSigPtr = (AMrcpSignaling*)a_event->EventData();
          if (l_absSigPtr == (AMrcpSignaling*)m_signalObj)
          {
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"RECOG_IDLE_STATE_REACHED received");
              m_asrComplete = true;
//              CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(m_signalObj);
//              CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);
/*
              if (!m_ttsResource || m_ttsComplete && m_state==SESSION_DISCONNECTED && !m_sessionCompleteSent)
              {
                NotifyObservers(SESSION_COMPLETE, this);        
                m_sessionCompleteSent = true;
              }
*/
              m_speechRecResource->Unregister(this, RECOG_IDLE_STATE_REACHED);
          }
          break;
        }   
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PerformSessionCallback
// Input - None
//         
// Output - None
//     setup callback to the client application that the session is setup
////////////////////////////////////////////////////////////////////////////	
void CMrcpSession::PerformSessionCallback(int a_status, int a_sequence)
{
	Name("PerformSessionCallback");
	MRCP_RESULT_STRUCT l_callResults;
	MrcpCallBackInfo l_callbackInfo;
        std::string l_serverAddress;

    	CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering Routine");
        std::string tmpLogMsg;
        tmpLogMsg = "Asr Handle = " + MrcpUtils::itos(m_asrSessionHandle) + "Tts Handle = " + MrcpUtils::itos ( m_ttsSessionHandle) + "a_status =" + MrcpUtils::itos(a_status);
        CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, tmpLogMsg);
        tmpLogMsg.erase();
        memset(&l_callResults, 0, sizeof(MRCP_RESULT_STRUCT));
        l_callResults.MrcpSrvrAddress[0] = 0;
        l_callResults.MrcpReturnPkt[0] = 0;

	l_callResults.MrcpReturnValue = a_status;
	l_callResults.referenceNumber = 0;
	l_callResults.asrSessionHandle = m_asrSessionHandle;
        l_callResults.ttsSessionHandle = m_ttsSessionHandle;
        l_serverAddress = m_serverAddress;
        if (!l_serverAddress.empty())
        {
           strncpy(l_callResults.MrcpSrvrAddress, l_serverAddress.c_str(),l_serverAddress.size()+1);
    	   CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"RESULTS MrcpSrvrAdress", l_callResults.MrcpSrvrAddress);
        }
	if (a_status == 1)
        {
		strncpy(l_callResults.MrcpReturnPkt,"Session Disconnected",21);
    	        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"SessionCallback Disconnected");
        }
	else if (a_status == 0)
		strncpy(l_callResults.MrcpReturnPkt, "Session Connected",18);
	else if (a_status == -1 || a_status == -4)
             {
		strncpy(l_callResults.MrcpReturnPkt, "Session not connected",22);
             }
        else if (a_status == -2)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Perform Session Callback -2");
           std::string l_strId = MrcpUtils::itos(a_sequence);
           std::string l_messageBuf = "Unsolicited Bye not connected COMPLETE ID=" + l_strId + "";
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Perform Session Callback message =",l_messageBuf);
           CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(m_signalObj);
           l_processResults->MessageBuffer(l_messageBuf);
           CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
           return;
        }
        else if (a_status = -3)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Perform Session Callback -3");
           std::string l_strId = MrcpUtils::itos(a_sequence);
           std::string l_messageBuf = "Communication lost MRCP server COMPLETE ID=" + l_strId + "";
           CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(m_signalObj);
           l_processResults->MessageBuffer(l_messageBuf);
           CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
           return;
        }
        else
        {
           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "UNEXPECTED VALUE =",a_status);
           strncpy(l_callResults.MrcpReturnPkt, "\n",2);
        }
        l_callResults.MrcpSrvrRtpPort = m_signalObj->ServerRtpPort();
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Server RTP PORT = ",l_callResults.MrcpSrvrRtpPort);

        l_callbackInfo.callbackHandle = NULL;
	if (m_cmdIdMgr->GetCommandObj(m_sessionCmdID, l_callbackInfo) == MrcpSuccess)
        {
            CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"GetCommandObj PATH");
            l_callResults.MrcpCallBackHandle = (void*)&l_callbackInfo;
            CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"GetCommandObj Handle = ",l_callResults.MrcpCallBackHandle);
	    m_cmdIdMgr->PerformCallbackProcessing(l_callResults,true,l_callbackInfo);
        }
	else
	if((m_asrResource)||(m_multiResourceServer))
        {
		l_callResults.MrcpCallBackHandle = (void*)&m_asrCallbackInfo;
    	        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"m_asrResource || m_multiResourceServer Path");
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this," SIGNAL PTR3 = ",m_signalPtr);
		m_cmdIdMgr->PerformCallbackProcessing(l_callResults,true,m_asrCallbackInfo);
    	        CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"m_asrResource || m_multiResourceServer Path");
        }
        else       
	if ((m_ttsResource)&& (!m_multiResourceServer))
        {
               l_callResults.MrcpCallBackHandle = (void*)&m_ttsCallbackInfo;  
               m_cmdIdMgr->SignalPtr(m_signalPtr);
               CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SIGNAL PTR4 = ",m_signalPtr);
	       m_cmdIdMgr->PerformCallbackProcessing(l_callResults,true,m_ttsCallbackInfo);
    	       CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,"m_ttsResource && ! m_multiResourceServer Path");
        }

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - DefineGrammar
// Input - define grammar parameters
//         
// Output - success or failure
//  Description define grammar object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::DefineGrammar(DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams)
{
	Name("DefineGrammar");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

        int l_cmdID = m_cmdIdMgr->GetReferenceId();
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_defineGrammarParams->callBack);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	  CRecDefineGrammar* DefineGrammarCmd = new CRecDefineGrammar(l_cmdID,m_speechRecResource, a_defineGrammarParams);
	  CMrcpTaskProcessor::Instance()->QueueTask(DefineGrammarCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - recognize parameters structure
//         
// Output - success or failure
// Description Recognized object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::Recognize(RECOGNIZE_STRUCT* a_recognizeParams)
{
        Name("Recognize");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_recognizeParams->referenceNumber = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_recognizeParams->callBack);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CRecRecognize* l_RecognizeCmd = new CRecRecognize(l_cmdID, m_speechRecResource, a_recognizeParams->cancelQueue, a_recognizeParams->contentType, a_recognizeParams->gramContent, a_recognizeParams->startInputTimers, a_recognizeParams->recTimeout);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_RecognizeCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_RecognizeCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpret
// Input - interpret structure
//         
// Output - success or failure
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::Interpret(INTERPRET_STRUCT* a_interpretParams)
{
        Name("Interpret");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID =  m_cmdIdMgr->GetReferenceId();
        int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_interpretParams->callBack);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }
	
        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
           CRecInterpret* l_interpretCmd = new CRecInterpret(l_cmdID, m_speechRecResource, a_interpretParams->contentType, 
	  		                                  a_interpretParams->interpretText,a_interpretParams->contentId,
							  a_interpretParams->content);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_interpretCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_interpretCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);


	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartInputTimers
// Input - callback function
//		   reference number
//         
// Output - success or failure
//  Description StartInputTimers object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::StartInputTimers(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("StartInputTimers");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if (!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CRecStartInputTimers* l_startInputTimersCmd = new CRecStartInputTimers(l_cmdID, m_speechRecResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_startInputTimersCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_startInputTimersCmd);
	}

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetResult
// Input - callback function
//         reference number
// Output - success or failure
// Description - Get Result object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::GetResult(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("GetResult");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye()&& !SignalCommFailed())
        {
	   CRecGetResult* l_getResultCmd = new CRecGetResult(l_cmdID, m_speechRecResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_getResultCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_getResultCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopRec
// Input - callback function
//         reference number
// Output - success or failure
// Description stop for recognition object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::StopRecog(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("StopRecog");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");
       
        StopPending(true); 
	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if (!SignalUnsolictBye() && !SignalCommFailed())
        {	
           CRecStopRec* l_stopRecCmd = new CRecStopRec(l_cmdID, m_speechRecResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_stopRecCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_stopRecCmd);
        }
        
        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speak
// Input - speak parameters structure
//         
// Output - success or failure
// Description - speak object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::Speak(SPEAK_STRUCT* a_speakParams)
{
        Name("Speak");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_speakParams->referenceNumber = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_speakParams->callBack);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSpkSpeak* l_speakCmd = new CSpkSpeak(l_cmdID, m_speechSynthResource,a_speakParams);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_speakCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_speakCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !TtsResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSpeak
// Input - callback function
//         reference number
// Output - success or failure
// Description stop for speak object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::StopSpeak(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("StopSpeak");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

        StopPending(true);

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if (!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSpkStop* l_stopSpeakCmd = new CSpkStop(l_cmdID, m_speechSynthResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_stopSpeakCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_stopSpeakCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !TtsResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Pause
// Input - callback function
//         reference number
// Output - success or failure
// Description -pause object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::Pause(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("Pause");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSpkPause* l_spkPauseCmd = new CSpkPause(l_cmdID, m_speechSynthResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_spkPauseCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_spkPauseCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !TtsResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Resume
// Input - callback function
//         reference number
// Output - success or failure
// description resume object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::Resume(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("Resume");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if (!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSpkResume* l_resumeCmd = new CSpkResume(l_cmdID, m_speechSynthResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_resumeCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_resumeCmd);
        } 

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !TtsResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - BargeIn
// Input - callback function
//         reference number
// Output - success or failure
// Description Barge In object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::BargeIn(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum)
{
        Name("BargeIn");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSpkBargeIn* l_bargeInCmd = new CSpkBargeIn(l_cmdID, m_speechSynthResource);
	   CMrcpTaskProcessor::Instance()->QueueTask(l_bargeInCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_bargeInCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !TtsResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetParams
// Input - call back function
//		   structure of parameters
//         reference number
// Output - success or failure
// Description - Get Params object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::GetParams(MrcpCallBackInfo a_callBackInfo,GET_PARAMETERS a_parameters, int& a_referenceNum)
{
        Name("GetParams");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

	int l_sessionVersion = 2;
	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
           if (m_asrResource && m_ttsResource)
  	     l_sessionVersion = m_mrcpAsrVersion;
	   else 
	   if (m_asrResource)
		l_sessionVersion = m_mrcpAsrVersion;
	   else
	   if (m_ttsResource)
		l_sessionVersion = m_mrcpTtsVersion;

	   CGetParams* l_getParamsCmd = new CGetParams(l_cmdID, m_genericCommandResource, l_sessionVersion);
	   l_getParamsCmd->m_parameters = a_parameters;
	   CMrcpTaskProcessor::Instance()->QueueTask(l_getParamsCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_getParamsCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SetParams
// Input - callback function
//         set parameters
//         reference number
// Output - success or failure
// Description - set params object instantiated and queued.  
//			returns the sequence number assigned for command processing at
//          MRCP server.  
///////////////////////////////////////////////////////////////////////////
int CMrcpSession::SetParams(MrcpCallBackInfo a_callBackInfo,SET_PARAMETERS a_parameters, int& a_referenceNum)
{
        Name("SetParams");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"Entering ");

//	int l_sessionVersion = 2;
        int l_sessionVersion = m_mrcpTtsVersion;
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,"VERSION = ",m_mrcpAsrVersion);

	int l_cmdID = m_cmdIdMgr->GetReferenceId();
	a_referenceNum = l_cmdID;
	int l_status = m_cmdIdMgr->RegisterCommandObj(l_cmdID, a_callBackInfo);
        { // scope for lock
          boost::mutex::scoped_lock l_controlLock (m_listMutex);
          m_pendingList.push_back(l_cmdID);
        }

        if(!SignalUnsolictBye() && !SignalCommFailed())
        {
	   CSetParams* l_setParamsCmd = new CSetParams(l_cmdID, m_genericCommandResource, l_sessionVersion);
	   l_setParamsCmd->m_parameters = a_parameters;
	   CMrcpTaskProcessor::Instance()->QueueTask(l_setParamsCmd);
//	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask(l_setParamsCmd);
        }

        if (SignalUnsolictBye())
          PerformSessionCallback(-2,l_cmdID);
        if (SignalCommFailed())
          PerformSessionCallback(-3,l_cmdID);
        if (!SignalCommFailed() && !SignalUnsolictBye() && !AsrResource())
          PerformSessionCallback(-2,l_cmdID);

	return MrcpSuccess;
	}

}//end namespace
