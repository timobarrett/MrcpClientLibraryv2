///////////////////////////////////////////////////////////////////////////////
//      File Name:              MrcpServerMgr.cpp
//
//      Description:    MrcpServerMgr class implementation
//                                      Provides server management functions 
//                                      Provides a method to register mrcp servers
//                                      Provides method to unregister mrcp servers
//                                      Provides method to round robin / return mrcp server
//
//
//      Declared in this file:
//              none
//
//      Implemented in this file:
//              CMrcpServerMgr::CMrcpServerMgr()
//              CMrcpServerMgr::~CMrcpServerMgr()
//              CMrcpServerMgr::GetServer()
//              int CMrcpServerMgr::RegisterServer()
//              int CMrcpServerMgr::UnRegisterServer()

///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpServerMgr.h"
#include "CMrcpCallbackTask.h"
#include "CMrcpTaskProcessor.h"
#include "CMrcpServer.h"
#include "CMrcpSrvrWatcher.h"
#define IP_ADDR_SIZE 128

namespace MrcpV2RefLib
{
//////////////////////////////////////////////////////////////////////////
//
// Description - Constructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////

CMrcpServerMgr::CMrcpServerMgr()

{
     ClassName("CMrcpServerMgr");
     Name("Constructor");

     m_asrIterator = m_resourceAsrPool.begin();
     m_ttsIterator = m_resourceTtsPool.begin();
     m_bothIterator = m_resourceBothPool.begin();
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Destructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////

CMrcpServerMgr::~CMrcpServerMgr()
{
     Name("Destructor");
     m_resourceAsrPool.clear();
     m_resourceTtsPool.clear();
     m_resourceBothPool.clear();
}
//////////////////////////////////////////////////////////////////////////
//
// Description - gets server configuration
// Input - Mrcp Request ID
//
// Output - returns server configuation for session
//          
//                     
///////////////////////////////////////////////////////////////////////////
//int CMrcpServerMgr::GetServer(char* a_resourceType,CMrcpServer* a_serverPtr)
int CMrcpServerMgr::GetServer(char* a_resourceType, char* a_serverAddr, int* a_serverPort)
{  
   Name("GetServer");
   CMrcpServer* l_serverPtr;
   RESOURCE_CFG_STRUCT* l_resourceCfg;
 
   { //scope for lock
      boost::mutex::scoped_lock lock( m_criticalSection);
   if (m_ttsIterator == m_resourceTtsPool.end())
   {
     m_ttsIterator = m_resourceTtsPool.begin();
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"TTS Pool reset");
   }
   if (m_asrIterator == m_resourceAsrPool.end())
   {
     m_asrIterator = m_resourceAsrPool.begin();
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"ASR Pool reset");
   }   
   if (m_bothIterator == m_resourceBothPool.end())
   {
     m_bothIterator = m_resourceBothPool.begin();
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Combined Pool reset");
   }
   if (!strcmp(a_resourceType,"ASR")&& m_resourceAsrPool.size())
   {
     l_serverPtr = m_asrIterator->second;
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"ASR Ptr = ",l_serverPtr);
     *a_serverPort = l_serverPtr->GetAsrServerPort();
     strcpy(a_serverAddr, l_serverPtr->GetAsrServerAddress()); 
     m_asrIterator++;
   }
   else
   if(!strcmp(a_resourceType,"TTS")&& m_resourceTtsPool.size())
   {
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"TTS Resource");
     l_serverPtr = m_ttsIterator->second;
     *a_serverPort = l_serverPtr->GetTtsServerPort();
     strcpy(a_serverAddr , l_serverPtr->GetTtsServerAddress()); 
     m_ttsIterator++;
   }
   else
   if ((!strcmp(a_resourceType, "BOTH")&& m_resourceBothPool.size()) ||
      (!strcmp(a_resourceType, "ASR")&& !m_resourceAsrPool.size()) ||
      (!strcmp(a_resourceType, "TTS")&& !m_resourceTtsPool.size())) 
   {
     l_serverPtr = m_bothIterator->second;
//TTS and ASR server information must be the same so took TTS values.
     *a_serverPort = l_serverPtr->GetTtsServerPort();
     strcpy(a_serverAddr, l_serverPtr->GetTtsServerAddress());
     m_bothIterator++;
   }
 
   }//end of lock scope

  // a_resourceCfg = l_serverPtr->GetResourceCfg();
  
   return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description -sets common server configuration information
