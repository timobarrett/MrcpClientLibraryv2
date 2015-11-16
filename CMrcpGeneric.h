///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpGeneric.h
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

#ifndef CMrcpGeneric_H
#define CMrcpGeneric_H

#include "AMrcpResource.h"
#include "ClientInterfaceDefs.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{
	class CMrcpCmdIdMgr;
	class AMrcpSignaling;
	class CGenericCommands;
	class AMrcpResource;

	class CMrcpGeneric : public AMrcpResource
	{
	public:
		CMrcpGeneric(RESOURCE_CFG_STRUCT* a_resourceCfg, AMrcpSignaling* a_signalObj);
		virtual ~CMrcpGeneric();

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
	    
    //            const std::string& Name(){return m_name;};
    //   	void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};
                bool IsStateIdle(){if(m_state == m_resourceIdleState) return true; else return false;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
		RESOURCE_CFG_STRUCT* m_resourceCfg;  
		MrcpV2RefLib::CMrcpCmdIdMgr* m_cmdIdMgr;
		MrcpV2RefLib::CGenericCommands* m_genericCmds;
                boost::mutex m_loggerMutex;

		int m_mrcpVersion;

		int PrimGetParams(CGetParams& a_task);
		int PrimSetParams(CSetParams& a_task);

		int PrimDefineGrammar(CRecDefineGrammar& a_task){return 0;};
		int PrimRecognize(CRecRecognize& a_task){return 0;};
		int PrimInterpret(CRecInterpret& a_task){return 0;};
		int PrimStartInputTimers(CRecStartInputTimers& a_task){return 0;};
		int PrimGetResult(CRecGetResult& a_task){return 0;};
		int PrimStopRec(CRecStopRec& a_task){return 0;};
//
		int PrimSpeak(CSpkSpeak& a_task){return 0;};
		int PrimPause(CSpkPause& a_task){return 0;};
		int PrimResume(CSpkResume& a_task){return 0;};
		int PrimBargeIn(CSpkBargeIn& a_task){return 0;};
		int PrimStopSpk(CSpkStop& a_task){return 0;};
//
		std::string m_className;
	        std::string m_name;
                std::string m_signalPtr;


	};

}//end namespace

#endif

