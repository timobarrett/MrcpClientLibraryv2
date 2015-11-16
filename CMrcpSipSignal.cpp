///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSipSignal.cpp
//
//	Description:	CMrcpSipSignal class implementation
//				  Part of sip signaling state machine
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpSipSignal::CMrcpSipSignal (CMrcpSession* a_session);
//		CMrcpSipSignal::~CMrcpSipSignal(); 
//	   int CMrcpSipSignal::PrimStart();
//	   int CMrcpSipSignal::PrimConnect();
//	   int CMrcpSipSignal::PrimOnSignalingStarted();
//	   int CMrcpSipSignal::PrimSendCommand(const std::string& a_message);
//	   int CMrcpSipSignal::PrimSendSignal(const std::string& a_message);
//	   int CMrcpSipSignal::PrimProcessResults(const std::string& a_message);
//	   int CMrcpSipSignal::PrimStop();
//	   int CMrcpSipSignal::PrimSendAck();
//	   int CMrcpSipSignal::PrimOnStopped();
//	   int CMrcpSipSignal::PrimDisconnect();
//	   int CMrcpSipSignal::PrimReinvite(MrcpResourceType a_resourceType);
//	   int CMrcpSipSignal::StartSipListenThread();
//	   int CMrcpSipSignal::ProcessSipThreadEvents();
//	   int CMrcpSipSignal::SetupCommandProcessing(int a_commandSocket);
//	   int CMrcpSipSignal::StartCommandListenThread();
//	   int CMrcpSipSignal::ProcessCommandThreadEvents();
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "CMrcpSipSignal.h"
#include "MrcpClientEvents.h"
#include "CMrcpSession.h"
#include "ClientInterfaceDefs.h"
#include "CMrcpProcessSipMsgs.h"
#include "CMrcpSendCommand.h"
#include "CMrcpSendSignal.h"
#include "CMrcpCmdIdMgr.h"
#include "MrcpUtils.h"
#include "CMrcpTaskProcessor.h"

#include <errno.h>
#include <pthread.h>
namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - session object pointer
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////

CMrcpSipSignal::CMrcpSipSignal(CMrcpSession* a_session)
	: m_session (a_session)
	, m_sipEventReceiverRunning (false)
	, m_commandEventReceiverRunning (false)
        , m_state(SIGNAL_UNINITIALIZED)
        , m_allDone(false)
        , m_sigRedirect(false)
        , m_sigFail(false)
{
	ClassName("CMrcpSipSignal");
        struct timespec l_sigTime;
        clock_gettime(CLOCK_REALTIME, &l_sigTime);
        SignalPtr(MrcpUtils::itos(int(this)));
        m_prevInviteResource.erase();
	m_cmdIdMgr = CMrcpCmdIdMgr::Instance();
	if (m_session->AsrResource())
	{
		m_defaultCallBackFunction = m_session->AsrCallBackFunction();
                m_defaultCallBackHandle = m_session->AsrCallBackHandle();
	}
	if (m_session->TtsResource())
	{
		m_defaultCallBackFunction = m_session->TtsCallBackFunction();
                m_defaultCallBackHandle = m_session->TtsCallBackHandle();
	}
	m_serverAddress = m_session->ServerAddress();
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " server address ",m_serverAddress);
      
	m_serverPort = m_session->ServerPort();
	m_clientAddress = m_session->ClientAddress();
	//added IPMS
	m_clientRtpPort = m_session->ClientPort();
        //srand(time(NULL)+this);
        //time granularity is in seconds modified my random number seed to:
      
//        srand((unsigned int)this + time(NULL) + m_session->ClientPort());
          srand((unsigned int)this + l_sigTime.tv_nsec);

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSipSignal::~CMrcpSipSignal()
{
       if (IsSipEventReceiverRunning() || m_sipThread)
       {
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"SIP Thread active at close");
          IsSipEventReceiverRunning(false);
          shutdown(SetupSocket(), SHUT_RDWR);
          MrcpUtils::CloseSocket(SetupFd());
          m_sipThread->join();
          SetupFd(0);
       }

	if (IsCommandEventReceiverRunning() || m_commandThread)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Command Thread active at close");
           IsCommandEventReceiverRunning(false);
           shutdown(CommandSocket(), SHUT_RDWR);
           MrcpUtils::CloseSocket(CommandSocket());
           m_commandThread->join();
           CommandSocket(0);
        }
	{ // scope for lock - make sure locked mutex is free?
//             boost::mutex::scoped_lock l_controlLock (m_loggerMutex);
        }
 
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimStart
// Input - None
//         
// Output - success or failure
//        
//   Connects the sever socket and queues the on started event
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::PrimStart()
{
	Name("PrimStart");
	int l_setupSocket;
        int l_setupFd;
        int l_socketStatus;
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " Entering ");
        m_state = SIGNAL_CONNECTING;
      
	l_setupSocket = MrcpUtils::ConnectServerSocket(m_serverAddress,m_serverPort, l_setupFd,m_connectBackoff,l_socketStatus);
        if (l_setupSocket == -1)
           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "SIP socket setup failed");
       
	if ( l_setupSocket == INVALID_SOCKET )
	{
		CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this, " Invalid Signaling Socket - status = ",l_socketStatus);
/*  The goal is to just notify the calling app. None of the socket or thread cleanup is needed
*/
                NotifyObservers(SIGNALING_PROTOCOL_FAILED,this);
		return -1;
	}
        else
        {
		SetupSocket(l_setupSocket);
                SetupFd(l_setupFd);
	}

	if (StartSipListenThread() < 0)
	{
		CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this, "Start Sip Listen Thread Issue ");
		return -1;
	}

	CSignalingOnStartedEvent* l_onStartedTask = new CSignalingOnStartedEvent(this);
	CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_onStartedTask);
    
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimOnSignalingStarted 
// Input - None
//         
// Output - success
//		instantiates a connect command object and queues it
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::PrimOnSignalingStarted()
{
	Name("PrimOnSignalingStarted");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

	CSignalingConnectCommand* l_onConnectTask = new CSignalingConnectCommand(this);
	CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onConnectTask);

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////////////
//
// Description - PrimOnSignalingConnected
// Input - None
//
// Output - success
//              sets state and notifies observers
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::PrimOnSignalingConnected()
{
        Name("PrimOnSignalingConnected");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

        SetStateToConnected();
        
	if(m_commandThread && CommandSocket() > 0)
        {
	   NotifyObservers(SIGNALING_PROTOCOL_CONNECTED,this);
        }
        else
        {
          CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "Command thread not setup ");
//          NotifyObservers(SIGNALING_PROTOCOL_FAILED, this);  ----> Duplicate - PrimSendAck determines if socket to cmd missing
        }     

        return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////////////
