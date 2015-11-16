///////////////////////////////////////////////////////////////////////////////
//      File Name:              CMrcpSrvrWatcher.h
//
//      Description:     class implementation
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
//
//  
///////////////////////////////////////////////////////////////////////////////
#ifndef CMrcpSrvrWatcher_h
#define CMrcpSrvrWatcher_h

#include <map>
#include <string>
#include "CLogger.h"
#include "Singleton.h"
#include "AMrcpObserverSubject.h"
#include "ClientInterfaceDefs.h"

#include <boost/thread/mutex.hpp>

namespace MrcpV2RefLib
{
   class CWatcherSession;
   class CMrcpSrvrWatcher  :public Singleton <CMrcpSrvrWatcher>
			   ,public AMrcpObserverSubject
   {
    friend class Singleton< CMrcpSrvrWatcher >;
    public:
          virtual ~CMrcpSrvrWatcher();
          int RegisterServer(std::string a_serverAddress, int a_serverPort, int a_keepInterval, int a_keepCnt,int a_backOff,
                             std::string a_resourceType,MrcpCallBackFunction a_callbackFunc);
          int UnregisterServer(std::string a_serverAddress,bool a_failedServer = false);
          int MonitorUpdate(std::string a_serverAddress, std::string a_resourceType);
          int Shutdown();


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
          LogObjectTypeEnum LogObjectType(){return LogObject_System;};

    private:
          CMrcpSrvrWatcher();
         
          typedef struct monStruct {
           int refCnt;
           CWatcherSession* watchObj;
          }; 
          typedef std::map<std::string, monStruct*> SrvrMonitorMap;
          SrvrMonitorMap m_srvrMonMap;
          int m_keepCnt;
          int m_keepIdle;
          int m_keepInterval;

          std::string m_className;
          std::string m_name;
          std::string m_signalPtr;

          boost::mutex m_loggerMutex;
          boost::mutex m_criticalSection;

   };
}

#endif
