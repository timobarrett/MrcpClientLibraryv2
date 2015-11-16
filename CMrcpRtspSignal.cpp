///////////////////////////////////////////////////////////////////////////////
//      File Name:              CMrcpRtspSignal.cpp
//
//      Description:    CMrcpRtspSignal class implementation
//                      Part of Rtsp signaling state machine
//      Declared in this file:
//              none
//
//      Implemented in this file:
//        CMrcpRtspSignal::CMrcpRtspSignal (CMrcpSession* a_session);
//        CMrcpRtspSignal::~CMrcpRtspSignal();
//        int CMrcpRtspSignal::PrimStart();
//        int CMrcpRtspSignal::PrimConnect();
//        int CMrcpRtspSignal::PrimOnSignalingStarted();
///////////////////////////////////////////////////////////////////////////////
#include "CMrcpRtspSignal.h"
#include "MrcpClientEvents.h"
#include "CMrcpSession.h"
#include "MrcpUtils.h"
#include "CMrcpSendSignal.h"
#include "CMrcpSendCommand.h"
#include "CMrcpTaskProcessor.h"
#include "CRtspProcessMessage.h"
#include "CMrcpCmdIdMgr.h"

#include <string>

#define M_ENDL "\r\n"

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
CMrcpRtspSignal::CMrcpRtspSignal(CMrcpSession* a_session)
        : m_session (a_session)
        , m_rtspEventReceiverRunning (false)
//      , m_commandEventReceiverRunning (false)
//        , m_state(SIGNAL_UNINITIALIZED)
//        , m_allDone(false)
//        , m_sigRedirect(false)
//        , m_sigFail(false)
        , m_asrTeardownSent(false)
        , m_ttsTeardownSent(false)
        , m_synthCallSeq(0)
        , m_speakSeq(0)
        , m_recogCallSeq(0)
        , m_recognizeSeq(0)
{
        ClassName("CMrcpRtspSignal");
        struct timespec l_sigTime;
        clock_gettime(CLOCK_REALTIME, &l_sigTime);
        SignalPtr(MrcpUtils::itos(int(this)));
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
        //time granularity is in seconds modified my random number seed to:

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
CMrcpRtspSignal::~CMrcpRtspSignal()
{
       if (IsRtspEventReceiverRunning() || m_rtspThread)
       {
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"RTSP Thread active at close");
          IsRtspEventReceiverRunning(false);
          shutdown(SetupSocket(), SHUT_RDWR);
          MrcpUtils::CloseSocket(SetupFd());
          m_rtspThread->join();
//          CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " RTSP Event thread exited ");
          SetupFd(0);
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
int CMrcpRtspSignal::PrimStart()
{
        Name("PrimStart");
        int l_setupSocket;
        int l_setupFd;
        int l_socketStatus;
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " Entering ");
//        m_state = SIGNAL_CONNECTING;

        l_setupSocket = MrcpUtils::ConnectServerSocket(m_serverAddress,m_serverPort, l_setupFd,m_connectBackoff,l_socketStatus);
        if ( l_setupSocket == INVALID_SOCKET )
        {
                CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this, " Invalid Signaling Socket - status = ",l_socketStatus);
                NotifyObservers(SIGNALING_PROTOCOL_FAILED,this);
                return -1;
        }
        else
        {
                SetupSocket(l_setupSocket);
                SetupFd(l_setupFd);
        }

        if (StartRtspListenThread() < 0)
        {
                CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this, "Start Rtsp Listen Thread Issue ");
                return -1;
        }

        CSignalingOnStartedEvent* l_onStartedTask = new CSignalingOnStartedEvent(this);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_onStartedTask);

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
int     CMrcpRtspSignal::PrimStop()
{
        Name("PrimStop");

	IsRtspEventReceiverRunning(false);

        CSignalingOnStoppedEvent* l_onStoppedTask = new CSignalingOnStoppedEvent( this);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onStoppedTask);

        return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - PrimOnStopped
// Input - None
//
// Output - success
//              closes the sockets but new state transition
///////////////////////////////////////////////////////////////////////////
int     CMrcpRtspSignal::PrimOnStopped()
{
        Name("PrimOnStopped");
        int l_status = 0;

        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
        shutdown(SetupFd(), SHUT_RDWR);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "socket shutdown ");
        MrcpUtils::CloseSocket(SetupFd());
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "socket closed");
	      
        m_rtspThread->join();
        SetupFd(0);
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"After RTSP THREAD JOIN");