//
// Description - PrimOnSignalingFailed
// Input - None
//
// Output - success
//              Cleans up failed sip connect thread
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::PrimOnSignalingFailed()
{
        Name("PrimOnSignalingFailed");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering");


        if (IsSipEventReceiverRunning()|| m_sipThread)
        {
           IsSipEventReceiverRunning(false);
           shutdown(SetupSocket(), SHUT_RDWR);  //close both portions of socket
           MrcpUtils::CloseSocket(SetupFd());
           m_sipThread->join();
           SetupFd(0);
           m_allDone = true;
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Exiting - after sip thread join ");
        }
        
        NotifyObservers(SIGNALING_PROTOCOL_FAILED, this);
//TEST
//        CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(this);
//        CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);

        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimRedirect - will process for redirect processing
// Input - None
//         
// Output - success
//	  cleans up previous SIP obj resources and signals for new connection
//        this is the only instance that reuses the same signal obj
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimRedirect()
{
        Name("PrimRedirect");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering");
        //reset the state for this SIP Obj
        //Cleanup socket and thread for redirected origin connection
        if( m_sipThread )
        {
           IsSipEventReceiverRunning(false);
           shutdown(SetupSocket(), SHUT_RDWR);
           MrcpUtils::CloseSocket(SetupFd());
           m_sipThread->join();
           SetupFd(0);
        }
    
        //now establish connection to redirected destination
        ServerAddress(m_redirectServerAddress);
        m_session->ServerAddress(m_redirectServerAddress);
        ServerPort(m_redirectServerPort);
        m_session->ServerPort(m_redirectServerPort);
        MrcpV2RefLib::CSignalingStartCommand* l_startCommand = NULL;
        l_startCommand = new CSignalingStartCommand(this);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_startCommand);

}

//////////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimConnect - will queue a SIP invite command for processing
// Input - None
//         
// Output - success
//			instantiates a SIP invite command and queues it
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimConnect()
{
	Name("PrimConnect");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

        m_callIDVal.erase();
        m_callFromVal.erase();
        m_callBranchVal.erase();
	long l_callID = (rand());
        std::string l_setupSocket = MrcpUtils::itos(SetupSocket());
	std::string l_errorInfo = "Sip Signal PrimConnect CALLID : " + MrcpUtils::itos(l_callID);
    	CLogger::Instance()->Log( LOG_LEVEL_INFO, *this, l_errorInfo);

	m_callIDVal = MrcpUtils::itos(l_callID) + "F2145@";
	m_callFromVal = MrcpUtils::itos(l_callID) + "214522";
	m_callBranchVal = "z9hG4bk" + MrcpUtils::itos(l_callID);

	bool l_allResourcesRequested = false;
	if (m_session->TtsResource() && m_session->AsrResource())
		l_allResourcesRequested = true;
	char l_serverPort[6];
	MrcpUtils::itoa(m_serverPort, l_serverPort, 10);

	std::string l_src1 = "INVITE sip:mresources@"+ m_serverAddress +":"+ l_serverPort +" SIP/2.0" M_ENDL;
	l_src1 += "via: SIP/2.0/TCP "+ m_clientAddress +":"+ l_setupSocket +";branch=" + m_callBranchVal + "" M_ENDL;
	l_src1 += "Max-Forwards: 6" M_ENDL;
	l_src1 += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverPort +">" M_ENDL;
	l_src1 += "From: MrcpClient <sip:MrcpClient@"+m_clientAddress+":" + l_setupSocket +">;tag=" + m_callFromVal +""  M_ENDL;

	l_src1 += "Contact: <sip:MrcpClient@" + m_clientAddress +":" + l_setupSocket + ";transport=TCP>" M_ENDL;  
	l_src1 += "Call-ID: "+ m_callIDVal + m_clientAddress + M_ENDL;
	l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + " INVITE" M_ENDL;
	l_src1 += "Content-Type: application/sdp" M_ENDL;
	l_src1 += "Content-Length: ";
	std::string l_src2 = "v=0" M_ENDL; 
	l_src2 += "o=- 33580 337654 IN IP4 " + m_clientAddress + M_ENDL; //need 2 numeric values before IN for valid SDP recommend time values
	l_src2 += "s= openMrcpSip"  M_ENDL;
	l_src2 += "c=IN IP4 " + m_clientAddress + M_ENDL;
	l_src2 += "m=application 9 TCP/MRCPv2" M_ENDL;
	l_src2 += "a=setup:active" M_ENDL;
	l_src2 += "a=connection:new" M_ENDL; //first call should be a new session
	if(m_session->AsrResource())
        {
	    l_src2 += "a=resource:speechrecog" M_ENDL; //need to determine values here based on resources
            m_prevInviteResource = "speechrecog";
        }
	else if (m_session->TtsResource())
        {
	    l_src2 += "a=resource:speechsynth" M_ENDL;
            m_prevInviteResource = "speechsynth";
        }
	l_src2 += "a=cmid:1" M_ENDL;  //should be command ID
	if (l_allResourcesRequested)
	{
		l_src2 += "m=application 9 TCP/MRCPv2" M_ENDL;
		l_src2 += "a=setup:active" M_ENDL;
		l_src2 += "a=connection:new" M_ENDL; //first call should be a new session
		if (m_session->TtsResource())
			l_src2 += "a=resource:speechsynth" M_ENDL;
		l_src2 += "a=cmid:1" M_ENDL;  //should be command ID
	}
//	l_src2 += "m=audio " + MrcpUtils::itos(m_clientRtpPort) +" RTP/AVP 0 96" M_ENDL;
        if(m_session->SessionCodec() == "pcmu/8000")
        {
	    l_src2 += "m=audio " + MrcpUtils::itos(m_clientRtpPort) +" RTP/AVP 0 96" M_ENDL;
            l_src2 += "a=rtpmap:0 "+ m_session->SessionCodec() + M_ENDL;
        }
        else if(m_session->SessionCodec() == "pcma/8000")
        {
	    l_src2 += "m=audio " + MrcpUtils::itos(m_clientRtpPort) +" RTP/AVP 8 96" M_ENDL;
            l_src2 += "a=rtpmap:8 "+ m_session->SessionCodec() + M_ENDL;
        }
	l_src2 += "a=rtpmap:96 telephone-event/8000" M_ENDL;    //caution, re-invite has telephony events as rtpmap:100
	if (l_allResourcesRequested)
        {
		l_src2 += "a=sendrecv" M_ENDL;
                m_prevInviteSockDirect = "sendrecv";  //should never re-invite off this
        } 
	else if(m_session->AsrResource())
        {
		l_src2 += "a=sendonly" M_ENDL;
                m_prevInviteSockDirect = "sendonly";
        }
	else if (m_session->TtsResource())
        {
		l_src2 += "a=recvonly" M_ENDL;
                m_prevInviteSockDirect = "recvonly";
        }
	l_src2 += "a=mid:1" M_ENDL;   //message ID

	int l_srcMessageLength = l_src2.length(); // it's the length of the message body that matters
	std::string l_src = l_src1 + MrcpUtils::itos(l_srcMessageLength) + M_ENDL + M_ENDL + l_src2;

	   CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
	   l_sendCommand->MessageBuffer(l_src);
	   CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "Invite queued - ",m_callIDVal);
 
        l_src.erase();
        l_src1.erase();
        l_src2.erase();
        m_inviteSeq = m_cSeq;
