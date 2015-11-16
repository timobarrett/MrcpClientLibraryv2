
///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpCallbackTask.h
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
#ifndef CMrcpCallbackTask_H
#define CMrcpCallbackTask_H
#include "MrcpTasks.h"

namespace MrcpV2RefLib
{
	class CMrcpCallbackTask : public MrcpTasks
	{
	public:
		CMrcpCallbackTask();
		virtual ~CMrcpCallbackTask();
		void Execute();
		
//		void CallResults(MRCP_RESULT_STRUCT a_callResults){m_callResults = a_callResults;};
		void CallResults(MRCP_RESULT_STRUCT a_callResults);
		void CallBackRtn(MrcpCallBackFunction a_callBackRtn){m_callBackRtn = a_callBackRtn;
                                                                    m_callBackInfo.callbackFunction = a_callBackRtn;};
		void CallBackHndl(MrcpCallBackHandle a_callBackHndl){m_callBackHndl = a_callBackHndl;
                                                                     m_callBackInfo.callbackHandle = a_callBackHndl;};
                void SetCallBackInfo(){m_callResults->MrcpCallBackHandle = (void*)&m_callBackInfo;};
//                void SetCallBackInfo(){m_callResults.MrcpCallBackHandle = (void*)&m_callBackInfo;};

	protected:

	private:
		MRCP_RESULT_STRUCT *m_callResults;
//		MRCP_RESULT_STRUCT m_callResults;
		MrcpCallBackFunction m_callBackRtn;
		MrcpCallBackHandle m_callBackHndl;
	        MrcpCallBackInfo  m_callBackInfo;
	};

}//end namespace

#endif
