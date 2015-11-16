///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpResourceState.h
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

///////////////////////////////////////////////////////////////////////////////


#ifndef AMrcpResourceState_H
#define AMrcpResourceState_H

#include <string>
#include "CLogger.h"
#include "ClientInterfaceDefs.h"


namespace MrcpV2RefLib
{
   class AMrcpResource;
   class CRecDefineGrammar;
   class CRecRecognize;
   class CRecInterpret;
   class CRecGetResult;
   class CRecStartInputTimers;
   class CRecStopRec;
   class CSpkSpeak;
   class CSpkStop;
   class CSpkPause;
   class CSpkResume;
   class CSpkBargeIn;
   class CGetParams;
   class CSetParams;
   class CCmdStop;

	class AMrcpResourceState
	{
	public:
		AMrcpResourceState();
		virtual ~AMrcpResourceState();
		virtual int Unintialized(AMrcpResource& a_state);
		virtual int DefineGrammar(AMrcpResource& a_state,CRecDefineGrammar& a_task);
		virtual int Recognize(AMrcpResource& a_state,CRecRecognize& a_task);
		virtual int Interpret(AMrcpResource& a_state,CRecInterpret& a_task);
		virtual int GetResult(AMrcpResource& a_state,CRecGetResult& a_task);
		virtual int StartInputTimers(AMrcpResource& a_state, CRecStartInputTimers& a_task);
		virtual int StopRec(AMrcpResource& a_state, CRecStopRec& a_task);
		virtual int Speak(AMrcpResource& a_state, CSpkSpeak& a_task);
		virtual int StopSpeak(AMrcpResource& a_state, CSpkStop& a_task);
		virtual int Pause(AMrcpResource& a_state, CSpkPause& a_task);
		virtual int Resume(AMrcpResource& a_state, CSpkResume& a_task);
		virtual int BargeIn(AMrcpResource& a_state, CSpkBargeIn& a_task);
		virtual int GetParams(AMrcpResource& a_state, CGetParams& a_task);
		virtual int SetParams(AMrcpResource& a_state, CSetParams& a_task);
                virtual int StopSessionCmds(AMrcpResource& a_state, CCmdStop& a_task);

		virtual int Idle(AMrcpResource& a_state);
		virtual int DefiningGrammar(AMrcpResource& a_state);
		virtual int Recognizing(AMrcpResource& a_state);
		virtual int Interpreting(AMrcpResource& a_state);
		virtual int GettingResult(AMrcpResource& a_state);
		virtual int StoppingCommand(AMrcpResource& a_state);
		virtual int StartingInputTimers(AMrcpResource& a_state);
		virtual int Stopped(AMrcpResource& a_state);
		virtual int Speaking(AMrcpResource& a_state);
		virtual int StopSpeaking(AMrcpResource& a_state);
		virtual int Pausing(AMrcpResource& a_state);
		virtual int Resuming(AMrcpResource& a_state);
		virtual int BargingIn(AMrcpResource& a_state);
		virtual int SettingParams(AMrcpResource& a_state);

		//events
		virtual int OnSettingParamsComplete(AMrcpResource& a_subject);
		virtual int OnBargeInComplete(AMrcpResource& a_subject);
		virtual int OnResumingComplete(AMrcpResource& a_subject);
		virtual int OnPausingComplete(AMrcpResource& a_subject);
		virtual int OnStopSpeakComplete(AMrcpResource& a_subject);
		virtual int OnSpeakingComplete(AMrcpResource& a_subject);
		//events ASR
		virtual int OnDefineGrammarComplete(AMrcpResource& a_subject);
		virtual int OnRecognitionComplete(AMrcpResource& a_subject);
		virtual int OnInterpretingComplete(AMrcpResource& a_subject);
		virtual int OnStartInputTimersComplete(AMrcpResource& a_subject);
		virtual int OnGettingResultComplete(AMrcpResource& a_subject);
		virtual int OnStopRecComplete(AMrcpResource& a_subject);

		void PerformBadStateCallback(int a_cmdID,MrcpSessionHandle a_asrSessionHandle, MrcpSessionHandle a_ttsSessionHandle, std::string a_serverAddress, std::string statusMsg = "Bad State for Command");
                std::string TranslateState(AMrcpResource& a_state);
		const std::string StateDesc() {return m_stateDesc;};
		void StateDesc(const std::string a_stateDesc) {m_stateDesc = a_stateDesc;};
		const std::string& ClassName(){return m_className;};
		void ClassName( const std::string& a_className){m_className = a_className;};
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
	   std::string m_name;
	   std::string m_className;
	   std::string m_stateDesc;
           std::string m_signalPtr;
           boost::mutex m_loggerMutex;
	};

}//end namespace
#endif //ifdef