//	++m_cSeq;
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimSendAck
// Input - None
//         
// Output - success
//          instantiates a on Ack Message object and queues it
//           goal is to send ack regardless
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimSendAck()
{	
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
        std::string l_setupSocket = MrcpUtils::itos(SetupSocket());
        char l_serverSignalPort[6];
        MrcpUtils::itoa(m_serverPort, l_serverSignalPort, 10);
        long l_callID = (rand());
        m_state = SIGNAL_ACK;
        m_session->SignalCommFailed(false);
        m_session->SignalUnsolictBye(false);

        m_callBranchVal = "z9hG4bk" + MrcpUtils::itos(l_callID);

	std::string Astr = "ACK sip:mrcpserver@"+m_serverAddress +":" + l_serverSignalPort + " SIP/2.0" M_ENDL;
	Astr += "via: SIP/2.0/TCP "+ m_clientAddress +":"+ l_setupSocket +";branch=" + m_callBranchVal + "" M_ENDL;
        Astr += "contact: <sip:MrcpClient@" + m_clientAddress + ";transport=TCP>" M_ENDL;
//				Astr += m_ackToValue;
	Astr += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverSignalPort +">;tag" + m_ackToTag + "" M_ENDL;
	Astr += "From: MrcpClient <sip:MrcpClient@"+m_clientAddress+":"+ l_setupSocket +">;tag=" + m_callFromVal +""  M_ENDL;
	Astr += "Call-ID: "+ m_callIDVal + m_clientAddress + M_ENDL;
        Astr += "CSeq: "+ MrcpUtils::itos(m_cSeq) +" ACK" M_ENDL;
	Astr += "Max-Forwards: 6" M_ENDL;
	Astr += "Content-Length: 0" M_ENDL M_ENDL;
				//Astr += m_ackToValue;
				//Astr += "user-Agent: openMrcpSip/0.0.1 (omiT)" M_ENDL M_ENDL;

// needs a scrub - goal is to send the ack command if we have a valid socket which we always should have

        if(SetupFd() && IsSipEventReceiverRunning())
        {
	   CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
	   l_sendCommand->MessageBuffer(Astr);
 	   CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
// 	   CMrcpTaskProcessor::Instance(m_signalPtr)->QueueTask( l_sendCommand);
	   CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "Ack queued - ",m_callIDVal);
        }
        else 
           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Ack not queued - socket failed");

//success case first - have not received a 4xx response to SIP invite and not a 300 multiple choice case
        if (!m_sigFail && !m_sigRedirect)
        {
           CSignalingOnAvailableEvent* l_onAvailableTask = new CSignalingOnAvailableEvent(this);
           CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onAvailableTask);
           ++m_cSeq;
        }

// This is the case of the command socket failing to setup - send bye and cleanup thread
        if (!m_commandThread && !m_sigRedirect)
        {
             CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Signal Failure / command socket not setup");
	     CSignalingCommsFailed* l_disconCmd = new CSignalingCommsFailed(this);
             CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_disconCmd);

        }
//        else
        if (m_sigRedirect)
        {
            
	    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "MSGREDIRECT - ");
            CSignalingOnRedirectEvent* l_redirectTask = new CSignalingOnRedirectEvent(this);
            CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_redirectTask);
            m_sigRedirect = false;
        }
// this is the case of receiving a 4xx response to SIP invite
        if(m_sigFail)
        {
             CSignalingOnFailedEvent* l_signalingFailedMessage = new CSignalingOnFailedEvent(this);
             CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_signalingFailedMessage);
        }


	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimStop
// Input - None
//         
// Output - success
//          instantiates a on stopped event object and queues it
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimStop()
{
	Name("PrimStop");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

	IsSipEventReceiverRunning(false);
	IsCommandEventReceiverRunning(false);
	
	CSignalingOnStoppedEvent* l_onStoppedTask = new CSignalingOnStoppedEvent( this);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onStoppedTask);

	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - PrimUnsolicitedByeStop
