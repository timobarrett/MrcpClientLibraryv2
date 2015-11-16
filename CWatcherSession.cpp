
///////////////////////////////////////////////////////////////////////////////
//      File Name:              CWatcherSession.cpp
//
//      Description:    CWatcherSession class implementation
//                      this object watches for 'dead' server connections
//      Declared in this file:
//              none
//
//      Implemented in this file:
//              CWatcherSession();
//              virtual ~CWatcherSession();
//              int WatcherLoop();
//              
///////////////////////////////////////////////////////////////////////////////
//      Revisions:
//      Date            Initial         Description
//      ----            -------         -----------
//      3/10/09         TMB             Initial Version
///////////////////////////////////////////////////////////////////////////////
#include "CWatcherSession.h"
#include "CMrcpSrvrWatcher.h"
#include "CMrcpServerMgr.h"
#include "MrcpUtils.h"
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>

namespace MrcpV2RefLib
{
//////////////////////////////////////////////////////////////////////////
//
// Description - Constructor
// Input - keep alive cnt
//         keep alive interval
//         backoff timer
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
CWatcherSession::CWatcherSession(int a_keepInterval, int a_keepCnt, int a_backoff,std::string a_serverAddress,
      int a_serverPort, std::string a_resourceType,MrcpCallBackFunction a_callbackFunc)
                 :m_serverDown(false)
                 ,m_watcherUpLoopExited(false)
                 ,m_watcherDownLoopExited(false)
                 ,m_exitUpLoop(false)
                 ,m_exitDownLoop(false)
                 ,m_keepAliveInterval(a_keepInterval)
                 ,m_keepAliveCnt(a_keepCnt)
                 ,m_connectBackoff(a_backoff)
                 ,m_serverAddress(a_serverAddress)
                 ,m_serverPort(a_serverPort)
                 ,m_resourceType(a_resourceType)
                 ,m_callbackFunc(a_callbackFunc)
{
     int l_optval = 1;
     ClassName("CWatcherSession");
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Entering");
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Interval", m_keepAliveInterval);
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"count", m_keepAliveCnt);
     CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Backoff", m_connectBackoff);

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Start - starts the watcher session
// Input - nont
//         
// Output - None
//
///////////////////////////////////////////////////////////////////////////
int CWatcherSession::Start()
{
  Name("Start");
  CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Entering");
  CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Keep Alive Interval = ",m_keepAliveInterval);

    m_srvrUpThrd = boost::shared_ptr<boost::thread>(new boost::thread( boost::bind( &CWatcherSession::WatchActiveLoop, this)));
    if (!m_srvrUpThrd)
    {
          CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  "Server Up Thread not started ");
          return -1;
    }

    m_srvrDownThrd = boost::shared_ptr<boost::thread>(new boost::thread( boost::bind( &CWatcherSession::WatchDownLoop, this)));
    if (!m_srvrDownThrd)
    {
          CLogger::Instance()->Log( LOG_LEVEL_ERROR,*this,  "Server Down Thread not started ");
          return -1;
    }

