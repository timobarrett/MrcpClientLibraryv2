///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSystem.h
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

///////////////////////////////////////////////////////////////////////////////
#ifndef CMrcpSystem_H
#define CMrcpSystem_H

#include <boost/thread/mutex.hpp>
#include <map>

#include "Singleton.h"
#include "ClientInterfaceDefs.h"
#include "CLogger.h"
#include "AMrcpEventHandler.h"

namespace MrcpV2RefLib
{
///////////////////////////////////////////////////////////////////////////////
// Enumeration: ::MrcpSystemStateEnum
//
// Description: Mrcp System states
//
///////////////////////////////////////////////////////////////////////////////
	typedef enum 
	{	
		SYSTEM_STARTING,
		SYSTEM_RUNNING,
		SYSTEM_STOPPING,
		SYSTEM_STOPPED
	} MrcpSystemStateEnum;

	class CMrcpSession;
	class CMrcpSessionMgr;
        class CMrcpServerMgr;
	class CMrcpCmdIdMgr;
	class CMrcpTaskProcessor;
	class CMrcpHandleMgr;
	class CMrcpEvent;
        class CMrcpSrvrWatcher;
	
	class CMrcpSystem  :public Singleton <CMrcpSystem>
		               ,public AMrcpEventHandler
	{
		friend class Singleton<CMrcpSystem>;

	public:
		virtual ~CMrcpSystem();
		int Start(int a_logLevel);
		int Stop();
		int OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg);
		int AddSessionResources(MrcpSessionHandle a_sessionHandle,MrcpResourceType a_resourceToAdd);
		int CloseSession(MrcpSessionHandle a_sessionHandle);
		// commands
		int DefineGrammar(MrcpSessionHandle a_sessionHandle,DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams);
		int Recognize(MrcpSessionHandle a_sessionHandle,RECOGNIZE_STRUCT* a_recognizeParams);
		int Interpret(MrcpSessionHandle a_sessionHandle,INTERPRET_STRUCT* a_interpretParams);
//		int StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo, int& a_referenceNum);
		int StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int& a_referenceNum);
//		int GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
//		int StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
		int GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		/////////
		int Speak(MrcpSessionHandle a_sessionHandle,SPEAK_STRUCT* a_speakParams);
//		int StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
//		int Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
//		int Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
//		int BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackFunction a_callBackInfo,int& a_referenceNum);
		int StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		int BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int& a_referenceNum);
		/////////
//		int GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackFunction a_callBackInfo, GET_PARAMETERS a_parameters, int& a_referenceNum);
//		int SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackFunction a_callBackInfo, SET_PARAMETERS a_parameters, int& a_referenceNum);
		int GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, GET_PARAMETERS a_parameters, int& a_referenceNum);
		int SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, SET_PARAMETERS a_parameters, int& a_referenceNum);
		////////
		int AddOutboundAudio(MrcpAudioHandle a_audioHandle, char& a_audioBuffer);
		int StartAudio(MrcpAudioHandle a_audioHandle);
		int StopAudio(MrcpAudioHandle a_audioHandle);

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
		CMrcpSystem();
		void HandleEvent(CMrcpEvent* a_event);

		MrcpV2RefLib::CMrcpTaskProcessor* m_mrcpTaskProcessor;
		MrcpV2RefLib::CMrcpTaskProcessor* m_mrcpCallbackProcessor;
                MrcpV2RefLib::CMrcpTaskProcessor* m_mrcpSipSignalProcessor;
		MrcpV2RefLib::CMrcpCmdIdMgr* m_cmdIdMgr;
		MrcpV2RefLib::CMrcpSessionMgr* m_sessionMgr;
                MrcpV2RefLib::CMrcpServerMgr*  m_serverMgr;
		MrcpV2RefLib::CMrcpHandleMgr* m_handleMgr;
                MrcpV2RefLib::CMrcpSrvrWatcher* m_serverWatcher;

		MrcpSystemStateEnum m_state;

		boost::mutex m_controlMutex;
                boost::mutex m_loggerMutex;
		std::string m_className;
         	std::string m_name;
                std::string m_signalPtr;


	};
}//end namespace
#endif

 