// Input - None
//
// Output - success
//          instantiates a on unsolicted bye stopped event object and queues it
///////////////////////////////////////////////////////////////////////////
int     CMrcpSipSignal::PrimUnsolicitedByeStop()
{
        Name("PrimUnsolicitedByeStop");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
        int l_pendingCmdId = 0;
        int l_pendingCount = 0;
        std::string l_strId;

        m_session->SignalUnsolictBye(true);
        m_session->ServerDiscntCleanup();

        l_pendingCount = m_session->GetPendingListSize();
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "Pending List Count = ",l_pendingCount);
        if(l_pendingCount)
        {
          while(l_pendingCount)
          {
             l_pendingCmdId = m_session->GetPendingFromList();
             if (l_pendingCmdId)
             {
                l_pendingCount--;
                l_strId = MrcpUtils::itos(l_pendingCmdId);
                std::string l_messageBuf = "Unsolicited Bye not connected ID=" + l_strId + "";
                CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(this);
                l_processResults->MessageBuffer(l_messageBuf);
                CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
             }
          }
        }
        IsSipEventReceiverRunning(false);
        IsCommandEventReceiverRunning(false);

        shutdown(SetupSocket(), SHUT_RDWR);
        MrcpUtils::CloseSocket(SetupFd());
        m_sipThread->join();
        SetupFd(0);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"After SIP THREAD JOIN");

        if (CommandSocket())
        {
          shutdown(CommandSocket(), SHUT_RDWR);
          MrcpUtils::CloseSocket(CommandSocket());
          m_commandThread->join();
          CommandSocket(0);
          m_allDone = true;
          NotifyObservers(SIGNALING_PROTOCOL_UNSOLICT_BYE,this);
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"After cmd THREAD JOIN");
        }

        return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - PrimOnCommsFailed
// Input - None
//
// Output - success
//          instantiated for comms failure
///////////////////////////////////////////////////////////////////////////
int     CMrcpSipSignal::PrimOnCommsFailed()
{
        Name("PrimOnCommsFailed");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
        int l_pendingCmdId = 0;
        int l_pendingCount = 0;
        std::string l_strId;

//        m_session->ServerDiscntCleanup();
        l_pendingCount = m_session->GetPendingListSize();
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Pending command count = ",l_pendingCount);
        if(l_pendingCount)
        {
          while(l_pendingCount)
          {
             l_pendingCmdId = m_session->GetPendingFromList();
             if (l_pendingCmdId)
             {
                l_pendingCount--;
                l_strId = MrcpUtils::itos(l_pendingCmdId);
                std::string l_messageBuf = "Communication lost MRCP server COMPLETE ID=" + l_strId + "";
                CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(this);
                l_processResults->MessageBuffer(l_messageBuf);
                CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
             }
          }
        }
	  NotifyObservers(COMMAND_SOCKET_FAILED,this);
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"COMMAND_SOCKET_FAILED notification");

        m_session->SignalCommFailed(true);

        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - PrimByeOk
// Input - None
//
// Output - success
//          instantiates an OK signal and queues it - unsolicited Bye
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::PrimByeOk()
{
        Name("PrimByeOk");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

        char l_serverSignalPort[6];
        MrcpUtils::itoa(m_serverPort, l_serverSignalPort, 10);
        std::string l_setupSocket = MrcpUtils::itos(SetupSocket());

        std::string Astr = "SIP/2.0 200 OK" M_ENDL;
        Astr += "via: SIP/2.0/TCP "+ m_clientAddress +":"+ l_setupSocket +";branch=" + m_callBranchVal + "" M_ENDL;
        Astr += "contact: <sip:MrcpClient@" + m_clientAddress + ";transport=TCP>" M_ENDL;
//        Astr += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverSignalPort +">;tag" + m_ackToTag + "" M_ENDL;
        Astr += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverSignalPort +">;tag" + m_byeFromTag + "" M_ENDL;
        Astr += "From: MrcpClient <sip:MrcpClient@"+m_clientAddress+":"+ l_setupSocket +">;tag=" + m_callFromVal +""  M_ENDL;
//        Astr += "From: MrcpClient <sip:MrcpClient@"+m_clientAddress+":"+ l_setupSocket +">;tag=" + m_byeFromTag +""  M_ENDL;
        Astr += "Call-ID: "+ m_callIDVal + m_clientAddress + M_ENDL;
        Astr += "CSeq: "+ MrcpUtils::itos(m_cSeq) +" ACK" M_ENDL;
        Astr += "Content-Length: 0" M_ENDL M_ENDL;

        if(SetupFd() && IsSipEventReceiverRunning())
        {
           CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
           l_sendCommand->MessageBuffer(Astr);
           CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
        }
        else
           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "BYE Ok not sent - socket failed");
       
 // now cleanup the unsolicited bye 
       CSignalingUnsolicitedByeStop* l_unsolicitStop = new CSignalingUnsolicitedByeStop(this);
       CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_unsolicitStop);

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimOnStopped
// Input - None
//         
// Output - success
//		closes the sockets but new state transition          
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimOnStopped()
{
	Name("PrimOnStopped");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
	int l_status = 0;
    

        shutdown(SetupSocket(), SHUT_RDWR);
	MrcpUtils::CloseSocket(SetupFd());
	m_sipThread->join();
        SetupFd(0);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"After SIP THREAD JOIN");

        if(m_commandThread)
        {
           shutdown(CommandSocket(), SHUT_RDWR);
	   MrcpUtils::CloseSocket(CommandSocket());
	   m_commandThread->join();
           CommandSocket(0);
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"After 2nd THREAD JOIN");
        }

        m_allDone = true;
	NotifyObservers(SIGNALING_PROTOCOL_DISCONNECTED,this);

	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimDisconnect
// Input - None
//         
// Output - success
//          instantiates a bye command object and queues the command object
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimDisconnect()//
{
	Name("PrimDisconnect");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
        //m_state = SIGNAL_DISCONNECT;

        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, m_ackToTag);
        std::string l_setupSocket = MrcpUtils::itos(SetupSocket());
	char l_serverSignalPort[6];
	MrcpUtils::itoa(m_serverPort, l_serverSignalPort, 10);
        long l_callID = (rand());
        m_callBranchVal = "z9hG4bk" + MrcpUtils::itos(l_callID);

	std::string Astr = "BYE sip:mrcpserver@" + m_serverAddress  + ":" + l_serverSignalPort + " SIP/2.0" M_ENDL;
	Astr += "Via: SIP/2.0/TCP "+ m_clientAddress +":"+ l_setupSocket +";branch=" + m_callBranchVal + "" M_ENDL;
	Astr += "Contact: <sip:MrcpClient@" + m_clientAddress + ";transport=TCP>" M_ENDL;
	Astr += "Max-Forwards: 60" M_ENDL;
			//        Astr += m_ackToValue;
	Astr += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverSignalPort +">;tag" + m_ackToTag + "" M_ENDL;
	Astr += "From: MrcpClient <sip:MrcpClient@"+ m_clientAddress +":" + l_setupSocket + ">;tag=" + m_callFromVal + "" M_ENDL;
	Astr += "Call-ID: "+ m_callIDVal  + m_clientAddress + M_ENDL;
	Astr += "CSeq: "+ MrcpUtils::itos(m_cSeq) + " BYE" M_ENDL;
	Astr += "Content-Length: 0" M_ENDL M_ENDL;

	if(SetupFd() && IsSipEventReceiverRunning())
        {
           CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
	   l_sendCommand->MessageBuffer(Astr);
	   CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
        }
        else
           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "Bye not sent - socket disconnected");