//        m_allDone = true;
        NotifyObservers(SIGNALING_PROTOCOL_DISCONNECTED,this);

        return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////////////
//
// Description - PrimOnSignalingStarted
// Input - None
//
// Output - success
//              instantiates a connect command object and queues it
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::PrimOnSignalingStarted()
{
        Name("PrimOnSignalingStarted");
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

        CSignalingConnectCommand* l_onConnectTask = new CSignalingConnectCommand(this);
        CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onConnectTask);

        return MrcpSuccess;
}

/* verification of MRCP v1 RTSP commands */
////////////////////////////////////////////////////////////////////////////////
//
// Description - PrimConnect - will queue a RTSP setup command for processing
// Input - None
//
// Output - success
//                     
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::PrimConnect()
{

/* The setup RTSP packet is 
TRANSPORT modes = RECORD recognize and PLAY for synthesis
-------------
SETUP rtsp://10.128.41.12:4900/media/speechrecognizer RTSP/1.0
CSeq: 626527163
Transport: RTP/AVP;Source=dummyHost;multicast;destination=10.128.41.9;client_port=37774-37775;mode=RECORD
Content-Type: application/sdp
Content-Length: 237

v=0
o=- 123 456 IN IP4 salltimo.labs.com
s=SpeechWorks MediaServer version 3.0
p=+1-888-555-1212
c=IN IP4 10.128.41.9
t=0 0
m=audio 37774 RTP/AVP 0 96
a=rtpmap:0 pcmu/8000
a=rtpmap:96 telephone-event/8000
a=fmtp:96 0-15 
*/
        Name("PrimConnect");

      std::string l_mediaType;
      std::string l_mediaMode;
      CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
//for multiresource opens - open asr here and tts via a reinvite
      if(m_session->AsrResource())
      {
          l_mediaType = "speechrecognizer" ; //need to determine values here based on resources
          l_mediaMode = "RECORD";
      }
      else if (m_session->TtsResource())
      {
          l_mediaType = "speechsynthesizer";
      }

  
      std::string l_src1 = "SETUP rtsp://"+ m_serverAddress +":"+ MrcpUtils::itos(m_serverPort) +"/media/" + l_mediaType + " RTSP/1.0" M_ENDL;
      l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + M_ENDL;
//      l_src1 += "Transport: RTP/AVP;Source=IPMS;unicast;destination="+ m_clientAddress +";client_port="+MrcpUtils::itos(m_clientRtpPort)+"-"+MrcpUtils::itos(m_clientRtpPort+1)+";";
      l_src1 += "Transport: RTP/AVP;unicast;client_port="+MrcpUtils::itos(m_clientRtpPort)+"-"+MrcpUtils::itos(m_clientRtpPort+1);
      if(m_session->AsrResource())
         l_src1 += "mode="+ l_mediaMode + M_ENDL;
      else
         l_src1 += M_ENDL;
      l_src1 += "Content-Type: application/sdp" M_ENDL;
      l_src1 += "Content-Length: ";

//break sdp from setup for length
      std::string l_src2 = "v=0" M_ENDL;
      l_src2 += "o=- 214 512 IN IP4" + m_clientAddress + M_ENDL;
      l_src2 += "p=+1-800-754-4444" M_ENDL;
      l_src2 += "c=IN IP4 " + m_clientAddress + M_ENDL;
      l_src2 += "t=0 0" M_ENDL;
      l_src2 += "m=audio " + MrcpUtils::itos(m_clientRtpPort) + " RTP/AVP 0 96" M_ENDL;
      if(m_session->SessionCodec() == "pcmu/8000")
         l_src2 += "a=rtpmap:0 "+ m_session->SessionCodec() + M_ENDL;
      else if(m_session->SessionCodec() == "pcma/8000")
         l_src2 += "a=rtpmap:8 "+ m_session->SessionCodec() + M_ENDL;
      l_src2 += "a=rtpmap:96 telephone-event/8000" M_ENDL;
      l_src2 += "a=fmtp:96 0-15" M_ENDL;

      int l_srcMessageLength = l_src2.length(); // it's the length of the message body that matters
      std::string l_src = l_src1 + MrcpUtils::itos(l_srcMessageLength) + M_ENDL + M_ENDL + l_src2;

      CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
      l_sendCommand->MessageBuffer(l_src);
      PrimSendSignal(l_src);
//      CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
      m_cSeq++;
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "Rtsp Setup queued - ");
}
////////////////////////////////////////////////////////////////////////////////
//
// Description - PrimReinvite - will queue a RTSP setup command for processing
//                              adding resource requested
// 
// Input - Resource type to add to session
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::PrimReinvite(MrcpResourceType a_resourceToAdd)
{
        Name("PrimReinvite");

      std::string l_mediaType;
      std::string l_mediaMode;
      CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
      if (a_resourceToAdd == speechrecog)
      {
          l_mediaType = "speechrecognizer"; //need to determine values here based on resources
          l_mediaMode = "record";
      }
      else if (a_resourceToAdd == speechsynth)
      {
          l_mediaType = "speechsynthesizer";
      }

      std::string l_src1 = "SETUP rtsp://"+ m_serverAddress +":"+ MrcpUtils::itos(m_serverPort) +"/media/" + l_mediaType + " RTSP/1.0" M_ENDL;
      l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + M_ENDL;
//      l_src1 += "Transport: RTP/AVP;Source=IPMS;unicast;destination="+ m_clientAddress +";client_port="+MrcpUtils::itos(m_clientRtpPort)+"-"+MrcpUtils::itos(m_clientRtpPort+1)+";";
      l_src1 += "Transport: RTP/AVP;unicast;client_port="+MrcpUtils::itos(m_clientRtpPort)+"-"+MrcpUtils::itos(m_clientRtpPort+1)+";";
      if(m_session->AsrResource())
         l_src1 += "mode="+ l_mediaMode + M_ENDL;
      else
         l_src1 += M_ENDL;
      l_src1 += "Session: " +  m_sessionID + M_ENDL + M_ENDL; 

      CMrcpSendSignal* l_sendCommand = new CMrcpSendSignal(this);
      l_sendCommand->MessageBuffer(l_src1);
      PrimSendSignal(l_src1);
//      CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
      m_cSeq++;
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "Rtsp Setup queued - ");
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
int CMrcpRtspSignal::PrimOnSignalingConnected()
{
        Name("PrimOnSignalingConnected");

        SetStateToConnected();

        if(m_rtspThread && SetupFd() > 0)
        {
           NotifyObservers(SIGNALING_PROTOCOL_CONNECTED,this);
           CSignalingOnAvailableEvent* l_onAvailableTask = new CSignalingOnAvailableEvent(this);
           CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onAvailableTask);
        }
