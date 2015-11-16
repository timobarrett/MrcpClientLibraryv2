///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSipSignal.h
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
#ifndef CMrcpSipSignal_h
#define CMrcpSipSignal_h

#include "AMrcpSignaling.h"
#include "CLogger.h"

#include <list>
#include <map>
#ifdef WIN32
#include <winsock2.h>
#else
#include </usr/include/sys/socket.h>
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

typedef enum
{
   SIGNAL_UNINITIALIZED,
   SIGNAL_CONNECTING,
   SIGNAL_INVITE,
   SIGNAL_NONINVITE,
   SIGNAL_ACK,
   SIGNAL_DISCONNECT
} SignalingStateEnum;

namespace MrcpV2RefLib
{

	class CMrcpSession;
	class CMrcpCmdIdMgr;
        class CMrcpTaskProcessor;
	class CMrcpSipSignal : public AMrcpSignaling
	{
	public:
		CMrcpSipSignal (CMrcpSession* a_session);
		virtual ~CMrcpSipSignal(); 

		bool IsSipEventReceiverRunning(){{//scope for lock
                                                   boost::mutex::scoped_lock l_flag(m_sipRunning);
                                                   return m_sipEventReceiverRunning;}};
		void IsSipEventReceiverRunning(bool a_sipEventReceiverRunning){//scope for lock
                                                         boost::mutex::scoped_lock l_flag(m_sipRunning);
                                                        {m_sipEventReceiverRunning = a_sipEventReceiverRunning;}};
		bool IsCommandEventReceiverRunning() {return m_commandEventReceiverRunning;};
		void IsCommandEventReceiverRunning(bool a_commandEventReceiverRunning) {m_commandEventReceiverRunning = a_commandEventReceiverRunning;};
		
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

		void SetAckToValue(std::string a_ackToVal) { m_ackToValue = a_ackToVal;};
                void SetToTag(std::string a_toTagVal) { m_ackToTag = a_toTagVal;};
                void SetFromTag(std::string a_fromTagVal) {m_byeFromTag = a_fromTagVal;};
                void SetSignalFail(bool a_sigFailVal) { m_sigFail = a_sigFailVal;};
                void SetRedirect(bool a_sigRedirect) {m_sigRedirect = a_sigRedirect;};
                bool IsSignalingStopped(){return m_allDone;};
                void ConnectBackoff(int a_connectBackoff){m_connectBackoff = a_connectBackoff;}; 
                int ConnectBackoff(){return m_connectBackoff;};
                std::string GetCallID() {return m_callIDVal;};

	protected:

	private:
		CMrcpSession* m_session;
		CMrcpCmdIdMgr* m_cmdIdMgr;
                SignalingStateEnum m_state;

		boost::shared_ptr<boost::thread> m_sipThread;
		boost::shared_ptr<boost::thread>m_commandThread;

		typedef struct sipInfoStruct
		{
			std::string callID;
			std::string branchVal;
			std::string toVal;
		};
		typedef std::map<std::string, sipInfoStruct> SipInfoMap;
		SipInfoMap m_sipInfoMap;

	   int PrimStart();
	   int PrimConnect();
           int PrimRedirect();
	   int PrimOnSignalingStarted();
           int PrimOnSignalingConnected();
           int PrimOnSignalingFailed();

	   int PrimSendCommand(const std::string& a_message, std::string a_messageType);
	   int PrimSendSignal(const std::string& a_message);
	   int PrimProcessResults(const std::string& a_message);
	   int PrimStop();
	   int PrimSendAck();
	   int PrimOnStopped();
	   int PrimDisconnect();
	   int PrimReinvite(MrcpResourceType a_resourceType);
           int PrimUnsolicitedByeStop();
           int PrimOnCommsFailed();
           int PrimByeOk();
	   int StartSipListenThread();
	   int ProcessSipThreadEvents();
	   int SetupCommandProcessing(int a_commandSocket);
	   int StartCommandListenThread();
	   int ProcessCommandThreadEvents();

	   boost::mutex m_controlMutex;
	   boost::mutex m_commandMutex;
           boost::mutex m_semaphoreMutex;
	   boost::mutex m_semaphore2Mutex;
           boost::mutex m_loggerMutex;
           boost::mutex m_sipRunning;

	   MrcpCallBackFunction m_defaultCallBackFunction;
	   MrcpCallBackHandle m_defaultCallBackHandle;
	   
           boost::condition m_semaphore;
	   bool m_sipEventReceiverRunning;
	   bool m_commandEventReceiverRunning;
           bool m_sigFail;
           bool m_sigRedirect;
           bool m_allDone;
           
           int m_inviteSeq;
           int m_connectBackoff;

	   std::string m_callIDVal;
	   std::string m_callFromVal;
	   std::string m_callBranchVal;
	   std::string m_ackToValue;
           std::string m_ackToTag;
           std::string m_byeFromTag;
           std::string m_prevInviteResource;
           std::string m_prevInviteSockDirect;
	   std::string m_className;
	   std::string m_name;
           std::string m_signalPtr;
	};
}//end namespace
#endif