//TEST
//        CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(this);
//        CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);


	//reset the commands for the signaling object
	m_cSeq = 100;
	m_callIDVal.erase();
	m_clientAddress.erase();
	m_serverAddress.erase();
	m_serverPort = 0;
        m_state = SIGNAL_DISCONNECT;

	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimReinvite
// Input - resource type to add
//         
// Output - success
//          instantiates a reinvite command object and queues it
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimReinvite(MrcpResourceType a_resourceToAdd)
{
	Name("PrimReinvite");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

	char l_serverSignalPort[6]; 
	MrcpUtils::itoa(m_serverPort,l_serverSignalPort,10);
        std::string l_setupSocket = MrcpUtils::itos(SetupSocket());
	char l_serverPort[6];
	MrcpUtils::itoa(m_serverPort, l_serverPort, 10);
	
	std::string l_src1 = "INVITE sip:mresources@"+ m_serverAddress +":"+l_serverSignalPort +" SIP/2.0" M_ENDL;
	l_src1 += "via: SIP/2.0/TCP "+ m_clientAddress +":"+ l_setupSocket +";branch=" + m_callBranchVal + "" M_ENDL;
	l_src1 += "Max-Forwards: 6" M_ENDL;
	l_src1 += "To: MediaServer <sip:mresources@"+ m_serverAddress +":" + l_serverPort +">" M_ENDL;
	l_src1 += "From: MrcpClient <sip:MrcpClient@"+m_clientAddress+":" + l_setupSocket +">;tag=" + m_callFromVal +""  M_ENDL;

	l_src1 += "Contact: <sip:MrcpClient@" + m_clientAddress + ";transport=TCP>" M_ENDL;
	l_src1 += "Call-ID: "+ m_callIDVal + m_clientAddress + M_ENDL;
//	l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + " INVITE" M_ENDL;
	l_src1 += "CSeq: "+ MrcpUtils::itos(m_inviteSeq) + " INVITE" M_ENDL;
	l_src1 += "Content-Type: application/sdp" M_ENDL;
	l_src1 += "Content-Length: ";
	std::string l_src2 = "v=0" M_ENDL; 
	l_src2 += "o=- 33580 337654 IN IP4 " + m_clientAddress + M_ENDL; //need 2 numeric values before IN for valid SDP recommend time values
	l_src2 += "s= openMrcpSip"  M_ENDL;
	l_src2 += "m=application 9 TCP/MRCPv2 " M_ENDL;
	l_src2 += "c=IN IP4 " + m_clientAddress + M_ENDL;
	l_src2 += "a=setup:active" M_ENDL;
	l_src2 += "a=connection:existing" M_ENDL;
        l_src2 += "a=resource:" + m_prevInviteResource +  M_ENDL;

	l_src2 += "a=cmid:1" M_ENDL;  //should be command ID
	l_src2 += "m=audio " + MrcpUtils::itos(m_clientRtpPort) +" RTP/AVP 0 100" M_ENDL;
	l_src2 += "c=IN IP4 " + m_clientAddress + M_ENDL;
        if(m_session->SessionCodec() == "pcmu/8000")
            l_src2 += "a=rtpmap:0 "+ m_session->SessionCodec() + M_ENDL;
        else if(m_session->SessionCodec() == "pcma/8000")
            l_src2 += "a=rtpmap:8 "+ m_session->SessionCodec() + M_ENDL;
        l_src2 += "a=" + m_prevInviteSockDirect + M_ENDL; //has to match what was set in the invite
	l_src2 += "a=mid:1" M_ENDL;   //message ID
        l_src2 += "m=application 9 TCP/MRCPv2 " M_ENDL;
	l_src2 += "c=IN IP4 " + m_clientAddress + M_ENDL;
        l_src2 += "a=setup:active" M_ENDL;
        l_src2 += "a=connection:existing" M_ENDL;
        if (a_resourceToAdd == speechrecog)
                l_src2 += "a=resource:speechrecog" M_ENDL; //need to determine values here based on resources
        if (a_resourceToAdd == speechsynth)
                l_src2 += "a=resource:speechsynth" M_ENDL;
        l_src2 += "a=cmid:1" M_ENDL;  //should be command ID

	int l_srcMessageLength = l_src2.length(); // it's the length of the message body that matters
	std::string l_src = l_src1 + MrcpUtils::itos(l_srcMessageLength) + M_ENDL + M_ENDL + l_src2;

//	std::cout << l_src.c_str() <<std::endl;

	CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
	l_sendCommand->MessageBuffer(l_src);
	CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);

	++m_cSeq;
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimSendCommand
// Input - reference to message string
//         
// Output - success or failure
//          sends the command down the tcp socket
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimSendCommand(const std::string& a_message, std::string a_messageType)//
{
	Name("PrimSendCommand");
        int l_status = 0;
	CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Entering" );
	SOCKET l_mrcpSocket = CommandSocket();
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,a_message);

        if(l_mrcpSocket)
        {
	   int l_rc;
	   {
		boost::mutex::scoped_lock lock( m_controlMutex);	
		l_rc = MrcpUtils::TcpSend( l_mrcpSocket, a_message.c_str(), a_message.size(),l_status);
	   } 

	   if (CRV_ok != l_rc)
	   {
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  "TcpSend - Mrcp command connection failed status =",l_status);
                CSignalingCommsFailed* l_commsFailed = new CSignalingCommsFailed(this);
                CMrcpTaskProcessor::Instance()->QueueTask(l_commsFailed);
	
                return -1;
	   }
        }
        else
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Sending on 0 socket Msg = ",a_message);


	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - PrimSendSignal