//        else
//        {
//          CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  "Command thread not setup ");
//          NotifyObservers(SIGNALING_PROTOCOL_FAILED, this);  ----> Duplicate - PrimSendAck determines if socket to cmd missing
//        }

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
int  CMrcpRtspSignal::PrimDisconnect()//
{
/*TEARDOWN rtsp://10.128.41.12:4900/media/speechsynthesizer RTSP/1.0
CSeq: 309346642
Session: EKBCKMAJAAALMBMEAAAAAAAA
*/
        Name("PrimDisconnect");
        //m_state = SIGNAL_DISCONNECT;
       std::string l_mediaType;
       CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");
      if(m_session->AsrResource())
      {
        l_mediaType = "speechrecognizer"; //need to determine values here based on resources
      }
      else if (m_session->TtsResource())
      {
        l_mediaType = "speechsynthesizer";
      }
       int l_seqID = m_cmdIdMgr->GetSynthSeqNum();
       m_lastCseq = l_seqID;
   
       std::string l_src1 = "TEARDOWN rtsp://" +m_serverAddress+":"+MrcpUtils::itos(m_serverPort)+"/media/"+l_mediaType+" RTSP/1.0" M_ENDL; 
       l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + M_ENDL;
       l_src1 += "Session: " + m_sessionID + M_ENDL + M_ENDL;

      CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(this);
      l_sendCommand->MessageBuffer(l_src1);
      PrimSendSignal(l_src1);
//      CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_sendCommand);
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,  "Rtsp Teardown queued - ",m_sessionID);
     if(m_session->AsrResource())
      {
        m_asrTeardownSent = true;
      }
      else if (m_session->TtsResource())
      {
        m_ttsTeardownSent = true;
      }
      if (m_session->AsrResource() && m_session->TtsResource())
      {
        l_mediaType = "speechsynthesizer";

       std::string l_src1 = "TEARDOWN rtsp://" +m_serverAddress+":"+MrcpUtils::itos(m_serverPort)+"/media/"+l_mediaType+" RTSP/1.0" M_ENDL;
       l_src1 += "CSeq: "+ MrcpUtils::itos(m_cSeq) + M_ENDL;
       l_src1 += "Session: " + m_sessionID + M_ENDL + M_ENDL;

       CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(this);
       l_sendCommand->MessageBuffer(l_src1);
       PrimSendSignal(l_src1);
       m_ttsTeardownSent = true;
     }

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
int     CMrcpRtspSignal::PrimSendSignal(const std::string& a_message)//
{
        Name("PrimSendSignal");
        int l_status = 0;
        SOCKET l_mrcpSocket = SetupFd();
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
                CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  "TcpSend - Mrcp connection failed status =",l_status);
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
// Description - PrimProcessResults
// Input - reference to message string
//
// Output - success
//          Processes the result packet received on the socket
///////////////////////////////////////////////////////////////////////////
int     CMrcpRtspSignal::PrimProcessResults(const std::string& a_message)
{
        Name("PrimProcessResults");
        int l_callStat;
        int l_callSeq;
        int l_pktIndx;
        int l_mrcpSeqNo;
        int l_activeSeqNo;
        bool l_commandComplete = false;
        std::string l_serverAddress;
        std::string l_statVal;
        std::string l_tmpString;
        MRCP_RESULT_STRUCT l_callResults;

        MrcpCallBackInfo l_callBackInfo;
        memset(&l_callResults, 0, sizeof(MRCP_RESULT_STRUCT));

        std::string::size_type l_cmdCmplte = a_message.find("-COMPLETE");
        std::string::size_type l_cmdVal = a_message.find(" COMPLETE");  //success case for command
        std::string::size_type l_cmdWip = a_message.find("IN-PROGRESS");
        std::string::size_type l_recCmp = a_message.find("RECOGNITION-COMPLETE");
        std::string::size_type l_spkCmp = a_message.find("SPEAK-COMPLETE");

        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "MESSAGE Received = ",a_message);
