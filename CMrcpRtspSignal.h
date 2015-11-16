///////////////////////////////////////////////////////////////////////////////
//      File Name:              CMrcpSipSignal.h
//
//      Description:     class implementation
//
//      Declared in this file:
//              none
//
//      Implemented in this file:
//
///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//
//  
///////////////////////////////////////////////////////////////////////////////
#ifndef CMrcpRtspSignal_h
#define CMrcpRtspSignal_h

#include "AMrcpSignaling.h"
#include "CLogger.h"

#include </usr/include/sys/socket.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

namespace MrcpV2RefLib
{

        class CMrcpSession;
        class CMrcpCmdIdMgr;
        class CMrcpTaskProcessor;
        class CMrcpRtspSignal : public AMrcpSignaling
        {
        public:
                CMrcpRtspSignal (CMrcpSession* a_session);
                virtual ~CMrcpRtspSignal();
                bool IsRtspEventReceiverRunning(){{//scope for lock
                                                   boost::mutex::scoped_lock l_flag(m_rtspRunning);
                                                   return m_rtspEventReceiverRunning;}};
                void IsRtspEventReceiverRunning(bool a_rtspEventReceiverRunning){//scope for lock
                                                         boost::mutex::scoped_lock l_flag(m_rtspRunning);
                                                        {m_rtspEventReceiverRunning = a_rtspEventReceiverRunning;}};

                std::string SessionId() {return m_sessionID;};
                void SessionId(std::string a_sessionID) {m_sessionID = a_sessionID;};

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
                void ConnectBackoff(int a_connectBackoff){m_connectBackoff = a_connectBackoff;};
                int ConnectBackoff(){return m_connectBackoff;};

                bool AsrTeardownSent() {return m_asrTeardownSent;};
                bool TtsTeardownSent() {return m_ttsTeardownSent;};
                CMrcpSession* SessionPtr() {return m_session;};

        private:
                CMrcpSession* m_session;
                CMrcpCmdIdMgr* m_cmdIdMgr;
//                SignalingStateEnum m_state;

                boost::shared_ptr<boost::thread> m_rtspThread;
                boost::shared_ptr<boost::thread>m_commandThread;
                boost::mutex m_loggerMutex;
                boost::mutex m_rtspRunning;
                boost::mutex m_controlMutex;
                boost::mutex m_semaphoreMutex;

                int PrimStart();
	        int PrimStop();
                int PrimOnStopped();
                int PrimConnect();
                int PrimOnSignalingStarted();
                int PrimOnSignalingConnected();
		int PrimDisconnect();
                int PrimReinvite(MrcpResourceType a_resourceType);
//                int PrimOnSignalingFailed();
                int PrimSendCommand(const std::string& a_message, std::string a_messageType);
                int PrimSendSignal(const std::string& a_message);
                int PrimProcessResults(const std::string& a_message);
                int StartRtspListenThread();
                int ProcessRtspThreadEvents();
                int BuildAnnounceHdr(const std::string a_resourceType, std::string& a_announceHdr);
                MrcpCallBackFunction m_defaultCallBackFunction;
                MrcpCallBackHandle m_defaultCallBackHandle;

                int m_connectBackoff;
                std::string m_className;
                std::string m_name;
                std::string m_signalPtr;
                std::string m_sessionID;
                std::string m_callIDVal;
                bool m_rtspEventReceiverRunning;
                bool m_asrTeardownSent;
                bool m_ttsTeardownSent;
                int m_lastCseq;
                int m_synthCallSeq;
                int m_speakSeq;
                int m_recogCallSeq;
                int m_recognizeSeq;

//useless defines for virtuals  some may migrate as development progresses
           int PrimRedirect(){return 0;};
           int PrimOnSignalingFailed(){return 0;};
           int PrimSendAck(){return 0;};
           int PrimUnsolicitedByeStop(){return 0;};
           int PrimOnCommsFailed(){return 0;};
           int PrimByeOk(){return 0;};
           void SetAckToValue(std::string a_value){return;};
           void SetToTag(std::string a_value){return;};
           void SetFromTag(std::string a_value){return;};
           void SetSignalFail(bool a_value) {return;};
           void SetRedirect(bool a_sigRedirect){return;};
           bool IsSignalingStopped(){return 0;};
           bool IsSipEventReceiverRunning(){return 0;};
           bool IsCommandEventReceiverRunning(){return 0;};
           std::string GetCallID(){return "";};
           int SetupCommandProcessing(int a_commandSocket){return 0;};

        };
} //end namespace

#endif