// Input - reference to message string
//
// Output - success or failure
//          sends the command down the tcp socket
///////////////////////////////////////////////////////////////////////////
int     CMrcpSipSignal::PrimSendSignal(const std::string& a_message)//
{
        Name("PrimSendSignal");
	CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Entering" );
        SOCKET l_mrcpSocket = SetupFd();
        int l_rc;
        int l_status = 0;
       
/* 
        struct timespec l_rtnStart, l_rtnStop;
        clock_gettime(CLOCK_REALTIME, &l_rtnStart);
        long sec, msec ;
*/
        if (l_mrcpSocket)
        {
           {
                boost::mutex::scoped_lock lock( m_controlMutex);
                l_rc = MrcpUtils::TcpSend( l_mrcpSocket, a_message.c_str(), a_message.size(),l_status);
                std::string::size_type l_statPos2 = a_message.find("INVITE");
                if (l_statPos2 != std::string::npos)
                   m_state = SIGNAL_INVITE;
           }
/*                clock_gettime(CLOCK_REALTIME, &l_rtnStop);
                sec = l_rtnStop.tv_sec - l_rtnStart.tv_sec ;
                msec= (sec * 1000) + l_rtnStop.tv_nsec / 1000000 ;
                msec -= l_rtnStart.tv_nsec / 1000000 ;
                std::string l_message = "SENT SIP MSG = " + MrcpUtils::itos(msec) + " msec";
                CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,l_message);
*/
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this, " SIP - Sent ",a_message);

           if (l_rc == CRV_failure)
           {
                CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,"TcpSend - Mrcp signaling connection failed status = ",l_status);

                CSignalingOnFailedEvent* l_signalingFailedMessage = new CSignalingOnFailedEvent(this);
                CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_signalingFailedMessage);

                return -1;
           }
       }

        return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimProcessResults
// Input - reference to message string
//         
// Output - success
//          Processes the result packet received on the socket
///////////////////////////////////////////////////////////////////////////
int	CMrcpSipSignal::PrimProcessResults(const std::string& a_message)//
{
	Name("PrimProcessResults");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,   " Entering ");

	MrcpCallBackInfo l_callBackInfo;
	std::string::size_type l_statPos;
	std::string::size_type l_statPos2;
	std::string::size_type l_statPos3;
	std::string::size_type l_endPos;
	std::string l_val;
	std::string l_statusValue;
	std::string l_sequenceNum;
        std::string l_serverAddress;
        std::string l_tmpString;

	int l_strLoc1 = 0;
	int l_statusVal = 0;
	int l_sequenceID = 0;
	bool l_commandComplete = false;

	MRCP_RESULT_STRUCT l_callResults;


      memset(&l_callResults, 0, sizeof(MRCP_RESULT_STRUCT));

      //first set the state
      if(a_message.find(" COMPLETE") != std::string::npos)
      {
            if(a_message.find("speechrecog") != std::string::npos)
                 NotifyObservers(RECOG_COMMAND_COMPLETE,this);
            else
                 NotifyObservers(SYNTH_COMMAND_COMPLETE,this);
            l_commandComplete = true;
      }

     //now find the reference number and status code.
      if ((l_statPos = a_message.find("MRCP/")) != std::string::npos)
      {
           //this is a valid MRCP message
          if ((l_endPos = a_message.find(" ",l_statPos + 1)) != std::string::npos)
          {
           //skip the length field, 
                if ((l_statPos = a_message.find(" ",l_endPos + 1)) != std::string::npos)
                if ((l_endPos = a_message.find(" ",l_statPos + 1)) != std::string::npos)
                {
                     l_val.assign(a_message.substr(l_statPos+1, l_endPos-l_statPos-1));
					 if(!MrcpUtils::isNumeric((char*)l_val.c_str(),l_val.length()))
                     {
                         if ((l_statPos = a_message.find(" ",l_endPos + 1)) != std::string::npos)
                         {
                              l_val.assign(a_message.substr(l_endPos+1, l_statPos-l_endPos-1));
                              l_sequenceID = atoi(l_val.c_str());
                          }
                           //get the completion-cause
                          if ((l_statPos = a_message.find("Completion-Cause:")) != std::string::npos)
                          {
                              l_val.assign(a_message.substr(l_statPos+ 18,3));
                              l_statusVal = atoi(l_val.c_str());
                           }
                      }
                      else
                      {
                         //this is a response, current field is reference number, next is status-code
                         l_sequenceID = atoi(l_val.c_str());
                         if ((l_statPos = a_message.find(" ",l_endPos + 1)) != std::string::npos)
                         {
                               l_val.assign(a_message.substr(l_endPos + 1, l_statPos - l_endPos - 1));
                               l_statusVal = atoi(l_val.c_str());
                          }
                      }
		}
           }
//	   strncpy(l_callResults.MrcpReturnPkt,a_message.c_str(),a_message.length()+1);
// the folowing is for Loquendo:
       std::string::size_type l_recCmp = a_message.find("RECOGNITION-COMPLETE");
           int l_confLev, l_pktIndx;
           std::string l_message = a_message;
           l_pktIndx = l_message.find("<?xml");
        if (l_message.size() && l_recCmp != std::string::npos && l_pktIndx != std::string::npos)
         {
              l_pktIndx = l_message.find("<?xml");
              std::string l_resultTxt;
              l_resultTxt = l_message.substr(l_pktIndx);
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_resultTxt = ",l_resultTxt);

              int l_responseVal = 0;
              std::string::size_type l_pktContent = l_message.size() - l_pktIndx;
              std::string::size_type l_lfPos = 0; // Must initialize
              std::string l_reformatStr;
              l_reformatStr.erase();
              //the following removes the cr in loquendos response as needed by VXI
                 while ( ( l_lfPos = l_resultTxt.find ("\n",l_lfPos) ) != std::string::npos )
                 {
                      l_resultTxt.replace ( l_lfPos, 1," " );
                      l_responseVal++;
                 }
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_resultTxt2 = ",l_resultTxt);
                if (l_responseVal)
                {
                   l_pktIndx = l_message.find("<?xml");
                   int vwx,cde,stu;
                   //the following removes "encoding=utf8" which causes issues in VXI
                   stu = l_resultTxt.find("encoding=");
                   if (stu != std::string::npos)
                     l_resultTxt.replace(stu, 16,"");

                   vwx = l_resultTxt.find("grammar=");
                   l_resultTxt.insert(vwx+9,"session:");
                   cde = l_resultTxt.find("grammar=",vwx+15);
                   l_resultTxt.insert(cde+9,"session:");
//                   l_tmpString.replace(l_pktIndx, l_message.size(),l_resultTxt);
                   l_message.replace(l_pktIndx, l_message.size(),l_resultTxt);
                   l_reformatStr = l_message;
             int l_cntLength = l_reformatStr.find("Content-Length:");
             int l_newCap = l_reformatStr.size();
             if (l_cntLength != std::string::npos)
             {
                int l_eol = l_reformatStr.find(M_ENDL,l_cntLength);
                l_cntLength+=16;
                int l_capDigs = l_eol - l_cntLength;
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_cntSize = ",l_cntLength);
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_eol = ",l_eol);
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "content digits = ",l_capDigs);
                char l_newVal[4];
                MrcpUtils::itoa(l_newCap, l_newVal, 10);
                l_reformatStr.replace (l_cntLength, l_capDigs, l_newVal);
             }
              strncpy(l_callResults.MrcpReturnPkt,l_reformatStr.c_str(),l_newCap);
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "MRCP Return PKT = ",l_callResults.MrcpReturnPkt);
         }
       }
      }
      else 
      if ((l_statPos2 = a_message.find("Unsolicited",0)) != std::string::npos)
      {
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Unsolicted Bye FOUND");
          if ((l_statPos3 = a_message.find("=",l_statPos2)) != std::string::npos)
          {
              l_val.assign(a_message.substr(l_statPos3+1,  a_message.size()-l_statPos3+1));
              l_sequenceID = atoi(l_val.c_str());
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Unsolicted Bye - sequence id = ",l_sequenceID);
	      strncpy(l_callResults.MrcpReturnPkt,"Unsolicited Bye not connected ",31);
          }
      }
      else
      if ((l_statPos2 = a_message.find("Communication lost")) != std::string::npos)
      {
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Comm lost message = ",a_message);
          if ((l_statPos3 = a_message.find("=")) != std::string::npos)
          {
              l_val.assign(a_message.substr(l_statPos3+1, a_message.size()-l_statPos3+1));
              l_sequenceID = atoi(l_val.c_str());
              CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this,"Communication lost - sequence id = ",l_sequenceID);
	      strncpy(l_callResults.MrcpReturnPkt,"Communication lost MRCP server ",32);
              m_session->SignalCommFailed(true);

          }
      }
        if (l_sequenceID)
          m_session->RemovePendingFromList(l_sequenceID);

	if(!strlen(l_callResults.MrcpReturnPkt))
           strncpy(l_callResults.MrcpReturnPkt,a_message.c_str(),a_message.length()+1);
        l_callResults.MrcpReturnValue = l_statusVal;
	l_callResults.referenceNumber = l_sequenceID;
	l_callResults.asrSessionHandle = AsrSessionHandle();
	l_callResults.ttsSessionHandle = TtsSessionHandle();
        l_serverAddress = ServerAddress();
        if (!l_serverAddress.empty())
        {
           strncpy(l_callResults.MrcpSrvrAddress, l_serverAddress.c_str(),l_serverAddress.size()+1);
        }
        else
           strncpy(l_callResults.MrcpSrvrAddress, "\n",1);