// Input -
//
// Output -
//     sets common server configuration 
//     NOTE: clientPort and sessionInfo are reserved for future use V2.6
///////////////////////////////////////////////////////////////////////////
int CMrcpServerMgr::SetCommonCfg(char* a_clientAddress,int a_sockConBckOff,int a_keepAlveInt,int a_keepAlveCnt,
                                 int a_mrcpVersion)
{
   Name("SetCommonCfg");
   
        m_clientPort = 0;
        m_mrcpVersion = a_mrcpVersion;
        m_sockConnectBackoff = a_sockConBckOff;
        m_keepAliveInter = a_keepAlveInt;
        m_keepAliveCnt = a_keepAlveCnt;
        strncpy(m_clientAddress, a_clientAddress, IP_ADDR_SIZE);

   return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Registers a server configuration for use
// Input - 
//
// Output - 
//     registers the server configuration for use
//      
///////////////////////////////////////////////////////////////////////////
int CMrcpServerMgr::RegisterServer(char* a_resourceType, char* a_serverAddress, int a_serverPort,
                                   MrcpCallBackFunction a_callbackFunc)
{
   Name("RegisterServer");

   CMrcpServer* l_serverObj = new CMrcpServer(a_resourceType, a_serverAddress, a_serverPort, a_callbackFunc);
    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Server object - ",l_serverObj);
  
   if (!strcmp(a_resourceType, "ASR"))
   {
    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "ASR server to register - ",a_serverAddress);
    { //scope for lock
      boost::mutex::scoped_lock lock( m_criticalSection);
      m_resourceAsrPool.insert(std::make_pair(a_serverAddress,l_serverObj));
    }
    CMrcpSrvrWatcher::Instance()->RegisterServer(a_serverAddress,a_serverPort,m_keepAliveInter,m_keepAliveCnt,
    m_sockConnectBackoff, "ASR", a_callbackFunc);

      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "ASR server registered - ",a_serverAddress);
      int tmpSize = m_resourceAsrPool.size();
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Asr Server Pool size = ", tmpSize);
   }
   else
   if (!strcmp(a_resourceType,"TTS"))
   {
    CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "TTS server to register - ",a_serverAddress);
    { //scope for lock
      boost::mutex::scoped_lock lock( m_criticalSection);
      m_resourceTtsPool.insert(std::make_pair(a_serverAddress,l_serverObj));
    }
    CMrcpSrvrWatcher::Instance()->RegisterServer(a_serverAddress,a_serverPort,m_keepAliveInter,m_keepAliveCnt,
    m_sockConnectBackoff, "Tts",a_callbackFunc);

      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "TTS server registered - ",a_serverAddress);
      int tmpSize = m_resourceTtsPool.size();
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "TTS Server Pool size = ", tmpSize);
   }
   if (!strcmp(a_resourceType,"BOTH"))
   {
    { //scope for lock
      boost::mutex::scoped_lock lock( m_criticalSection);
      m_resourceBothPool.insert(std::make_pair(a_serverAddress,l_serverObj));
    }
    CMrcpSrvrWatcher::Instance()->RegisterServer(a_serverAddress,a_serverPort,m_keepAliveInter,m_keepAliveCnt,
    m_sockConnectBackoff, "BOTH",a_callbackFunc);

      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Both ASR TTS server registered - ",a_serverAddress);
   }

   return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Unregisters a server configuration taking it out of service
// Input -
//
// Output -
//     registers the server configuration for use
//
///////////////////////////////////////////////////////////////////////////
int CMrcpServerMgr::UnregisterServer(std::string a_resourceType, std::string a_serverAddress)
{
   Name("UnregisterServer");

   serverIter l_serverIter;
   CMrcpServer* l_mrcpServer;

   { //scope for lock
     boost::mutex::scoped_lock lock( m_criticalSection);

   if (a_resourceType == "ASR")
   {
      l_serverIter = m_resourceAsrPool.find(a_serverAddress);
      if (l_serverIter != m_resourceAsrPool.end())
      {
           l_mrcpServer = l_serverIter->second;
           delete l_mrcpServer;
           m_resourceAsrPool.erase(l_serverIter);
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "ASR server unregistered - ",a_serverAddress);
      }
   }

   if (a_resourceType == "TTS")
   {
      l_serverIter = m_resourceTtsPool.find(a_serverAddress);
      if (l_serverIter != m_resourceTtsPool.end())
      {
           m_resourceTtsPool.erase(l_serverIter);
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "TTS server unregistered - ",a_serverAddress);
      }
   }
   if (a_resourceType == "BOTH")
   {
      l_serverIter = m_resourceBothPool.find(a_serverAddress);
      if (l_serverIter != m_resourceBothPool.end())
      {
           m_resourceBothPool.erase(l_serverIter);
           CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this, "Both ASR TTS server unregistered - ",a_serverAddress);
      }
   }

   }//end of scope lock

   return MrcpSuccess;
}

}//end namespace
