///////////////////////////////////////////////////////////////////////////////
//      File Name:              MrcpServer.cpp
//
//      Description:    MrcpServer class implementation
//                                      Provides server cfg storage function
//
//
//      Declared in this file:
//              none
//
//      Implemented in this file:

///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpServer.h"
#include "CMrcpServerMgr.h"

#define IP_ADDR_SIZE 128

namespace MrcpV2RefLib
{
//////////////////////////////////////////////////////////////////////////
//
// Description - Constructor
// Input - config structure,
//                 bool indicating asr
//                 bool indicating tts
//
// Output - None
//                      Can support asr and tts resource per session
///////////////////////////////////////////////////////////////////////////
CMrcpServer::CMrcpServer(char* a_resourceType, char* a_serverAddress, int a_serverPort, MrcpCallBackFunction a_callbackFunc)
{
     ClassName("CMrcpServer");
     Name("Constructor");

   m_resourceCfg = new RESOURCE_CFG_STRUCT;
   m_serverCfg = new MRCP_SERVER_CONFIG;

   INIT_RESOURCE_CFG_STRUCT(m_resourceCfg);
   INIT_MRCP_SERVER_CONFIG(m_serverCfg);
        //added the following  IPMS

   strncpy(m_serverAddress, a_serverAddress, IP_ADDR_SIZE);
   strncpy(m_clientAddress, CMrcpServerMgr::Instance()->GetClientAddr(),IP_ADDR_SIZE);
   
   m_resourceCfg->clientAddress = CMrcpServerMgr::Instance()->GetClientAddr();
   CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Client Address",m_resourceCfg->clientAddress);
   m_resourceCfg->clientPort = CMrcpServerMgr::Instance()->GetClientPort();
   CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Client Port",m_resourceCfg->clientPort);
   m_resourceCfg->sockConnectBackoff = CMrcpServerMgr::Instance()->GetSockConBackoff();
   CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Backoff",m_resourceCfg->sockConnectBackoff);
   m_resourceCfg->keepAliveInterval = CMrcpServerMgr::Instance()->GetKeepAliveInt();
   CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"KeepAliveInt",m_resourceCfg->keepAliveInterval);
   m_resourceCfg->keepAliveCnt = CMrcpServerMgr::Instance()->GetKeepAliveCnt();
   CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"KeepAliveCnt",m_resourceCfg->keepAliveCnt);
   m_serverCfg->callBackInfo.callbackFunction = a_callbackFunc;
   m_serverCfg->mrcpVersion = CMrcpServerMgr::Instance()->GetMrcpVersion();
   m_serverCfg->serverPort = a_serverPort;
   m_serverCfg->serverAddress = a_serverAddress;
   m_serverCfg->rtpCodec = m_codec;

   if (!strcmp(a_resourceType, "ASR"))
   {
      m_resourceCfg->asrServerConfig = m_serverCfg;
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"ADDED ASR Server");
   }
   else
   if (!strcmp(a_resourceType, "TTS"))
   {
      m_resourceCfg->ttsServerConfig = m_serverCfg;
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"ADDED TTS Server");
   }
   else 
   if (!strcmp(a_resourceType, "BOTH"))
   {
      m_resourceCfg->ttsServerConfig = m_serverCfg;
      m_resourceCfg->asrServerConfig = m_serverCfg;
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"ADDED ASR/TTS Server");
   }

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Destructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
CMrcpServer::~CMrcpServer()
{
        Name("Destructor");
    delete m_resourceCfg;
    delete m_serverCfg;
}

} //end namespace