//	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " server address ",l_callResults.MrcpSrvrAddress);

	int l_callBackStat = m_cmdIdMgr->GetCommandObj(l_sequenceID,l_callBackInfo);
	if (l_callBackStat != MrcpSuccess)
	{
		l_callBackInfo.callbackFunction = m_defaultCallBackFunction;
		l_callBackInfo.callbackHandle = m_defaultCallBackHandle;
                CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, " GetCommandObj ERROR ",l_sequenceID);
	}
	m_cmdIdMgr->PerformCallbackProcessing(l_callResults,l_commandComplete,l_callBackInfo);
	
	return MrcpSuccess;
}


////////////////////////////////////////////////////////////////////////// 
// Description - StartSocketListenThread - start thread to process socket events
// Input - None
//         
// Output - success or failure
//          starts a thread to receive command status messages from server
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::StartSipListenThread()
{			
	Name("StartSipListenThread");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      if ( IsSipEventReceiverRunning())
      {
	  	 CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this,  " Already running ");
         return -1;
      }
   }

   m_sipThread = boost::shared_ptr<boost::thread>(new boost::thread( boost::bind( &CMrcpSipSignal::ProcessSipThreadEvents, this)));	
   if (!m_sipThread)
	{	    
		CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  " Thread not started ");
		return -1;
	}
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
// Description - ProcessSipThreadEvents - Listen on socket for events
// Input - None
//         
// Output - success
//         Thread to collect results to commands sent to MRCP server 
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::ProcessSipThreadEvents()
{
//	Name("ProcessSipThreadEvents");

	fd_set	l_check_set;
        fd_set  l_error_set;
	struct timeval l_time_interval = { 0, 5000 } ;
	int l_packetsReady = 0; 
//	char l_buff[10000], l_orig_buff[10000];
        char l_buff[5000], l_orig_buff[5000];
	std::string l_bufferReceived;
	std::string::size_type l_statPos;
        int l_signalCnt = 0;
        int sigCode;

   boost::mutex::scoped_lock semaphoreLock( m_semaphoreMutex);

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      m_sipEventReceiverRunning = true;
   }
