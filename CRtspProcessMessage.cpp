///////////////////////////////////////////////////////////////////////////////
//      File Name:              CRtspProcessMessage.cpp
//
//      Description:    CMrcpProcessRtspResults class implementation
//                                      Process the MRCPv1 responses received
//
//      Declared in this file:
//              none
//
//      Implemented in this file:
//              CRtspProcessMessage::CRtspProcessMessage
///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CRtspProcessMessage.h"
#include "CMrcpRtspSignal.h"
//#include "CMrcpSendCommand.h"
#include "CMrcpTaskProcessor.h"
#include "MrcpUtils.h"
#include "AMrcpSignalingState.h"
#include "CMrcpSession.h"
#include <time.h>

#include <iostream>

namespace MrcpV2RefLib
{
//////////////////////////////////////////////////////////////////////////
//
// Description - Constructor
// Input - pointer to signaling object
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////

CRtspProcessMessage::CRtspProcessMessage(CMrcpRtspSignal* a_signalObj)
                : MrcpTasks( a_signalObj)
{
    ClassName("CRtspProcessMessage");
    m_signalObj = a_signalObj;
    SignalPtr(MrcpUtils::itos(int(m_signalObj)));
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Destructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
CRtspProcessMessage::~CRtspProcessMessage()
{

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Execute
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
void CRtspProcessMessage::Execute()
{
        Name("Execute");
        int l_sessionPos;
        int l_sessEndPos;
        int l_audioPort;
        int l_callStat;
        std::string l_sessionID;
        std::string l_toAudioPort;
       

        //test for response to rtsp setup
//        std::string::size_type l_statPos = m_messageBuffer.find("sdp");
        std::string::size_type l_statPos = m_messageBuffer.find("Transport");
        if (l_statPos != std::string::npos)
        {
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP response message path");
           //get the session ID returned
	   std::string::size_type l_sessionPos = m_messageBuffer.find("Session:");
           if (l_sessionPos != std::string::npos)
           {
	      std::string::size_type l_sessEndPos = m_messageBuffer.find(M_ENDL, l_sessionPos);
              l_sessionPos += 9;
              l_sessionID.assign(m_messageBuffer.substr(l_sessionPos, (l_sessEndPos - l_sessionPos)));
              m_signalObj->SessionId(l_sessionID);
              CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP session ID = ",l_sessionID);
           }
           else
             CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "RTSP response Malformed - Session ID not found");
           std::string::size_type l_audioPortPos = m_messageBuffer.find("server_port=", 0);
           //get the rtp port identified by server
           if (l_audioPortPos != std::string::npos)
           {
               l_audioPortPos += 12; 
               std::string::size_type l_pos1 = m_messageBuffer.find("-");
               l_toAudioPort.assign(m_messageBuffer.substr(l_audioPortPos, (l_audioPortPos - l_pos1)));
               l_audioPort = atoi(l_toAudioPort.c_str());
               m_signalObj->ServerRtpPort(l_audioPort);
               CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP audio port = ",l_audioPort);
               CSignalingOnConnectedEvent* l_onConnectedTask = new CSignalingOnConnectedEvent(m_signalObj);
               //                    l_onConnectedTask->SetSipResult(l_callStat);
               CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onConnectedTask);
               CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Queued on connected event");
           }
           else
             CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "RTSP response Malformed - server_port not found");

        }
        else
        {
//command results processing
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP non SDP packet received");
           std::string::size_type l_statVal = m_messageBuffer.find("200 OK");
           std::string::size_type l_prgressVal = m_messageBuffer.find("IN-PROGRESS");
 
           CMrcpProcessCommandResults* l_processResults = new CMrcpProcessCommandResults(m_signalObj);
           l_processResults->MessageBuffer(m_messageBuffer);
           CMrcpTaskProcessor::Instance("CALLBACK")->QueueTask( l_processResults);
             
           if(l_statVal != std::string::npos && l_prgressVal == std::string::npos)
           {
              if(!m_signalObj->SessionPtr()->MultiResourceServer() && (m_signalObj->AsrTeardownSent() || m_signalObj->TtsTeardownSent())
              || (m_signalObj->SessionPtr()->MultiResourceServer() && m_signalObj->AsrTeardownSent() && m_signalObj->TtsTeardownSent()))
             {
        //tear the thing down 
                CSignalingOnDisconnectedEvent* l_disconnectedEvent = new CSignalingOnDisconnectedEvent(m_signalObj);
//               l_disconnectedEvent->SetSipResult(l_callStat);
                CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_disconnectedEvent);
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP Disconnect event queued");
                MrcpUtils::MsSleep(1);
                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Queuing RTSP Stop Command");
                CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(m_signalObj);
                CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);
//                CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "RTSP Stop Command queued");
             }
          }
       }        

}

}//end namespace
