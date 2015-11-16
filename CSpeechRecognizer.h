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

#ifndef CSpeechRecognizer_h
#define CSpeechRecognizer_h

#define MRCP_ENDL "\r\n" 

#include "AMrcpResource.h"
#include "ClientInterfaceDefs.h"
#include "AMrcpObserverSubject.h"

#include <string>

namespace MrcpV2RefLib
{
	class CMrcpSession;
	class CRecognitionCommands;
	class CMrcpCmdIdMgr;
	class AMrcpSignaling;

	class CSpeechRecognizer : public AMrcpResource
	{
	public:
		CSpeechRecognizer(RESOURCE_CFG_STRUCT* a_resourceCfg, AMrcpSignaling* a_signalObj);
		virtual ~CSpeechRecognizer();
                void CleanUp();
                bool IsStateIdle(){if(m_state == m_resourceIdleState) return true; else return false;};

	protected:

	private:
		RESOURCE_CFG_STRUCT* m_resourceCfg;  
		MrcpV2RefLib::CMrcpCmdIdMgr* m_cmdIdMgr;
		MrcpV2RefLib::CRecognitionCommands* m_recognizerCmds;

		MrcpResourceType m_resourceType;
		int m_mrcpVersion;


		int PrimDefineGrammar(CRecDefineGrammar& a_task);
		int PrimRecognize(CRecRecognize& a_task);
		int PrimInterpret(CRecInterpret& a_task);
		int PrimStartInputTimers(CRecStartInputTimers& a_task);
		int PrimGetResult(CRecGetResult& a_task);
		int PrimStopRec(CRecStopRec& a_task);
//
		int PrimSpeak(CSpkSpeak& a_task){return 0;};
		int PrimPause(CSpkPause& a_task){return 0;};
		int PrimResume(CSpkResume& a_task){return 0;};
		int PrimBargeIn(CSpkBargeIn& a_task){return 0;};
		int PrimStopSpk(CSpkStop& a_task){return 0;};
	//
		int PrimGetParams(CGetParams& a_task) {return 0;};
		int PrimSetParams(CSetParams& a_task) {return 0;};

	};

}//end namespace


#endif
