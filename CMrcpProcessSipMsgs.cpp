///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpProcessSipMsgs.cpp
//
//	Description:	CMrcpProcessSipMsgs class implementation
//					Process the SIP responses received
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CSipProcessMessage::CSipProcessMessage
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "CMrcpProcessSipMsgs.h"
#include "CMrcpSipSignal.h"
#include "CMrcpSendCommand.h"
#include "CMrcpTaskProcessor.h"
#include "MrcpUtils.h"
#include "AMrcpSignalingState.h"
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

CSipProcessMessage::CSipProcessMessage(AMrcpSignaling* a_signalObj)
		: MrcpTasks( a_signalObj)
{
    ClassName("CSipProcessMessage");
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
CSipProcessMessage::~CSipProcessMessage()
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
void CSipProcessMessage::Execute()
{
        Name("Execute");
	int l_callStat = 0;
	int l_commandPort = 0;
	int l_audioPort = 0;
        long sec, msec ;
	std::string l_tempChannel;
	std::string l_toAddressValue;
        std::string l_toAddressTag;
	std::string l_toCommandPort;
	std::string l_toAudioPort;
	std::string l_toChannelId;
	std::string l_toChannelId2;
        std::string l_newAddrPort;
        std::string l_newAddr;
        std::string l_newPort;
	std::string::size_type l_cmdPos = std::string::npos;
	std::string::size_type l_pos3 = std::string::npos;
	std::string::size_type l_okPos = std::string::npos;
        struct timespec l_rtnStart, l_rtnStop;

        clock_gettime(CLOCK_REALTIME, &l_rtnStart);

//  TODO - either this or the one in CMrcpSipSignal...
//        CLogger::Instance()->Log( LOG_LEVEL_DEBUG, *this,m_messageBuffer.c_str());
		std::string::size_type l_statPos = m_messageBuffer.find("SIP");
		if (l_statPos != std::string::npos) 
		{
                    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "SIP message path");
			std::string l_statVal = (m_messageBuffer.substr((l_statPos + 8), 3));
			l_callStat = atoi(l_statVal.c_str());
			l_cmdPos = m_messageBuffer.find("BYE");
			if ((200 == l_callStat)&&(l_cmdPos != std::string::npos))
			{	
				l_okPos = m_messageBuffer.find("OK");
				if (l_cmdPos != std::string::npos )//&& l_okPos == std::string::npos)
				{
					CSignalingOnDisconnectedEvent* l_disconnectedEvent = new CSignalingOnDisconnectedEvent(m_signalObj);
                                        l_disconnectedEvent->SetSipResult(l_callStat);
					CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_disconnectedEvent);
					MrcpUtils::MsSleep(1);

					CSignalingStopCommand* l_stopCommand = new CSignalingStopCommand(m_signalObj);
					CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_stopCommand);
				}

			}
                        //this is an unsolicited bye message
                        else if ((l_cmdPos != std::string::npos) && (200 != l_callStat))
                        {
                         std::string::size_type l_pos1 = m_messageBuffer.find("From:");
                          if (l_pos1 != std::string::npos)
                          {
                             std::string::size_type l_pos2 = m_messageBuffer.find(M_ENDL, l_pos1);
                             std::string::size_type l_pos3 = m_messageBuffer.find("tag=", l_pos1);
                             l_pos3+=3;
                             l_toAddressTag.assign(m_messageBuffer.substr(l_pos3,(l_pos2 - l_pos3)));
                          }
                          m_signalObj->SetFromTag(l_toAddressTag);

                           CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this, "UNSOLICITED BYE received from mrcp server ");
                           CSignalingUnsolicitedByeAck* l_unsolicitBye = new CSignalingUnsolicitedByeAck(m_signalObj);                           
                           CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_unsolicitBye);    
                        }
			if ((l_callStat > 199 && l_callStat < 300) && l_cmdPos == std::string::npos) 
			{  //process continues for success only
                            CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Status between 199 and 299 ");
				std::string::size_type l_pos1 = m_messageBuffer.find("To:");
				if (l_pos1 != std::string::npos)
				{
				   std::string::size_type l_pos2 = m_messageBuffer.find(M_ENDL, l_pos1);
				   l_toAddressValue.assign(m_messageBuffer.substr(l_pos1, (l_pos2 - l_pos1)));
				   l_toAddressValue.append(M_ENDL);
                                   std::string::size_type l_pos3 = m_messageBuffer.find("tag=", l_pos1);
                                   l_pos3+=3;
                                   l_toAddressTag.assign(m_messageBuffer.substr(l_pos3,(l_pos2 - l_pos3)));
				} 

				std::string::size_type l_cmdPortPos = m_messageBuffer.find("m=application", 0);
				if (l_cmdPortPos != std::string::npos)
				{
				   l_cmdPortPos += 13;  //add the size of the search string m=application to adjust pointer
				   std::string::size_type l_pos2 = m_messageBuffer.find("TCP/MRCP");
				   l_toCommandPort.assign(m_messageBuffer.substr(l_cmdPortPos, (l_pos2 - l_cmdPortPos)));
				   l_commandPort = atoi(l_toCommandPort.c_str());
				   m_signalObj->SetupCommandProcessing(l_commandPort);
				}
				std::string::size_type l_audioPortPos = m_messageBuffer.find("m=audio", 0);
				if (l_cmdPortPos != std::string::npos)
				{
				   l_audioPortPos += 7;  //add the size of the search string m=application to adjust pointer
				   std::string::size_type l_pos3 = m_messageBuffer.find("RTP/AVP");
				   l_toAudioPort.assign(m_messageBuffer.substr(l_audioPortPos, (l_pos3 - l_audioPortPos)));
				   l_audioPort = atoi(l_toAudioPort.c_str());
				   m_signalObj->ServerRtpPort(l_audioPort);
				}

				std::string::size_type l_chanIdPos = m_messageBuffer.find("a=channel:", 0);
				if (l_chanIdPos != std::string::npos)
				{
				   l_chanIdPos +=10;
				   l_pos3 = m_messageBuffer.find(M_ENDL, l_chanIdPos);
				   l_tempChannel.assign(m_messageBuffer,l_chanIdPos, (l_pos3 - l_chanIdPos));
				   std::string::size_type l_pos4 = l_tempChannel.find("speechrecog");
				   std::string::size_type l_pos5 = l_tempChannel.find("speechsynth");
				   l_toChannelId.assign(m_messageBuffer.substr(l_chanIdPos, (l_pos3 - l_chanIdPos)));
				   if (l_pos4 != std::string::npos)
					m_signalObj->ComRecogChannelID(l_toChannelId);
				   else
				     if (l_pos5 != std::string::npos)
					m_signalObj->ComSynthChannelID(l_toChannelId);
				 }
			        l_tempChannel.erase();
				std::string::size_type l_chanIdPos2 = m_messageBuffer.find("a=channel:", l_pos3);
				if (l_chanIdPos2 != std::string::npos)
				{
				   l_chanIdPos2 +=10;
				   std::string::size_type l_pos6 = m_messageBuffer.find(M_ENDL, l_chanIdPos2);
				   l_tempChannel.assign(m_messageBuffer, l_chanIdPos2, (l_pos6 - l_chanIdPos2));
				   std::string::size_type l_pos7 = l_tempChannel.find("speechsynth");
				   std::string::size_type l_pos8 = l_tempChannel.find("speechrecog");
				   l_toChannelId2.assign(m_messageBuffer.substr(l_chanIdPos2, (l_pos6 - l_chanIdPos2)));
				   if (l_pos7 != std::string::npos)
					m_signalObj->ComSynthChannelID(l_toChannelId2);
				   else
				   if (l_pos8 != std::string::npos)
					m_signalObj->ComRecogChannelID(l_toChannelId2);
				}					
				if(l_cmdPos == std::string::npos)
				{
				   CSignalingAckCommand* l_signalingAckMessage = new CSignalingAckCommand( m_signalObj);
				   m_signalObj->SetAckToValue(l_toAddressValue);
                                   m_signalObj->SetToTag(l_toAddressTag);
                                   m_signalObj->SetSignalFail(false);
				   CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_signalingAckMessage);
                                   CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Queued ACK message");
				   CSignalingOnConnectedEvent* l_onConnectedTask = new CSignalingOnConnectedEvent(m_signalObj);	
                                   l_onConnectedTask->SetSipResult(l_callStat);
				   CMrcpTaskProcessor::Instance("SIP")->QueueTask( l_onConnectedTask);
                                   CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Queued on connected event");
                                  
				}
			}
                        if ((l_callStat == 300) && l_cmdPos == std::string::npos) //300 multiple choice
                        {
                          CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "300 multiple choice received ");
                          CSignalingAckCommand* l_signalingAckMessage = new CSignalingAckCommand(m_signalObj);
			  std::string::size_type l_pos1 = m_messageBuffer.find("To:");
			  if (l_pos1 != std::string::npos)
			  {
			     std::string::size_type l_pos2 = m_messageBuffer.find(M_ENDL, l_pos1);
			     l_toAddressValue.assign(m_messageBuffer.substr(l_pos1, (l_pos2 - l_pos1)));
			     l_toAddressValue.append(M_ENDL);
                             std::string::size_type l_pos3 = m_messageBuffer.find("tag=", l_pos1);
                             l_pos3+=3;
                             l_toAddressTag.assign(m_messageBuffer.substr(l_pos3,(l_pos2 - l_pos3)));
			  } 
		          m_signalObj->SetAckToValue(l_toAddressValue);
                          m_signalObj->SetToTag(l_toAddressTag);
                          m_signalObj->SetSignalFail(false);  // clean up sip signal object to resource manager
                          m_signalObj->SetRedirect(true);
			  CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_signalingAckMessage);

                          std::string::size_type l_contactInfoStart = m_messageBuffer.find("Contact: <",0);
                          if(l_contactInfoStart != std::string::npos)
                          {
			     std::string::size_type l_contactInfoEnd = m_messageBuffer.find(M_ENDL, l_contactInfoStart);
			     l_newAddrPort.assign(m_messageBuffer,l_contactInfoStart+10,((l_contactInfoEnd - 1) - (l_contactInfoStart + 10)));
                             std::string::size_type l_contactAddrStart = l_newAddrPort.find("@",0);
                             std::string::size_type l_contactAddrEnd = l_newAddrPort.find(":",l_contactAddrStart);
                             l_newAddr.assign(l_newAddrPort, l_contactAddrStart + 1, (l_contactAddrEnd - 1) - (l_contactAddrStart));
                             l_newPort.assign(l_newAddrPort, (l_contactAddrEnd + 1), l_contactInfoEnd);
                             m_signalObj->RedirectServerAddress(l_newAddr);
                             CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"REDIRECT SERVER ADDRESS =",l_newAddr);
                             m_signalObj->RedirectServerPort(atoi(l_newPort.c_str()));
     
                             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"300 contact info = ",l_newAddrPort);
                             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"300 contact addr = ",l_newAddr);
                             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"300 contact port = ",l_newPort);
                          }
                        }
			if ((l_callStat > 399) && l_cmdPos == std::string::npos) //400 status message
                        {
                            CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this,"SIP Signal Error - Status = ",l_callStat);
                            //send the ack message to cleanup server
			    CSignalingAckCommand* l_signalingAckMessage = new CSignalingAckCommand( m_signalObj);
			    m_signalObj->SetAckToValue(l_toAddressValue);
                            m_signalObj->SetToTag(l_toAddressTag);
                            m_signalObj->SetSignalFail(true);
			    CMrcpTaskProcessor::Instance("SIP")->QueueTask(l_signalingAckMessage);
                            // the actual cleanup is launched from PrinSendAck
                                
                        } 
		}
                clock_gettime(CLOCK_REALTIME, &l_rtnStop);
                sec = l_rtnStop.tv_sec - l_rtnStart.tv_sec ;
                msec= (sec * 1000) + l_rtnStop.tv_nsec / 1000000 ;
                msec -= l_rtnStart.tv_nsec / 1000000 ;
                std::string l_message = "Process SIP-time = " + MrcpUtils::itos(msec) + " msec";
                CLogger::Instance()->Log( LOG_LEVEL_INFO, *this,l_message);

}

}//end namespace

