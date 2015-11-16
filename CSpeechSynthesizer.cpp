///////////////////////////////////////////////////////////////////////////////
//	File Name:		CSpeechSynthesizer.cpp
//
//	Description:	CSpeechSynthesizer class implementation
//				 Implements the primitives for the synthesizer commands
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CSpeechSynthesizer::CSpeechSynthesizer(RESOURCE_CFG_STRUCT* a_resourceCfg, AMrcpSignaling* a_signalObj);
//		CSpeechSynthesizer::~CSpeechSynthesizer();
//		int CSpeechSynthesizer::PrimSpeak(CSpkSpeak& a_task);
//		int CSpeechSynthesizer::PrimPause(CSpkPause& a_task);
//		int CSpeechSynthesizer::PrimResume(CSpkResume& a_task);
//		int CSpeechSynthesizer::PrimBargeIn(CSpkBargeIn& a_task);
//		int CSpeechSynthesizer::PrimStopSpk(CSpkStop& a_task);

///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CSpeechSynthesizer.h"
#include "CSpeakCommands.h"
#include "CMrcpCmdIdMgr.h"
#include "AMrcpSignaling.h"
#include "CMrcpTaskProcessor.h"
#include "CMrcpSendCommand.h"
#include "MrcpClientEvents.h"
#include "CMrcpEvent.h"

#include "MrcpUtils.h"

namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - Resource COnfig Struct
//         singaling object pointer
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CSpeechSynthesizer::CSpeechSynthesizer(RESOURCE_CFG_STRUCT* a_resourceCfg,AMrcpSignaling* a_signalObj)
	: m_resourceCfg (a_resourceCfg)
	, m_resourceType (speechsynth)
	, m_mrcpVersion (m_resourceCfg->ttsServerConfig->mrcpVersion)

{
       ClassName("CSpeechSynthesizer");
	m_signalObj = a_signalObj;
        SignalPtr(MrcpUtils::itos(int(m_signalObj)));
	BasicInitialization();
        m_signalObj->RegisterForNotification(this, SIGNALING_DISCONNECTED);
        m_signalObj->RegisterForNotification(this, SYNTH_COMMAND_COMPLETE);
        m_signalObj->RegisterForNotification(this, RECOG_COMMAND_COMPLETE);

	m_cmdIdMgr = MrcpV2RefLib::CMrcpCmdIdMgr::Instance();
	m_speakCmds = new CSpeakCommands(m_mrcpVersion);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " MRCP Version = ",m_mrcpVersion);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_speakCmds = ",m_speakCmds);
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CSpeechSynthesizer::~CSpeechSynthesizer()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - CleanUp
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
void CSpeechSynthesizer::CleanUp()
{
	delete m_speakCmds;
        m_speakCmds = 0;
        m_signalObj->Unregister(this, SIGNALING_DISCONNECTED);
        m_signalObj->Unregister(this, SYNTH_COMMAND_COMPLETE);
        m_signalObj->Unregister(this, RECOG_COMMAND_COMPLETE);

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimSpeak
// Input - speak task object
//         
// Output - sucess or failure
//   code that is called as task processor calls request from queue                                          
///////////////////////////////////////////////////////////////////////////
int CSpeechSynthesizer::PrimSpeak(CSpkSpeak& a_task)
{
        Name("PrimSpeak");
	std::string l_src;
        std::string l_bargeInStr;
                int m_cmdID;
//              std::string m_killOnBargeIn;
//                int m_killOnBargeIn;
//                std::string m_voiceName;
//              std::string m_speechLanguage;
//                std::string m_contentType;
//                std::string m_content;
//                std::string m_contentID;

        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_speakCmds = ",m_speakCmds);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_speechLanguage = ",a_task.m_speechLanguage);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_contentType = ",a_task.m_contentType);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_content = ",a_task.m_content);
        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " m_contentID = ",a_task.m_contentID);
        
        if (!m_speakCmds)
        {
	  CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"m_spkCmds is ZERO");
          return -1;
        }
        if(m_mrcpVersion == 1)
            l_src = m_speakCmds->Speak_1;
        else
            l_src = m_speakCmds->Speak_2;
        

        
	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(3)");
        if (l_channelId != std::string::npos)
	   l_src = l_src.substr(0,l_channelId) + m_signalObj->ComSynthChannelID() + l_src.substr (l_channelId + 3);

	int l_bargeIn = l_src.find("(4)");
//	if (strcmp(a_task.m_killOnBargeIn.c_str(), "\n"))
//		l_src = l_src.substr(0,l_bargeIn) +"Kill-On-Barge-In: "+ a_task.m_killOnBargeIn + MRCP_ENDL + l_src.substr(l_bargeIn + 3);
        if (l_bargeIn != std::string::npos)
        {
             if (a_task.m_killOnBargeIn == true)
                l_bargeInStr = "true";
             else
                l_bargeInStr = "false";
             l_src = l_src.substr(0,l_bargeIn) +"Kill-On-Barge-In: "+ l_bargeInStr + MRCP_ENDL + l_src.substr(l_bargeIn + 3);
        }

	else
		l_src.erase(l_bargeIn, 3);
	
