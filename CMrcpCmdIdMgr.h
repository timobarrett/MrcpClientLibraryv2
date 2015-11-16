///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpCmdIdMgr.h
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

#ifndef CMrcpCmdIdMgr_h
#define CMrcpCmdIdMgr_h

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <map>
#include "Singleton.h"
#include "ClientInterfaceDefs.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{


	class CMrcpCmdIdMgr : public Singleton<CMrcpCmdIdMgr>
	{
	   friend class Singleton<CMrcpCmdIdMgr>;

	public:
		virtual ~CMrcpCmdIdMgr();
		int GetCommandObj(long a_id, MrcpCallBackInfo &a_callBackInfo);
		int RegisterCommandObj(long a_id, MrcpCallBackInfo &a_callBackiInfo);
		int UnRegisterObject(long a_id);
		int PerformCallbackProcessing(MRCP_RESULT_STRUCT a_callResults,bool a_commandComplete, MrcpCallBackInfo a_defaultCallBackInfo);

		int GetReferenceId();
                int GetRecoSeqNum(){m_recSeqId++; return m_recSeqId;};
                int GetSynthSeqNum(){m_synSeqId++; return m_synSeqId;};

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
		CMrcpCmdIdMgr();


		typedef std::map<long, MrcpCallBackInfo> CmdByIDMap;
		CmdByIDMap m_cmdIdMap;

		boost::mutex m_controlMutex;
		boost::mutex m_criticalSection;
                boost::mutex m_loggerMutex;

	       int m_sequenceID;
	       int m_recSeqId;
	       int m_synSeqId;
	       std::string m_className;
	       std::string m_name;
               std::string m_signalPtr;

	};

} // end namespace


#endif