try
{
   while( IsSipEventReceiverRunning())
   {
	 while (!l_packetsReady && IsSipEventReceiverRunning()) 
         {  
	    FD_ZERO( &l_check_set );
	    FD_ZERO( &l_error_set );
            FD_SET(SetupFd(), &l_check_set);
            FD_SET(SetupFd(), &l_error_set);
            l_time_interval.tv_usec = 5000;
	    l_packetsReady = select ( FD_SETSIZE, &l_check_set, NULL, NULL, &l_time_interval ) ;
            if (l_packetsReady < 0)
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SIP select returned -1",l_packetsReady);
	 }
	 if (l_packetsReady > 0 && IsSipEventReceiverRunning())
	 {
	     int len = MrcpUtils::UdpGet ( SetupFd() , l_buff, sizeof ( l_buff )); //used because it deals with long packets

             if (len > 1)
             {
	       l_bufferReceived.assign( l_buff, len); 
//	       l_bufferReceived.assign( l_buff, sizeof(l_buff)); 
     	       if ( l_statPos = (l_bufferReceived.find("SIP")) != std::string::npos) 
	       {
                  CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "GOT A SIP",l_bufferReceived);
	   	  CSipProcessMessage* l_onSipMessage = new CSipProcessMessage( this);
		  l_onSipMessage->SetMessageBuffer(l_bufferReceived);
		  CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onSipMessage);
		  l_bufferReceived.erase();
		  strncpy(l_buff,"\n",1);
	          l_statPos = 0;
	       }
               else
               if (IsSipEventReceiverRunning())
               {
                 CSignalingOnFailedEvent* l_signalFailed = new CSignalingOnFailedEvent(this);
                 CMrcpTaskProcessor::Instance()->QueueTask(l_signalFailed);
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SIP socket FAIL");
               }
            }
	    l_packetsReady = 0;
         }

   }
}
catch(...)
{
  CLogger::Instance()->Log(LOG_LEVEL_ERROR,*this,"EXCEPTION Signaling thread");
}
   SetupFd(0);
   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " SIP Event thread exited ");

   return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
// Description - SetupCommandProcessing - start thread to process socket events
// Input - command socket
//         
// Output - results from starting command listen thread
//          connects a socket to the server , sets the command socket & starts 
//			listen thread
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::SetupCommandProcessing(int a_commandSocket)
{
	Name("SetupCommandProcessing");
       CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

        int lStat = 0;


      struct timespec l_rtnStart, l_rtnStop;
//       long sec, msec ;
//        clock_gettime(CLOCK_REALTIME, &l_rtnStart);

        if (!CommandSocket())
        { 
	   int l_cmdSocket=MrcpUtils::ConnectServerSocket(m_serverAddress,a_commandSocket,m_serverFd,m_connectBackoff,lStat);
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Command Socket Connect sent");
           if ( l_cmdSocket != INVALID_SOCKET )
           {
	      CommandSocket(m_serverFd);
              if(!IsCommandEventReceiverRunning())
	        StartCommandListenThread();
 /*             clock_gettime(CLOCK_REALTIME, &l_rtnStop);
                sec = l_rtnStop.tv_sec - l_rtnStart.tv_sec ;
                msec= (sec * 1000) + l_rtnStop.tv_nsec / 1000000 ;
                msec -= l_rtnStart.tv_nsec / 1000000 ;
                std::string l_message = "COMMAND SOCKET HOOKED UP = " + MrcpUtils::itos(msec) + " msec";
                CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,l_message);
*/
           }
           else
           {
              CLogger::Instance()->Log(LOG_LEVEL_ERROR, *this, "MRCP Command socket not setup");
              CommandSocket(0);    
           }
           CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Exiting ");
        }

        return lStat;
}
////////////////////////////////////////////////////////////////////////// 
// Description - StartCommandListenThread - start thread to process socket events
// Input - None
//         
// Output - success
//          starts a thread to process commands received
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::StartCommandListenThread()
{	
	Name("StartCommandListenThread");
	CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      if ( IsCommandEventReceiverRunning())
      {
         CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this,  " Command Listen Thread already running ");
         return -1;
      }
   }

   m_commandThread = boost::shared_ptr<boost::thread> ( new boost::thread( boost::bind( &CMrcpSipSignal::ProcessCommandThreadEvents, this)));
	if (!m_commandThread)
	{
	    CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  " Error creating thread ");
	    return -1;
	}
	
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
// Description - ProcessCommandThreadEvents - Listen on socket for events
// Input - None
//         
// Output - success
//        process sip events received on socket  
///////////////////////////////////////////////////////////////////////////
int CMrcpSipSignal::ProcessCommandThreadEvents()
{

	fd_set	l_check_set;
        fd_set  l_error_set;
	struct timeval l_time_interval = { 0, 5000 } ;
	int l_packetsReady = 0; 
	char l_buff[10000]="";
	std::string l_bufferReceived;

   boost::mutex::scoped_lock semaphoreLock( m_semaphore2Mutex);
//   memset(l_buff,0,sizeof(l_buff));

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_commandMutex);
      m_commandEventReceiverRunning = true;
   }

try
{
   while( IsCommandEventReceiverRunning())
   {
      while (!l_packetsReady && IsCommandEventReceiverRunning()) 
      {  
	 FD_ZERO( &l_check_set );
         FD_ZERO( &l_error_set );
	 FD_SET(CommandSocket(), &l_check_set);
         FD_SET(CommandSocket(), &l_error_set);
         l_time_interval.tv_usec = 5000;
	 l_packetsReady = select ( FD_SETSIZE, &l_check_set, NULL, NULL, &l_time_interval ) ;
      }
      int l_buf_len = sizeof ( l_buff ) - 1 ;
      int l_len = MrcpUtils::TcpGet(CommandSocket(), l_buff, l_buf_len);
      if (CRV_ok ==l_len)
      {
         CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, l_buff );
	 CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(this);
	 l_processResults->MessageBuffer( l_buff);  
	 CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
	 memset(l_buff,0,sizeof(l_buff));
      }
      else
      if (IsCommandEventReceiverRunning())
      {
          CSignalingCommsFailed* l_commsFailed = new CSignalingCommsFailed(this);
          CMrcpTaskProcessor::Instance()->QueueTask(l_commsFailed);
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "COMMAND socket FAIL");
      }
   }
}
catch (...)
{
   CLogger::Instance()->Log(LOG_LEVEL_ERROR, *this, "EXCEPTION command loop");
}
       CommandSocket(0);
       return MrcpSuccess;
}	

} //end namespace