//Get the sequence Number - common regardless of the status 
        std::string::size_type l_statPos,l_statPos1, l_endPos, l_endPos2;
        if ((l_statPos = a_message.find("MRCP/")) != std::string::npos)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_statPos =",l_statPos);
           //this is a valid MRCP message
           if(l_spkCmp != std::string::npos)
              l_statPos = l_spkCmp + 15;
           else
           if(l_recCmp != std::string::npos)
              l_statPos = l_recCmp + 21;
           else
              l_statPos +=9;
           if ((l_endPos = a_message.find(" ",l_statPos)) != std::string::npos)
           {
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_endPos =",l_endPos);
                 std::string l_seqNum;
                 l_seqNum.assign(a_message.substr(l_statPos, l_endPos - l_statPos));
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "MRCP# =",l_seqNum);
                 l_mrcpSeqNo = atoi(l_seqNum.c_str());
           }
        }
// " COMPLETE" case processing
        if (l_cmdVal != std::string::npos)
        {
           l_statVal = (a_message.substr((l_cmdVal - 3), 3));
           l_callStat = atoi(l_statVal.c_str());
           if(l_mrcpSeqNo)
              m_session->RemovePendingFromList(l_mrcpSeqNo);
           l_commandComplete = true;
           if (m_session->StopPending())
           {           
              //the following assumes a single pending command
              std::string::size_type l_activePos = a_message.find("Active-Request-Id-List:");
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Active-Request-Id-List COMPLETE path");
              if (l_activePos != std::string::npos)
              {
                 l_activePos +=24;
                 std::string::size_type l_endLine = a_message.find(M_ENDL,l_activePos);
                 std::string l_stopSeq;
                 l_stopSeq.assign(a_message.substr(l_activePos, l_endLine - l_activePos));
                 l_activeSeqNo = atoi(l_stopSeq.c_str());
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Active ID =",l_activeSeqNo);
                 m_session->RemovePendingFromList(l_activeSeqNo);
              }
              m_session->StopPending(false);
             }
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Command Complete status =",l_callStat);
        }
        else
