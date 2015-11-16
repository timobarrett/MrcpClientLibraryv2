///////////////////////////////////////////////////////////////////////////////
//      File Name:              CMrcpSrvrWatcher.cpp
//
//      Description:    CMrcpSrvrWatcher class implementation
//                      this object provides a server registry and watches for 'dead' server connections
//      Declared in this file:
//              none
//
//      Implemented in this file:
//              CMrcpSrvrWatcher();
//              virtual ~CMrcpSrvrWatcher();
//              int RegisterServer(std::string);
//              int Shutdown();
//              int UnregisterServer(std::string);
//              int UpdateServer(std::string); 
///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//
///////////////////////////////////////////////////////////////////////////////
#include "CMrcpSrvrWatcher.h" 
#include "MrcpClientEvents.h"
#include "CWatcherSession.h"
#include "CMrcpServerMgr.h"

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
//                 
///////////////////////////////////////////////////////////////////////////
CMrcpSrvrWatcher::CMrcpSrvrWatcher()
                 :m_signalPtr("")
{
     ClassName("CMrcpSrvrWatcher");

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Destructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
CMrcpSrvrWatcher::~CMrcpSrvrWatcher()
{

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Register IP of connected server
// Input - std::string ip address to monitor
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
int CMrcpSrvrWatcher::RegisterServer(std::string a_serverAddress,int a_serverPort,int a_keepInterval,int a_keepCnt,
                                     int a_backOff, std::string a_resourceType,MrcpCallBackFunction a_callbackFunc)
{
   Name("RegisterServer");
   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Entering ");
   
   if ( a_serverAddress.empty())
   {
       CLogger::Instance()->Log(LOG_LEVEL_ERROR, *this, "server address is empty");
       return -1;
   }

    SrvrMonitorMap::iterator l_monitorMap; 
   { //scope for lock
       boost::mutex::scoped_lock lock( m_criticalSection);
    l_monitorMap = m_srvrMonMap.find(a_serverAddress);
    if (l_monitorMap == m_srvrMonMap.end())
    {
        CWatcherSession* l_watchObj = new CWatcherSession(a_keepInterval, a_keepCnt, a_backOff,a_serverAddress,a_serverPort,
                                          a_resourceType,a_callbackFunc);
  	monStruct* l_monitor = new monStruct;
        l_monitor->refCnt = 1;
        l_monitor->watchObj = l_watchObj;
        l_watchObj->Start();

        m_srvrMonMap.insert(make_pair(a_serverAddress, l_monitor));
    }
    else
    {
        l_monitorMap->second->refCnt++;
    }
   
   }

   return 1;

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Unregister IP of server being monitored if ref count 
//               == 0 then stop monitoring and cleanup thread
// Input - std::string ip address to stop monitoring
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
int CMrcpSrvrWatcher::UnregisterServer(std::string a_serverAddress, bool a_failedServer)
{
   Name("UnregisterServer");
   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Entering ");
   SrvrMonitorMap::iterator l_monitorMap;
   
   { //scope for lock
       boost::mutex::scoped_lock lock( m_criticalSection);
       l_monitorMap = m_srvrMonMap.find(a_serverAddress);
       if (l_monitorMap != m_srvrMonMap.end())
       {
         l_monitorMap->second->refCnt--;
         int foo = l_monitorMap->second->refCnt;
         CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " After decrement ",foo);
         
         if (!l_monitorMap->second->refCnt ||a_failedServer )
         {
            delete(l_monitorMap->second->watchObj);
            delete (l_monitorMap->second);
            m_srvrMonMap.erase(l_monitorMap);
            CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Server Unregistered and deleted");
         }
      }
      else
      {
        CLogger::Instance()->Log(LOG_LEVEL_WARNING,*this,"Unregister called for unregistered server - ",a_serverAddress);
      }

   }
   CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, " Exiting ");
  
   return 1;

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Orderly shutdown of all monitoring of servers
//               
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
int CMrcpSrvrWatcher::Shutdown()
{
   Name("ShutdownMonitoring");
   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Entering ");
   
   { //scope for lock
     boost::mutex::scoped_lock lock( m_criticalSection);
     while(!m_srvrMonMap.empty())
     {
       delete m_srvrMonMap.begin()->second;
       m_srvrMonMap.erase(m_srvrMonMap.begin());
     }
   }

   CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Exiting ");  
   return 1;

}

//////////////////////////////////////////////////////////////////////////
//
// Description - Monitor the MrcpServer for unreachable condition
//               
// Input - std::string ip address to monitor
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
int CMrcpSrvrWatcher::MonitorUpdate(std::string a_serverAddress, std::string a_resourceType)
{
  Name("MonitorUpdate");

    CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Server Unreachable IP Address =",a_serverAddress);
    NotifyObservers(MRCP_SERVER_UNREACHABLE,(void *)a_serverAddress.c_str());  
    CMrcpServerMgr::Instance()->UnregisterServer(a_serverAddress,a_resourceType);    

    return 1;
}

}//end namespace

