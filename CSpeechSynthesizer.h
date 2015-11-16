///////////////////////////////////////////////////////////////////////////////
//	File Name:		CSpeechRecognizer.h
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

#ifndef CSpeechSynthesizer_H
#define CSpeechSynthesizer_H

#include "AMrcpResource.h"
#include "ClientInterfaceDefs.h"
#include "AMrcpObserverSubject.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{
	class CMrcpSession;
	class CSpeakCommands;
	class CMrcpCmdIdMgr;
	class AMrcpSignaling;
	class CMrcpEvent;
	
	class CSpeechSynthesizer : public AMrcpResource
	{
	public:
		CSpeechSynthesizer(RESOURCE_CFG_STRUCT* a_resourceCfg, AMrcpSignaling* a_signalObj);
		virtual ~CSpeechSynthesizer();
                void CleanUp();

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
//                const std::string& Name(){return m_name;};
//                void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};
                LogObjectTypeEnum LogObjectType(){return LogObject_System;};
                bool IsStateIdle(){if(m_state == m_resourceIdleState) return true; else return false;};

	protected:

	private:
		RESOURCE_CFG_STRUCT* m_resourceCfg;  
		MrcpV2RefLib::CMrcpCmdIdMgr* m_cmdIdMgr;
		MrcpV2RefLib::CSpeakCommands* m_speakCmds;

		MrcpResourceType m_resourceType;
		int m_mrcpVersion;

		int PrimSpeak(CSpkSpeak& a_task);
		int PrimPause(CSpkPause& a_task);
		int PrimResume(CSpkResume& a_task);
		int PrimBargeIn(CSpkBargeIn& a_task);
		int PrimStopSpk(CSpkStop& a_task);

		int PrimDefineGrammar(CRecDefineGrammar& a_task) {return 0;};
		int PrimRecognize(CRecRecognize& a_task){return 0;};
		int PrimInterpret(CRecInterpret& a_task){return 0;};
		int PrimStartInputTimers(CRecStartInputTimers& a_task){return 0;};
		int PrimGetResult(CRecGetResult& a_task){return 0;};
		int PrimStopRec(CRecStopRec& a_task){return 0;};

		int PrimGetParams(CGetParams& a_task) {return 0;};
		int PrimSetParams(CSetParams& a_task) {return 0;};
	
                boost::mutex m_loggerMutex;	
                std::string m_className;
                std::string m_name;
                std::string m_signalPtr;

	};


}//end namespace

#endif