// "COMMAND-COMPLETE" case processing
        if (l_cmdCmplte != std::string::npos)
        {
           std::string::size_type l_cmpCause = a_message.find("Completion-Cause:");
           if (l_cmpCause != std::string::npos)
           {
              l_statVal = a_message.substr((l_cmpCause+18), 3);
              l_callStat = atoi(l_statVal.c_str());
              std::string::size_type l_pos2 = a_message.find(M_ENDL, l_cmpCause);
              l_cmpCause+=18;
              std::string l_cmplete = (a_message.substr(l_cmpCause,(l_pos2 - l_cmpCause)));
              if(l_mrcpSeqNo)
                 m_session->RemovePendingFromList(l_mrcpSeqNo);
              l_commandComplete = true;
           }
        }
       
//use CSeq to id which command response type
           std::string::size_type l_seqPos = a_message.find("CSeq:");
           if (l_seqPos != std::string::npos)
           {
              l_seqPos+=5;
              std::string::size_type l_pos = a_message.find(M_ENDL, l_seqPos);
              std::string l_seqVal = (a_message.substr(l_seqPos,(l_pos - l_seqPos)));
//              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "CSeq found - value =",l_seqVal);
              l_callSeq = atoi(l_seqVal.c_str());
              if(l_cmdCmplte != std::string::npos || l_cmdVal != std::string::npos)
              {
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Complete found CSeq value =",l_seqVal);
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Complete recog CSeq value =",m_recogCallSeq);
                 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Complete synth CSeq value =",m_synthCallSeq);
                 if(l_callSeq == m_recogCallSeq || (l_recCmp != std::string::npos))
                 {
                    NotifyObservers(RECOG_COMMAND_COMPLETE,this);
                    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RECOG_COMPLETE sent");
                 }
                 else if (l_callSeq == m_synthCallSeq || l_spkCmp != std::string::npos)
                 {
                    NotifyObservers(SYNTH_COMMAND_COMPLETE,this);
                    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SYNTH_COMPLETE sent");
                 }
              }
           }
 
        if (l_recCmp != std::string::npos)
        {
           int l_confLev;
           std::string l_message = a_message;
           l_pktIndx = l_message.find("<?xml");

// this loop finds all instances of confidence and converts to decimal equivs.
           l_confLev = l_message.find("confidence=",l_pktIndx);
         while (l_confLev != std::string::npos)
         {
              l_confLev+=12;  //get the first quote
              int l_trailQuote = l_message.find(">",l_confLev);
              l_trailQuote-=1;
              std::string l_conf = (l_message.substr(l_confLev,(l_trailQuote - l_confLev)));
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "CONFIDENCE LEVEL = ",l_conf);
              float foo;
              foo = atof(l_conf.c_str());
              foo/=100;
              std::stringstream tmpFoo;
              tmpFoo << foo;
              std::string foo2 = tmpFoo.str();
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "CONFIDENCE LEVEL = ",foo2);
              int x = l_trailQuote - l_confLev;
              std::string l_message2 = l_message.substr(0,l_confLev) + foo2 + l_message.substr(l_trailQuote + x-2);
              std::string::size_type l_strippedPkt = l_message2.size() - l_recCmp;
              l_tmpString = l_message2.substr(l_recCmp, l_strippedPkt+2);
              l_pktIndx = l_trailQuote;
              l_message = l_message2;
              l_confLev = l_message.find("confidence=",l_pktIndx);
         }
         if (l_message.size())
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
              int l_prvCnt = 0;
              //the following removes the cr in loquendos response as needed by VXI
                 while ( ( l_lfPos = l_resultTxt.find ("\n",l_lfPos) ) != std::string::npos )
                 {
                      l_resultTxt.replace ( l_lfPos, 1," " );
                      l_responseVal++;
                      l_prvCnt = l_lfPos;
                 } 
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "l_resultTxt2 = ",l_resultTxt);
                if (l_responseVal)
                {
                   l_pktIndx = l_tmpString.find("<?xml");
                   int xyz, abc, def, ghi, jkl, mno, pqr,stu,vwx,cde;
                   //the following removes "encoding=utf8" which causes issues in VXI
                   stu = l_resultTxt.find("encoding=");
                   if (stu != std::string::npos)
                     l_resultTxt.replace(stu, 16,"");
                   
                   vwx = l_resultTxt.find("grammar=");
                   l_resultTxt.insert(vwx+9,"session:");
                   cde = l_resultTxt.find("grammar=",vwx+15);
                   l_resultTxt.insert(cde+9,"session:");
                   l_tmpString.replace(l_pktIndx, l_message.size(),l_resultTxt);
                   l_reformatStr = l_tmpString;
                }

              if (l_reformatStr.empty())
                  l_reformatStr = l_tmpString;
              else
                  if (l_reformatStr.find("</result")==std::string::npos)
                     l_reformatStr += " </result>";

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
         else
             strncpy(l_callResults.MrcpReturnPkt, l_tmpString.c_str(), l_tmpString.length()+1);
        }
        else 
           strncpy(l_callResults.MrcpReturnPkt,a_message.c_str(),a_message.length()+1);

        l_callResults.MrcpReturnValue = l_callStat;
        l_callResults.referenceNumber = l_mrcpSeqNo;
        l_callResults.asrSessionHandle = AsrSessionHandle();
        l_callResults.ttsSessionHandle = TtsSessionHandle();
        l_serverAddress = ServerAddress();
        if (!l_serverAddress.empty())
        {
           strncpy(l_callResults.MrcpSrvrAddress, l_serverAddress.c_str(),l_serverAddress.size()+1);
        }
        else
           strncpy(l_callResults.MrcpSrvrAddress, "\n",1);

        int l_callBackStat = CMrcpCmdIdMgr::Instance()->GetCommandObj(l_mrcpSeqNo,l_callBackInfo);
        if (l_callBackStat != MrcpSuccess)
        {
                l_callBackInfo.callbackFunction = m_defaultCallBackFunction;
                l_callBackInfo.callbackHandle = m_defaultCallBackHandle;
                CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, " GetCommandObj ERROR ",l_mrcpSeqNo);
        }
        if (l_cmdWip == std::string::npos)
           m_cmdIdMgr->PerformCallbackProcessing(l_callResults,l_commandComplete,l_callBackInfo);

        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Build announce header for commands
