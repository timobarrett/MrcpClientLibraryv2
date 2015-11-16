///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSipProcessSipMsgs.h
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
//	6/21/06 	TMB 		Initial Version
//  4/1/07      TMB         modified setmessagebuffer
///////////////////////////////////////////////////////////////////////////////
#ifndef CMrcpProcessSipMsgs_h
#define CMrcpProcessSipMsgs_h


#include "MrcpTasks.h"
#include "CLogger.h"

#include <string>

#define M_ENDL "\r\n" 


namespace MrcpV2RefLib
{


	class CSipProcessMessage : public MrcpTasks
	{
	public:
		CSipProcessMessage(AMrcpSignaling* a_signalObj);
		virtual ~CSipProcessMessage();
		void Execute();
		void SetMessageBuffer(std::string a_messageBuffer) 
		{ 
			m_messageBuffer.clear();	
			m_messageBuffer = a_messageBuffer;
		};
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
                //const std::string& Name(){return m_name;};
		//void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:
	private:
	    std::string m_messageBuffer;
	    std::string m_className;
	    std::string m_name;
            std::string m_signalPtr;
            MrcpV2RefLib::AMrcpSignaling* m_signalObj;
            boost::mutex m_loggerMutex;
	};

}//end namespace

#endif
