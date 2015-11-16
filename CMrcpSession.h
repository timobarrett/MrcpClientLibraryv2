///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSession.h
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
#ifndef CMrcpSession_H
#define CMrcpSession_H

#include "AMrcpEventHandler.h"
#include "AMrcpObserverSubject.h"
#include "ClientInterfaceDefs.h"
#include "CLogger.h"
//////////////////////

#include <string>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <list>

typedef enum 
{
   SESSION_STARTING,   
   SESSION_CONNECTING,
   SESSION_CONNECTED,
   SESSION_DISCONNECTING,
   SESSION_DISCONNECTED,
   SESSION_STOPPING
} SessionStateEnum;

namespace MrcpV2RefLib
{
	class AMrcpSignaling;
	class CMrcpTaskProcessor;
	class CMrcpCmdIdMgr;
	class CSpeechRecognizer;
	class CSpeechSynthesizer;
	class CMrcpEvent;
	class CMrcpGeneric;
	class CMrcpHandleMgr;


	class CMrcpSession :public AMrcpEventHandler
                           ,public AMrcpObserverSubject
	{
	public:
		CMrcpSession(RESOURCE_CFG_STRUCT* a_resourceCfg, bool a_asrResource, bool a_ttsResource);
		virtual ~CMrcpSession();
		int StartSession();
		int StopSession();
		int AddResourceToSession(MrcpResourceType a_resourceToAdd);
		// commands
		int DefineGrammar(DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams);
		int Recognize(RECOGNIZE_STRUCT* a_recognizeParams);
		int Interpret(INTERPRET_STRUCT* a_interpretParams);
		int StartInputTimers(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum);
		int GetResult(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum);
		int StopRecog(MrcpCallBackInfo a_callBackInfo, int& a_referenceNum);
		/////////
		int Speak(SPEAK_STRUCT* a_speakParams);
		int StopSpeak(MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int Pause(MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int Resume(MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int BargeIn(MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		////////
		int GetParams(MrcpCallBackInfo a_callBackInfo, GET_PARAMETERS a_parameters, int& a_referenceNumber);
		int SetParams(MrcpCallBackInfo a_callBackInfo, SET_PARAMETERS a_parameters, int& a_referenceNumber);

		std::string ServerAddress(){return m_serverAddress;};
                void ServerAddress(std::string a_serverAddr){m_serverAddress = a_serverAddr;};
		int ServerPort() {return m_serverPort;};
                void ServerPort(int a_srvrPort){m_serverPort = a_srvrPort;};
		std::string ClientAddress() {return m_clientAddress;};
		//added IPMS
		int ClientPort() {return m_clientPort;};
                std::string SessionCodec() {return m_codec;};
		MrcpCallBackFunction AsrCallBackFunction() { return m_asrCallbackInfo.callbackFunction;};
		MrcpCallBackHandle AsrCallBackHandle() { return m_asrCallbackInfo.callbackHandle;};
		MrcpCallBackFunction TtsCallBackFunction() { return m_ttsCallbackInfo.callbackFunction;};
		MrcpCallBackHandle TtsCallBackHandle() { return m_ttsCallbackInfo.callbackHandle;};
		bool AsrResource() {return m_asrResource;};
		bool TtsResource() {return m_ttsResource;};
                void MultiResourceServer(bool a_multiResource){m_multiResourceServer = a_multiResource;};
                bool MultiResourceServer(){return m_multiResourceServer;};

                void ServerDiscntCleanup() { m_asrResource = false;
                                             m_ttsResource = false;
                                             m_multiResourceServer = false;};
		const char* StateDesc() { return "N/A";}		
	        const std::string& Name(){{ // scope for lock
                  boost::mutex::scoped_lock l_controlLock (m_loggerMutex);
                  return m_name;
                  }};
                void Name( const std::string& a_name){ { // scope for lock
                  boost::mutex::scoped_lock l_controlLock( m_loggerMutex);
                  m_name = a_name;
                  }};
	        const std::string& ClassName(){return m_className;};
		void ClassName(const std::string& a_className) {m_className = a_className;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

                void RemovePendingFromList(int a_pendCmdId){{boost::mutex::scoped_lock l_controlLock (m_listMutex); m_pendingList.remove(a_pendCmdId);}};
                int GetPendingFromList(){
                                         int a_pendingCmd = 0;
                                         { //scope for lock
                                         boost::mutex::scoped_lock l_controlLock (m_listMutex);
                                         a_pendingCmd =m_pendingList.front();
                                         if (a_pendingCmd) m_pendingList.pop_front();
                                         }
                                         return a_pendingCmd;};
               int GetPendingListSize(){return m_pendingList.size();};

                void SignalCommFailed(bool a_signalStatus){m_signalCommFail = a_signalStatus;
                                                           m_state = SESSION_DISCONNECTED;};
                bool SignalCommFailed(){return m_signalCommFail;};
                void SignalUnsolictBye(bool a_signalBye){m_byeReceived = a_signalBye;
                                                         m_state = SESSION_DISCONNECTED;};
                bool SignalUnsolictBye(){return m_byeReceived;};

                void StopPending(bool a_stopPend){m_stopPending = a_stopPend;};
                bool StopPending() {return m_stopPending;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
		SessionStateEnum m_state;
		RESOURCE_CFG_STRUCT* m_resourceCfg;
		void HandleEvent(CMrcpEvent* a_event);
		void PerformSessionCallback(int a_status, int a_seqId = 0);
		
		MrcpV2RefLib::CMrcpCmdIdMgr* m_cmdIdMgr;
		MrcpV2RefLib::CMrcpTaskProcessor* m_mrcpTaskProcessor;
		MrcpV2RefLib::CSpeechRecognizer* m_speechRecResource;
		MrcpV2RefLib::CSpeechSynthesizer* m_speechSynthResource;
		MrcpV2RefLib::CMrcpGeneric* m_genericCommandResource;
		MrcpV2RefLib::AMrcpSignaling* m_signalObj;
		MrcpV2RefLib::CMrcpHandleMgr* m_handleMgr;
                boost::mutex m_loggerMutex;
                typedef std::list<int> PendingMrcpTaskList;
                PendingMrcpTaskList m_pendingList;
                boost::mutex m_listMutex;
		std::string m_serverAddress;
		int m_serverPort;
		//added IPMS
		int m_clientPort;
                int m_asrSessionHandle;
                int m_ttsSessionHandle;
                int m_keepAliveInterval;
                int m_keepAliveCnt;
		std::string m_clientAddress;
                std::string m_codec;
		MrcpCallBackInfo m_asrCallbackInfo;
		MrcpCallBackInfo m_ttsCallbackInfo;
		int  m_mrcpAsrVersion;
		int m_mrcpTtsVersion;
		int m_sessionCmdID;
		bool m_asrResource;
                bool m_asrComplete;
		bool m_ttsResource;
                bool m_ttsComplete;
                bool m_multiResourceServer;
                bool m_byeReceived;
                bool m_signalCommFail;
                bool m_resourceAdded;
                bool m_stopping;
                bool m_sessionCompleteSent;
                bool m_stopPending;
		MrcpSessionHandle m_sessionHandle;
                struct timespec m_sessionStart;
                struct timespec m_sessionStop;

	    std::string m_className;
	    std::string m_name;
            std::string m_signalPtr;
 

	};

}//end namespace

#endif


