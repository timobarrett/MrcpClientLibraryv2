///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpGeneric.cpp
//
//	Description:	CMrcpGeneric class implementation
//                Implements primitives for get and set parameters
//				  defined as generic commands in the MRCP SPecification
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpGeneric::CMrcpGeneric(RESOURCE_CFG_STRUCT* a_resourceCfg,AMrcpSignaling* a_signalObj)
//		CMrcpGeneric::~CMrcpGeneric()
//		int	CMrcpGeneric::PrimGetParams(CGetParams& a_task)
//		int	CMrcpGeneric::PrimSetParams(CSetParams& a_task)
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpGeneric.h"
#include "CGenericCommands.h"
#include "CMrcpCmdIdMgr.h"
#include "MrcpUtils.h"
#include "AMrcpSignaling.h"
#include "ClientInterfaceDefs.h"
#include "CMrcpSendCommand.h"
#include "CMrcpTaskProcessor.h"
#include "AMrcpResourceState.h"
#include "MrcpClientEvents.h"
#include "CMrcpEvent.h"

namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - config structure
//         signaling object
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpGeneric::CMrcpGeneric(RESOURCE_CFG_STRUCT* a_resourceCfg,AMrcpSignaling* a_signalObj)
	: m_resourceCfg (a_resourceCfg)
{ 
	m_signalObj = a_signalObj;
	BasicInitialization();
	ClassName("CMrcpGeneric");
	m_cmdIdMgr = MrcpV2RefLib::CMrcpCmdIdMgr::Instance();
	m_genericCmds = new CGenericCommands(m_mrcpVersion);

        m_signalObj->RegisterForNotification(this, SIGNALING_DISCONNECTED);
        m_signalObj->RegisterForNotification(this, RECOG_COMMAND_COMPLETE);
        m_signalObj->RegisterForNotification(this, SYNTH_COMMAND_COMPLETE);

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpGeneric::~CMrcpGeneric()
{
	delete m_genericCmds;
        m_signalObj->Unregister(this, SIGNALING_DISCONNECTED);
        m_signalObj->Unregister(this, SYNTH_COMMAND_COMPLETE);
        m_signalObj->Unregister(this, RECOG_COMMAND_COMPLETE);

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimGetParams
// Input - task object
//         
// Output - Success or failure
//          
///////////////////////////////////////////////////////////////////////////
int	CMrcpGeneric::PrimGetParams(CGetParams& a_task)
{
	Name("CMrcpGeneric");
	std::string l_src;
	std::string l_params;
        std::string l_tmpParam;
	std::string l_comChannelRecID;
        std::string l_comChannelSynID;
        std::ostringstream tmpStrValue;
	int i;
        int lSequence = 0;
        char* tmpParams = NULL;

	if(a_task.m_version == 1)
		l_src = m_genericCmds->get_params1;
	else
		l_src = m_genericCmds->get_params2;

        l_comChannelRecID.erase();	
        l_comChannelSynID.erase();	
	l_comChannelRecID = m_signalObj->ComRecogChannelID();
	l_comChannelSynID = m_signalObj->ComSynthChannelID();

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(3)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + l_comChannelRecID + l_src.substr (l_channelId + 3);

        std::string foo = "src after channel_id assigned" + l_src;
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, foo);

	if(a_task.m_parameters.numParams)
	{
	   l_params = m_genericCmds->get_params_arguments2;
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, l_params);
           tmpParams = strtok(a_task.m_parameters.parameterName," ");
           if(tmpParams != NULL)
           { 
	      lSequence = l_params.find("(1)");
	      l_params = l_params.substr (0,lSequence) + tmpParams + l_params.substr (lSequence + 3);
              while (tmpParams != NULL)
              {
                 tmpParams = strtok(NULL, " ");
                 if(tmpParams != NULL)
                 {
                    tmpStrValue << tmpParams;// << std::endl;
                    l_params += tmpStrValue.str()+ ":" + MRCP_ENDL;
                    tmpStrValue.clear();
                 }
              }
              l_src += l_params;
           }
	}
// The following supports wildcard?
        l_src += MRCP_ENDL;
	int l_sequence1 = l_src.find ("(1)");
	if (l_sequence1 != std::string::npos)
	{
		int l_packetLength = l_src.size() - 1;
		l_src = l_src.substr(0,l_sequence1) + MrcpUtils::itos(l_packetLength) + l_src.substr (l_sequence1 + 3);
	}

        std::string foo2 = "src after channel_id assigned" + l_src;
        CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, foo2);

	if(m_signalObj)
	{
		CMrcpSendCommand* l_sendCommand = new CMrcpSendCommand(m_signalObj);
		l_sendCommand->MessageBuffer(l_src);
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - PrimSetParams
// Input - task object
//         
// Output - success or failure
//          
///////////////////////////////////////////////////////////////////////////
int	CMrcpGeneric::PrimSetParams(CSetParams& a_task)
{
	Name ("PrimSetParams");
	std::string l_src;
	std::string l_params;
	std::string l_comChannelID;
	std::string l_comChannelRecID;
        std::string l_comChannelSynID;
	int i;

        l_comChannelRecID.erase();	
        l_comChannelSynID.erase();	
	l_comChannelRecID = m_signalObj->ComRecogChannelID();
	l_comChannelSynID = m_signalObj->ComSynthChannelID();
	
        if(a_task.m_version == 1)
		l_src = m_genericCmds->set_params1;
	else
	{
                l_src = m_genericCmds->set_params2;
	}

	l_comChannelID = m_signalObj->ComSynthChannelID();
	if (l_comChannelID.empty())
		l_comChannelID = m_signalObj->ComRecogChannelID();

	int l_sequence2 = l_src.find("(2)");
	l_src = l_src.substr (0,l_sequence2) + MrcpUtils::itos (a_task.m_cmdID ) + l_src.substr (l_sequence2 + 3);

	int l_channelId = l_src.find("(3)");
	if (l_channelId != std::string::npos)
		l_src = l_src.substr(0,l_channelId) + l_comChannelRecID + l_src.substr (l_channelId + 3);

	if(a_task.m_parameters.numParams)
	{
		for (i=0; i < a_task.m_parameters.numParams; i++)
		{
			l_params = m_genericCmds->set_params_arguments2;
			int l_sequence = l_params.find("(1)");
			l_params = l_params.substr (0,l_sequence) + a_task.m_parameters.parameterName[i] + l_params.substr (l_sequence + 3);
			int l_sequence2 = l_params.find("(2)");
			l_params = l_params.substr (0,l_sequence2) + a_task.m_parameters.parameterValue[i] + l_params.substr (l_sequence2 + 3);
			l_src += l_params;
			l_params.erase();
		}
		l_src += MRCP_ENDL;
	}

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
		l_sendCommand->ResourceType("reco");
		CMrcpTaskProcessor::Instance()->QueueTask( l_sendCommand);
		return MrcpSuccess;
	}
	return -1;
}

} //end namespace

