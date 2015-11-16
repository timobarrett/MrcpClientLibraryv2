///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpCallbackTask.cpp
//
//	Description:	CMrcpCallbackTask class implementation
//				  Executes callbacks received from the callback task processor
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpCallbackTask::CMrcpCallbackTask()
//		CMrcpCallbackTask::~CMrcpCallbackTask()
//		void CMrcpCallbackTask::Execute()
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpCallbackTask.h"
namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpCallbackTask::CMrcpCallbackTask()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpCallbackTask::~CMrcpCallbackTask()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Execute
// Input - None - parameters set in class by accesors before queued
//         
// Output - None
//		Uses the callback task processor to gaurd the execution thread 
//          from excessive client application delay.
///////////////////////////////////////////////////////////////////////////
void CMrcpCallbackTask::Execute()
{
//	m_callBackRtn(&m_callResults);
	m_callBackRtn(m_callResults);
        delete m_callResults;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Callresults
// Input - None - results structure 
//
// Output - None
//              Uses the callback task processor to gaurd the execution thread
//          from excessive client application delay.
///////////////////////////////////////////////////////////////////////////
void CMrcpCallbackTask::CallResults(MrcpV2RefLib::MRCP_RESULT_STRUCT a_results)
{

        m_callResults = new MrcpV2RefLib::MRCP_RESULT_STRUCT;
        m_callResults->MrcpReturnPkt[0]=0;
//        memset(&m_callResults, 0, sizeof(MRCP_RESULT_STRUCT));
        m_callResults->referenceNumber = a_results.referenceNumber;
        m_callResults->MrcpReturnValue = a_results.MrcpReturnValue;
        m_callResults->asrSessionHandle = a_results.asrSessionHandle;
        m_callResults->ttsSessionHandle = a_results.ttsSessionHandle;
//       	strcpy (m_callResults->MrcpReturnPkt, a_results.MrcpReturnPkt);
       	strncpy (m_callResults->MrcpReturnPkt, a_results.MrcpReturnPkt,strlen(a_results.MrcpReturnPkt)+1);
        strncpy (m_callResults->MrcpSrvrAddress, a_results.MrcpSrvrAddress,strlen(a_results.MrcpSrvrAddress)+1);
        m_callResults->MrcpSrvrRtpPort = a_results.MrcpSrvrRtpPort;
        m_callResults->MrcpCallBackHandle = a_results.MrcpCallBackHandle;
}

}//end namespace