    return 1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Destructor
// Input - None
//
// Output - None
//
///////////////////////////////////////////////////////////////////////////
CWatcherSession::~CWatcherSession()
{
   Name("Destructor");
   int sock_flags;

     if(m_watcherUpLoopExited)
      CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Destructor loop thread has exited");

   if (m_srvrUpThrd && !m_watcherUpLoopExited)
   {
     m_exitUpLoop = true;
     m_watcherUpLoopExited = true;
     shutdown(m_setupFd, SHUT_RDWR);
     m_srvrUpThrd->join();
   }
   if (m_srvrDownThrd && !m_watcherDownLoopExited)
   {
     m_exitDownLoop = true;
     m_watcherDownLoopExited = true;
     shutdown(m_setupFd, SHUT_RDWR);
     m_srvrDownThrd->join();
   }

   else CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Cleaned up and leaving");
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Watcher loop - establishes socket to watched server
//               sets parameters for socket
//               socket sends on m_keepAliveInterval
// Input - None
//
// Output - success - when told to close by CMrcpSrvrWatcher
//          -1 when encountering a socket condition triggering a close 
///////////////////////////////////////////////////////////////////////////
int CWatcherSession::WatchActiveLoop()
{
  CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Entering Active Server loop IP Address =",m_serverAddress);
  CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"Keep Alive Interval = ",m_keepAliveInterval);

  SOCKET l_sock;
  int l_thisAddr;
  int l_res;
  struct sockaddr_in l_remoteAddr;
  struct in_addr l_address;
  int l_socketStatus = 0;
  int l_optval = 1;
  char l_pingData[10] = "IPMS Ping";
  int l_len = 10;
  int l_packetsReady = 0;
  fd_set  l_checkSet;
  struct timeval l_timeInterval = {10, 0 } ;
  this->m_serverDown = false;
  bool l_loggedIt = false;

    sleep(m_keepAliveInterval);
    m_watchSock = MrcpUtils::ConnectServerSocket(m_serverAddress,m_serverPort,m_setupFd,m_connectBackoff,l_socketStatus);

     setsockopt(m_setupFd, SOL_SOCKET, SO_KEEPALIVE, (char *)&l_optval, sizeof(l_optval)) ;
     setsockopt(m_setupFd, SOL_SOCKET, SO_ERROR, (char *)&l_optval, sizeof(l_optval)) ;
     setsockopt(m_setupFd, SOL_TCP, TCP_KEEPCNT, (char *)&m_keepAliveCnt, sizeof(m_keepAliveCnt)) ;
     setsockopt(m_setupFd, SOL_TCP, TCP_KEEPIDLE, (char *)&m_keepAliveInterval, sizeof(m_keepAliveInterval)) ;
     setsockopt(m_setupFd, SOL_TCP, TCP_KEEPINTVL, (char *)&m_keepAliveInterval, sizeof(m_keepAliveInterval)) ;

    if (m_watchSock == -1)
    {
      CLogger::Instance()->Log(LOG_LEVEL_WARNING,*this,"Couldn't connect Mrcp Server for keep alive - ",m_serverAddress);
      m_serverDown = true;
    }
    
    while (!this->m_exitUpLoop)
    {
       FD_ZERO( &l_checkSet );
       FD_SET(m_setupFd, &l_checkSet);
       int l_optval = 1;
       socklen_t l_optlen = sizeof(l_optval);
       struct sockaddr l_remoteSockCon;
       socklen_t   l_sockAddrSize = sizeof(struct sockaddr);
       if (!m_serverDown)
       {
          if(getsockopt(m_setupFd, SOL_SOCKET, SO_KEEPALIVE, &l_optval, &l_optlen) < 0 ||
              (getpeername(m_setupFd, (struct sockaddr *)&l_remoteSockCon, &l_sockAddrSize) < 0))
          {
             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"get peer failed", m_serverAddress);
             m_serverDown = true;
          }

          l_timeInterval.tv_sec = 10;
          l_timeInterval.tv_usec = 0;
       	  l_packetsReady = select ( FD_SETSIZE, &l_checkSet,NULL, NULL,&l_timeInterval);

             if (l_packetsReady < 0)
             {
               CLogger::Instance()->Log(LOG_LEVEL_WARNING, *this,"Error selecting from MRCP server ",m_serverAddress);
               this->m_serverDown = true;
               l_loggedIt = false;
               close(this->m_setupFd);
             }
       }
       else
       {
         if (!this->m_watcherUpLoopExited && !l_loggedIt)
         {
            CLogger::Instance()->Log(LOG_LEVEL_WARNING,*this,"Mrcp Server non responsive - ",m_serverAddress);
            CMrcpSrvrWatcher::Instance()->MonitorUpdate(m_serverAddress, m_resourceType);
            l_loggedIt = true;
         }
       sleep(m_keepAliveInterval);
       }
    }
    CLogger::Instance()->Log(LOG_LEVEL_DEBUG,*this,"watcher loop exited - server = ",m_serverAddress);
    return 1;

}
//////////////////////////////////////////////////////////////////////////
//
// Description - Watcher loop detect server back up
//               sets parameters for socket
//               socket sends on m_keepAliveInterval
// Input - None
//
// Output - success - when told to close by CMrcpSrvrWatcher
//          -1 when encountering a socket condition triggering a close
///////////////////////////////////////////////////////////////////////////
int CWatcherSession::WatchDownLoop()
{
  char l_pingData[10] = "IPMS Ping";
  int l_optval = 1;
  int l_len = 10;
  int l_socketStatus = 0;
  int l_res;
  int l_packetsReady = 0;
  bool l_loggedIt = false;
  struct sockaddr l_remoteSockCon;
  socklen_t   l_sockAddrSize = sizeof(struct sockaddr);

  fd_set  l_checkSet;
  struct timeval l_timeInterval = {10, 0 } ;

  while (!m_exitDownLoop)
  {
    if (m_serverDown)
    {
       if (!l_loggedIt)
       {
          CLogger::Instance()->Log( LOG_LEVEL_INFO,*this, " Entering Down Server loop IP Address =",m_serverAddress);
          l_loggedIt = true;
       }
       sleep(m_keepAliveInterval);
       m_watchSock = MrcpUtils::ConnectServerSocket(m_serverAddress,m_serverPort,m_setupFd,m_connectBackoff,l_socketStatus);
       if (m_watchSock != -1 && !m_exitDownLoop)
       {
          setsockopt(m_setupFd, SOL_SOCKET, SO_KEEPALIVE, (char *)&l_optval, sizeof(l_optval)) ;
          setsockopt(m_setupFd, SOL_SOCKET, SO_ERROR, (char *)&l_optval, sizeof(l_optval)) ;
          setsockopt(m_setupFd, SOL_TCP, TCP_KEEPCNT, (char *)&m_keepAliveCnt, sizeof(m_keepAliveCnt)) ;
          setsockopt(m_setupFd, SOL_TCP, TCP_KEEPIDLE, (char *)&m_keepAliveInterval, sizeof(m_keepAliveInterval)) ;
          setsockopt(m_setupFd, SOL_TCP, TCP_KEEPINTVL, (char *)&m_keepAliveInterval, sizeof(m_keepAliveInterval)) ;

          FD_ZERO( &l_checkSet );
          FD_SET(m_setupFd, &l_checkSet);
          if(getpeername(m_setupFd, (struct sockaddr *)&l_remoteSockCon, &l_sockAddrSize)== 0)
          {
             CLogger::Instance()->Log(LOG_LEVEL_DEBUG, *this,"Server connection has been restored ", m_serverAddress);
             m_serverDown = false;
          }

        }
     }
    sleep(m_keepAliveInterval);
   }

  return 1;
}
}//end namespace