// Input - type of resource
//
// Output - success or failure
//          builds announce header for rtsp commands
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::BuildAnnounceHdr(const std::string a_resourceType,  std::string& a_announceHdr)
{
  int l_seqID;
  a_announceHdr = "ANNOUNCE rtsp//"+m_serverAddress+"/media/";
  if (a_resourceType =="reco")
  {
    a_announceHdr += "speechrecognizer ";
    l_seqID = m_cmdIdMgr->GetRecoSeqNum();
    m_recogCallSeq = l_seqID;
  }
  else if (a_resourceType == "synth")
  {
    a_announceHdr += "speechsynthesizer ";
    l_seqID = m_cmdIdMgr->GetSynthSeqNum();
    m_synthCallSeq = l_seqID;
  }
  a_announceHdr += "RTSP/1.0" M_ENDL;
  a_announceHdr += "CSeq: "+ MrcpUtils::itos(l_seqID) + M_ENDL;
  a_announceHdr += "Content-Type: application/mrcp" M_ENDL;
  a_announceHdr += "Session: " + m_sessionID + M_ENDL;
  a_announceHdr += "Content-Length: ";
 
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
int     CMrcpRtspSignal::PrimSendCommand(const std::string& a_message, std::string a_messageType)
{
        Name("PrimSendCommand");
        int l_status = 0;
        SOCKET l_mrcpSocket = SetupFd();
        std::string l_header;
        std::string l_message;
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,a_message);

        if(l_mrcpSocket)
        {
           BuildAnnounceHdr(a_messageType,l_header);
           if (a_message.find("RECOGNIZE")!=std::string::npos)
              m_recognizeSeq = m_recogCallSeq; 
           if (a_message.find("SPEAK") != std::string::npos)
              m_speakSeq = m_synthCallSeq; 
           l_header += MrcpUtils::itos(a_message.size()) + M_ENDL + M_ENDL;
           l_message = l_header + a_message;
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,l_message);

           int l_rc;
           {
                boost::mutex::scoped_lock lock( m_controlMutex);
                l_rc = MrcpUtils::TcpSend( l_mrcpSocket, l_message.c_str(), l_message.size(),l_status);
           }

           if (CRV_ok != l_rc)
           {
                CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  "TcpSend - Mrcp command connection failed status =",l_status);
//                CSignalingCommsFailed* l_commsFailed = new CSignalingCommsFailed(this);
//                CMrcpTaskProcessor::Instance()->QueueTask(l_commsFailed);

                return -1;
           }
        }
        else
          CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Sending on 0 socket Msg = ",a_message);


        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