/*	int l_voiceName = l_src.find("(5)");
	if (strcmp(a_task.m_voiceName.c_str(), "\n"))
		l_src = l_src.substr(0,l_voiceName) +"Voice-Name: " + a_task.m_voiceName + MRCP_ENDL + l_src.substr(l_voiceName + 3);
	else
		l_src.erase(l_voiceName, 3);
*/
	int l_speechLanguage = l_src.find("(6)");
	if (strcmp(a_task.m_speechLanguage.c_str(), "\n"))
	   l_src = l_src.substr(0,l_speechLanguage) +"Speech-Language: "+ a_task.m_speechLanguage + MRCP_ENDL + l_src.substr(l_speechLanguage + 3);
	else
		l_src.erase(l_speechLanguage, 3);

	int l_contType = l_src.find("(7)");
        if (m_mrcpVersion == 1 && a_task.m_contentType == "application/ssml+xml")
           a_task.m_contentType = "application/synthesis+ssml";
	l_src = l_src.substr(0,l_contType) + a_task.m_contentType + l_src.substr (l_contType + 3);

	int l_contLen = l_src.find("(8)");
	l_src = l_src.substr(0,l_contLen) + MrcpUtils::itos(a_task.m_content.size()) + l_src.substr(l_contLen + 3);
//foo
	int l_contLength = l_src.find("(9)");
/*        std::string foo = a_task.m_content;
        int xyz = foo.find(">");
        foo.insert(xyz+1,"\r\n"); 
        int xz = foo.find(">",xyz+1);
        foo.insert(xz+1,"\r\n");
        int ab = foo.find("xml:lang");
        foo.insert(ab,"Voice-");
        foo.replace(ab+6,1,"X"); */
	l_src = l_src.substr(0,l_contLength) + a_task.m_content + l_src.substr(l_contLength + 3);
//	l_src = l_src.substr(0,l_contLength) + foo + l_src.substr(l_contLength + 3);

	//the following goes last
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1!=std::string::npos)
	{
		int l_packetLength = l_src.size();
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
                l_sendCommand->ResourceType("synth");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimPause
// Input - speak pause command object 
//         
// Output - success or failure
//   code that is called as task processor calls request from queue                                      
///////////////////////////////////////////////////////////////////////////
int CSpeechSynthesizer::PrimPause(CSpkPause& a_task)
{
	std::string l_src;
	if(m_mrcpVersion == 1)
		l_src = m_speakCmds->Pause_1;
	else
		l_src = m_speakCmds->Pause_2;

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(3)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + m_signalObj->ComSynthChannelID() + l_src.substr (l_channelId + 3);

	//the following goes last
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1!=std::string::npos)
	{
		int l_packetLength = l_src.size() - 1;
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
                l_sendCommand->ResourceType("synth");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimResume
// Input - speak resume object
//         
// Output - success or failure
//   code that is called as task processor calls request from queue                                      
///////////////////////////////////////////////////////////////////////////
int CSpeechSynthesizer::PrimResume(CSpkResume& a_task)
{
	std::string l_src;
	if(m_mrcpVersion == 1)
		l_src = m_speakCmds->Resume_1;
	else
		l_src = m_speakCmds->Resume_2;

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(3)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + m_signalObj->ComSynthChannelID() + l_src.substr (l_channelId + 3);

	//the following goes last
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1 != std::string::npos)
	{
		int l_packetLength = l_src.size()- 1;
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
                l_sendCommand->ResourceType("synth");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimBargeIn
// Input - barge in task object
//         
// Output - success or failure
//   code that is called as task processor calls request from queue                                       
///////////////////////////////////////////////////////////////////////////
int CSpeechSynthesizer::PrimBargeIn(CSpkBargeIn& a_task)
{
	std::string l_src;
	if(m_mrcpVersion == 1)
	        l_src = m_speakCmds->BargeIn_1;
	else
		l_src = m_speakCmds->BargeIn_2;

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

        int l_proxyId = l_src.find("(3)");
        if (l_proxyId != std::string::npos)
             l_src = l_src.substr(0,l_proxyId) + "603890720" + l_src.substr (l_proxyId + 3);

	int l_channelId = l_src.find("(4)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + m_signalObj->ComSynthChannelID() + l_src.substr (l_channelId + 3);

	//the following goes last
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1 != std::string::npos)
	{
		int l_packetLength = l_src.size() - 1;
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
                l_sendCommand->ResourceType("synth");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimStopSpk
// Input - speak stop task object
//         
// Output - success or failure
//   code that is called as task processor calls request from queue                                          
///////////////////////////////////////////////////////////////////////////
int CSpeechSynthesizer::PrimStopSpk(CSpkStop& a_task)
{
	std::string l_src;
	if(m_mrcpVersion == 1)
		l_src = m_speakCmds->Stop_1;
	else
		l_src = m_speakCmds->Stop_2;

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(4)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + m_signalObj->ComSynthChannelID() + l_src.substr (l_channelId + 3);

	//the following goes last
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1 != std::string::npos)
	{
		int l_packetLength = l_src.size()- 1;
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
                l_sendCommand->ResourceType("synth");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}

	return -1;
}

}//end namespace
