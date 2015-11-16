///////////////////////////////////////////////////////////////////////////////
//      File Name:              CMrcpServerMgr.h
//
//      Description:    class implementation
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
///////////////////////////////////////////////////////////////////////////////

#ifndef CMrcpServerMgr_h
#define CMrcpServerMgr_h

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <map>
#include "Singleton.h"
#include "ClientInterfaceDefs.h"
#include "CLogger.h"
#include "CMrcpServer.h"

namespace MrcpV2RefLib
{
        class CMrcpServer;
        class CMrcpServerMgr : public Singleton<CMrcpServerMgr>
        {
           friend class Singleton<CMrcpServerMgr>;

        public:
         virtual ~CMrcpServerMgr();
         int SetCommonCfg(char* a_clientAddress,int a_sockConBckOff,int a_keepAlveInt,int a_keepAlveCnt,int a_mrcpVersion);
  //       CMrcpServer* GetServer(char* a_resourceType);
//         int GetServer(char* a_resourceType,CMrcpServer* a_serverPtr);
         int GetServer(char* a_resourceType, char* a_serverAddr, int* a_serverPort);
         int RegisterServer(char* l_resourceType,char* a_serverAddress,int a_serverPort,MrcpCallBackFunction a_callbackFunc);
         int UnregisterServer(std::string a_resourceType, std::string a_serverAddress);

                int GetMrcpVersion(){return m_mrcpVersion;};
                int GetClientPort(){return m_clientPort;};
                int GetSockConBackoff() {return m_sockConnectBackoff;};
                int GetKeepAliveInt() { return m_keepAliveInter;};
                int GetKeepAliveCnt() { return m_keepAliveCnt;};
                char* GetClientAddr() { return m_clientAddress;};

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
                int GetNumAsrServers(){int l_numServers = m_resourceAsrPool.size(); return l_numServers;};
                int GetNumTtsServers(){int l_numServers = m_resourceTtsPool.size(); return l_numServers;};
                int GetNumDualServers(){int l_numServers = m_resourceBothPool.size(); return l_numServers;};

                LogObjectTypeEnum LogObjectType(){return LogObject_System;};
        protected:

        private:
                CMrcpServerMgr();

                int m_mrcpVersion;
                int m_clientPort;
                int m_sockConnectBackoff;  
                int m_keepAliveInter;
                int m_keepAliveCnt;

		char m_clientAddress[128];
                std::string m_className;
                std::string m_name;
                std::string m_signalPtr;
                boost::mutex m_loggerMutex;
                boost::mutex m_criticalSection;
               
                std::map<std::string, CMrcpServer*> m_resourceAsrPool;
                std::map<std::string, CMrcpServer*> m_resourceTtsPool;
                std::map<std::string, CMrcpServer*> m_resourceBothPool;
                typedef std::map<std::string, CMrcpServer*>::iterator serverIter;
                serverIter m_asrIterator;
                serverIter m_ttsIterator;
                serverIter m_bothIterator;
        };
}//end namespace
#endif
