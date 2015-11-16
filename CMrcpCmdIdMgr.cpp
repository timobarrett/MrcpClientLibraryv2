///////////////////////////////////////////////////////////////////////////////
//	File Name:		MrcpCmdIdMgr.cpp
//
//	Description:	MrcpCmdIdMgr class implementation
//					Provides unique request ids for all MRCP commands sent from library
//					Provides a map of request ids to callback routines amd a lookup into map
//				    Provides means to unregister cmd id and callbacks
//					Provides method to queue callbacks for processing to callback task processor
//					
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpCmdIdMgr::CMrcpCmdIdMgr()
//		CMrcpCmdIdMgr::~CMrcpCmdIdMgr()
//		MrcpCallBackFunction CMrcpCmdIdMgr::GetCommandObj(long a_id)
//		int CMrcpCmdIdMgr::RegisterCommandObj(long a_id, MrcpCallBackFunction a_cmdobj)
//		int CMrcpCmdIdMgr::UnRegisterObject(long a_id)

///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpCmdIdMgr.h"
#include "ClientInterfaceDefs.h"
#include "CMrcpCallbackTask.h"
#include "CMrcpTaskProcessor.h"

#include <map>

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

CMrcpCmdIdMgr::CMrcpCmdIdMgr()
	:m_sequenceID (1001)
	,m_recSeqId (101)
	,m_synSeqId (101)

{
	ClassName("CMrcpCmdIdMgr");
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////

CMrcpCmdIdMgr::~CMrcpCmdIdMgr()
{

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - gets command object 
// Input - Mrcp Request ID
//         
// Output - returns pointer to object issuing command
//          Performs a lookup in the map of request ids to callbacks
//			returns callback routine
///////////////////////////////////////////////////////////////////////////
int CMrcpCmdIdMgr::GetCommandObj(long a_id,MrcpCallBackInfo &a_callBackInfo)
{  //scope for lock
  std::string debug_log;
  char logMsg[1024];
//  Name("GetCommandObj");
  std::map<long, MrcpCallBackInfo>::iterator cmdIdIter;
      {
	boost::mutex::scoped_lock lock( m_criticalSection);
        cmdIdIter = m_cmdIdMap.find(a_id);
        if(cmdIdIter == m_cmdIdMap.end())
//	if ( m_cmdIdMap.find(a_id) == m_cmdIdMap.end())
		return MrcpNonRegCmd;
	else
	{
                a_callBackInfo.callbackHandle = cmdIdIter->second.callbackHandle;
                a_callBackInfo.callbackFunction = cmdIdIter->second.callbackFunction;
		return MrcpSuccess;
	}
      }
}

////////////////////////////////////////////////////////////////////////// 
//
// Input - Mrcp Request ID, pointer to command object
//         
// Output - success or failure
//     registers the object issuing command
//      adds a map entry of command id, callback handle to command sequence id    
///////////////////////////////////////////////////////////////////////////
//int CMrcpCmdIdMgr::RegisterCommandObj(long a_id, MrcpCallBackFunction &a_callBackInfo)
int CMrcpCmdIdMgr::RegisterCommandObj(long a_id, MrcpCallBackInfo &a_callBackInfo)
{
	Name("RegisterCommandObj");
        char logMsg[1024];

	if ( a_callBackInfo.callbackFunction == NULL) 
	{
		CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Command Object is NULL");
		return -1;
	}


	if ( GetCommandObj(a_id, a_callBackInfo)!= MrcpNonRegCmd)
	{
		CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Duplicate Command Object found on lookup");
		return -1;
	}
	{//scope for lock
	boost::mutex::scoped_lock lock( m_criticalSection);
        m_cmdIdMap.insert(std::make_pair(a_id,a_callBackInfo));
	}

	return MrcpSuccess;

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - unregisters a command object 
// Input - Mrcp Request ID
//         
// Output - success or failure
//			unregisters the object issuing command
//         remove entry from map of sequence id to callback 
///////////////////////////////////////////////////////////////////////////

int CMrcpCmdIdMgr::UnRegisterObject(long a_id)
{
        Name("UnRegisterObject");
	CmdByIDMap::iterator l_pos;

	{ //scope for lock
	boost::mutex::scoped_lock lock( m_criticalSection);
	if ( (l_pos = m_cmdIdMap.find(a_id)) == m_cmdIdMap.end())
	{
		CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Command Object not found");
		return -1;
	}
	m_cmdIdMap.erase(l_pos);
	}
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - returns a unique requestID to be used to by command obj 
// Input - None
//         
// Output - requestId
//       returns a unique sequence id to be used in the MRCP call to the server
//		 sequence id should be used by client application to associate asynch
//		 statuses to MRCP commands issued.
///////////////////////////////////////////////////////////////////////////
int CMrcpCmdIdMgr::GetReferenceId()
{  //scope for lock
	boost::mutex::scoped_lock lock( m_controlMutex);
	m_sequenceID++;

	return m_sequenceID;

}
////////////////////////////////////////////////////////////////////////// 
//
// Input - command result structure
//         command complete indicator
//		   callback function
// Output - success or failure
//       looks up callback routine associated with a sequence id,
//       queue's callback command and submits to callback task processor queue
//               takes precedance over default call back for Server 
///////////////////////////////////////////////////////////////////////////
int CMrcpCmdIdMgr::PerformCallbackProcessing(MRCP_RESULT_STRUCT a_callResults,bool a_commandComplete, MrcpCallBackInfo a_defaultCallBackInfo)
{
	Name("PerformCallbackProcessing");
	CmdByIDMap::iterator l_callBackItr;
	MrcpCallBackFunction l_callBackRtn;
	MrcpCallBackHandle l_callBackHndle;

	CLogger::Instance()->Log(LOG_LEVEL_INFO, *this, "Entering Routine");
	{//scope for lock
	boost::mutex::scoped_lock lock (m_criticalSection);

	if ( (l_callBackItr = m_cmdIdMap.find(a_callResults.referenceNumber)) == m_cmdIdMap.end())
        {
		if (a_defaultCallBackInfo.callbackFunction != NULL)  // && a_defaultCallBackInfo.callbackHandle != NULL)
		{
	                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Callback function not found path");
			l_callBackRtn = a_defaultCallBackInfo.callbackFunction;
			l_callBackHndle = a_defaultCallBackInfo.callbackHandle;
		}
		else
		{
		   CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Callback not found - default not set");
		   return -1;
		}
        }
	else
	{
		CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Callback found - default not used");
		l_callBackRtn = (*l_callBackItr).second.callbackFunction;
		l_callBackHndle = (*l_callBackItr).second.callbackHandle;
                std::string debString;
  	       
		if (a_commandComplete)
			m_cmdIdMap.erase(l_callBackItr);
	}
	}//end scope lock

	CMrcpCallbackTask* l_callbackTask = new CMrcpCallbackTask();
	l_callbackTask->CallBackRtn(l_callBackRtn);
	l_callbackTask->CallBackHndl(l_callBackHndle);
	l_callbackTask->CallResults(a_callResults);
        l_callbackTask->SetCallBackInfo();
	
        CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_callbackTask);

	return MrcpSuccess;
}
}// end namespace