// Description - StartRtspListenThread - start thread to process socket events
// Input - None
//
// Output - success or failure
//          starts a thread to receive command status messages from server
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::StartRtspListenThread()
{
   Name("StartRtspListenThread");
   CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this,  " Entering ");

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      if ( IsRtspEventReceiverRunning())
      {
         CLogger::Instance()->Log( LOG_LEVEL_WARNING,*this,  "RTSP Receiver Already running ");
         return -1;
      }
   }

   m_rtspThread = boost::shared_ptr<boost::thread>(new boost::thread( boost::bind( &CMrcpRtspSignal::ProcessRtspThreadEvents, this)));
   if (!m_rtspThread)
   {
         CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  " Thread not started ");
         return -1;
   }
   return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
// Description - ProcessRtspThreadEvents - Listen on socket for events
// Input - None
//
// Output - success
//         Thread to collect results to commands sent to MRCP server
///////////////////////////////////////////////////////////////////////////
int CMrcpRtspSignal::ProcessRtspThreadEvents()
{
        fd_set  l_check_set;
        fd_set  l_error_set;
        struct timeval l_time_interval = { 0, 5000 } ;
        int l_packetsReady = 0;
//      char l_buff[10000], l_orig_buff[10000];
        char l_buff[5000], l_orig_buff[5000];
        std::string l_bufferReceived;
        std::string::size_type l_statPos;
        int l_signalCnt = 0;
        int sigCode;

   boost::mutex::scoped_lock semaphoreLock( m_semaphoreMutex);

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      m_rtspEventReceiverRunning = true;
   }
  try
  {
   while( IsRtspEventReceiverRunning())
   {
      while (!l_packetsReady && IsRtspEventReceiverRunning())
      {
         FD_ZERO( &l_check_set );
         FD_ZERO( &l_error_set );
         FD_SET(SetupFd(), &l_check_set);
//         FD_SET(SetupSocket(), &l_check_set);
         FD_SET(SetupFd(), &l_error_set);
//         FD_SET(SetupSocket(), &l_error_set);
         l_time_interval.tv_sec = 0;
         l_time_interval.tv_usec = 5000;
         l_packetsReady = select ( FD_SETSIZE, &l_check_set, NULL, NULL, &l_time_interval ) ;
//	 CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "AFTER SELECT");
         if (l_packetsReady < 0)
            CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP select returned -1",l_packetsReady);

      }
      int l_buf_len = sizeof ( l_buff ) - 1 ;
      if (l_packetsReady > 0)
      {
         int l_len = MrcpUtils::TcpGet(SetupFd(), l_buff, l_buf_len);
//         int l_len = MrcpUtils::TcpGet(SetupSocket(), l_buff, l_buf_len);
         if (CRV_ok ==l_len)
         {
            CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, l_buff );
            CRtspProcessMessage* l_processMsg = new CRtspProcessMessage(this);
            l_processMsg->SetMessageBuffer( l_buff);
            CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processMsg);
            memset(l_buff,0,sizeof(l_buff));
         }
         else
         if (IsRtspEventReceiverRunning())
         {
             CSignalingCommsFailed* l_commsFailed = new CSignalingCommsFailed(this);
             CMrcpTaskProcessor::Instance()->QueueTask(l_commsFailed);
             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP socket FAIL");
             IsRtspEventReceiverRunning(false);
         }
      }
   }
  }
  catch(...)
  {
    CLogger::Instance()->Log(LOG_LEVEL_ERROR,*this,"EXCEPTION RTSP Signaling thread");
  }
   SetupFd(0);
//   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " RTSP Event thread exited ");

   return MrcpSuccess;

}
} //end namespace

