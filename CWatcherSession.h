///////////////////////////////////////////////////////////////////////////////
//      File Name:              CWatcherSession.h
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
#ifndef CWatcherSession_h
#define CWatcherSession_h
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include "CLogger.h"
#include "ClientInterfaceDefs.h"

typedef int SOCKET;

namespace MrcpV2RefLib
{
   class CWatcherSession  
   {
    public:
          CWatcherSession(int a_keepInterval, int a_keepCnt, int a_backOff, std::string a_serverAddress,int a_serverPort, 
                          std::string a_resourceType,MrcpCallBackFunction a_callbackFunc);
          virtual ~CWatcherSession();
          int Start();
     
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
          int WatchActiveLoop();
          int WatchDownLoop();
          bool m_exitUpLoop;
          bool m_exitDownLoop;
          bool m_serverDown;
          bool m_watcherUpLoopExited;
          bool m_watcherDownLoopExited;
          int m_keepAliveInterval;
          int m_keepAliveCnt;
          int m_connectBackoff;
          std::string m_serverAddress;
          std::string m_resourceType;
          int m_serverPort;
          boost::shared_ptr<boost::thread> m_srvrUpThrd;
          boost::shared_ptr<boost::thread> m_srvrDownThrd;
          SOCKET m_watchSock;
          int m_setupFd;
          fd_set m_checkSet;
          int m_setupFd2;
          std::string m_className;
          std::string m_name;
          std::string m_signalPtr;

          boost::mutex m_loggerMutex;
          MrcpCallBackFunction m_callbackFunc;
   };
}



#endif
