///////////////////////////////////////////////////////////////////////////////
//      File Name:              MrcpServer.h
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
#ifndef CMrcpServer_h
#define CMrcpServer_h

#include "ClientInterfaceDefs.h"
#include "CLogger.h"

namespace MrcpV2RefLib
{
   class CMrcpServer
   {
    public:
       CMrcpServer(char* a_resourceType, char* a_serverAddress, int a_serverPort, MrcpCallBackFunction a_callbackFunc);
       virtual ~CMrcpServer(); 
       RESOURCE_CFG_STRUCT* GetResourceCfg(){return m_resourceCfg;};
       char* GetAsrServerAddress() {return m_resourceCfg->asrServerConfig->serverAddress;};
       int GetAsrServerPort() {return m_resourceCfg->asrServerConfig->serverPort;};
       char* GetTtsServerAddress() {return m_resourceCfg->ttsServerConfig->serverAddress;};
       int GetTtsServerPort() {return m_resourceCfg->ttsServerConfig->serverPort;};

       const std::string& Name(){{ // scope for lock
         boost::mutex::scoped_lock l_controlLock (m_loggerMutex);
         return m_name;
       }};
       void Name( const std::string& a_name){ { // scope for lock
         boost::mutex::scoped_lock l_controlLock( m_loggerMutex);
         m_name = a_name;
       }};
       const std::string& ClassName(){return m_className;};
       void ClassName(const std::string& a_className) {m_className = a_className;};
       void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
       const std::string& SignalPtr(){return m_signalPtr;};
       LogObjectTypeEnum LogObjectType(){return LogObject_System;};

    protected:
    private:
       MRCP_SERVER_CONFIG* m_serverCfg;
       RESOURCE_CFG_STRUCT* m_resourceCfg;
       MrcpCallBackInfo m_callbackInfo;
    
       char m_clientAddress[128];
       char m_serverAddress[128];
       char m_codec[10];
 
       boost::mutex m_loggerMutex;
       std::string m_className;
       std::string m_name;
       std::string m_signalPtr;

   };

}//end namespace

#endif

