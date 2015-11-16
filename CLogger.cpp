///////////////////////////////////////////////////////////////////////////////
// File Name: CLogger.cpp
//
// Description: CLogger class implementation
//
///////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)

#include "CLogger.h"
#include "MrcpUtils.h"

#include <sys/timeb.h>
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <boost/bind.hpp>
#include <boost/format.hpp>

const char* CLogger::LogLevelDesc[] = 
{
   "INVALID",
   "-----",
   "DEBUG",
   "INFO ",
   "WARN ",
   "ERROR"
};

const char* CLogger::LogObjectLevelID[LogObject_MaxTypes] = 
{
   "INVALID",
   "SYSTEM",
   "RESOURCE",
   "SESSION",
   "SESSION_MGR",
   "TASK_PROCESSOR",
   "HANDLE_MGR",
   "CMD_ID_MGR",
   "COMMAND_STATE"
};

///////////////////////////////////////////////////////////////////////////////
// Function: CLogger::CLogger()
//
// Description: Default constructor
//
// Return: 
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
CLogger::CLogger() : m_state( LOGGER_STOPPED), m_logLevel( LOG_LEVEL_ERROR) 
{
}

///////////////////////////////////////////////////////////////////////////////
// Function: CLogger::~CLogger()
//
// Description: Destructor
//
// Return: 
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
CLogger::~CLogger()
{
   Stop();
}

///////////////////////////////////////////////////////////////////////////////
// Function: const char* CLogger::ClassName()
//
// Description: Access the class name
//
// Return: const char* 
//
// Parameters: none 
//
///////////////////////////////////////////////////////////////////////////////
const char* CLogger::ClassName()
{
   return "CLogger";
}

///////////////////////////////////////////////////////////////////////////////
// Function: const char* CLogger::Name()
//
// Description: Access the object name
//
// Return: const char* 
//
// Parameters: none 
//
///////////////////////////////////////////////////////////////////////////////
const char* CLogger::Name()
{
   return "Logger";
}
///////////////////////////////////////////////////////////////////////////////
// Function: const char* CLogger::SignalPtr()
//
// Description: Access the signal obj ptr
//
// Return: const char*
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
const char* CLogger::SignalPtr()
{
   return " ";
}

///////////////////////////////////////////////////////////////////////////////
// Function: bool CLogger::IsRunning()
//
// Description: Test if the Logger is running
//
// Return: bool 
//
// Parameters: none 
//
///////////////////////////////////////////////////////////////////////////////
bool CLogger::IsRunning()
{
   return m_state == LOGGER_RUNNING;
}

///////////////////////////////////////////////////////////////////////////////
// Function: bool CLogger::IsStopped()
//
// Description: Test if the Logger is stopped
//
// Return: bool 
//
// Parameters: none 
//
///////////////////////////////////////////////////////////////////////////////
bool CLogger::IsStopped()
{
   return m_state == LOGGER_STOPPED;
}

///////////////////////////////////////////////////////////////////////////////
// Function: const char* CLogger::StateDesc()
//
// Description: Access a desctiption of the current styate
//
// Return: const char* 
//
// Parameters: none 
//
///////////////////////////////////////////////////////////////////////////////
const char* CLogger::StateDesc() 
{ 
   switch (m_state)
   {
   case LOGGER_STOPPED:
      return "Stopping";
   case LOGGER_STARTING:
      return "Starting";
   case  LOGGER_RUNNING:
      return "Running";
   case  LOGGER_STOPPING:
      return "Stopping";
   }
   return "unknown";
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogLevelEnum CLogger::LogLevel(LogObjectTypeEnum a_objectType)
//
// Description: Access the LogLevel for a given LogObjectType
//
// Return: LogLevelEnum
//
// Parameters: LogObjectTypeEnum a_objectType
//
///////////////////////////////////////////////////////////////////////////////
LogLevelEnum CLogger::LogLevel( LogObjectTypeEnum a_objectType)
{
   return m_logLevelByType[ a_objectType];
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::LogLevel(LogObjectTypeEnum a_objectType, LogLevelEnum a_logLevel)
//
// Description: Set the LogLevel of a given LogObjectType
//
// Return: void
//
// Parameters: LogObjectTypeEnum a_objectType
//             LogLevelEnum a_logLevel
//
///////////////////////////////////////////////////////////////////////////////
void CLogger::LogLevel( LogObjectTypeEnum a_objectType, LogLevelEnum a_logLevel)
{
   if ( a_objectType == LogObject_MaxTypes)
   {
      m_logLevel = a_logLevel;
      for ( int i=0; i<LogObject_MaxTypes; i++)
      {
         m_logLevelByType[i] = a_logLevel;
      }
   }
   else
   {
      m_logLevelByType[ a_objectType] = a_logLevel;
      for ( int i=0; i<LogObject_MaxTypes; i++)
      {
         if ( m_logLevelByType[i] > m_logLevel)
         {
            m_logLevel = m_logLevelByType[i];
         }
      }
   }
}
///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::LogLevel(LogLevelEnum a_logLevel)
//
// Description: Set the LogLevel of the entire library
//
// Return: void
//
// Parameters: LogLevelEnum a_logLevel 
//
///////////////////////////////////////////////////////////////////////////////
void CLogger::LogLevel(LogLevelEnum a_logLevel)
{
   m_logLevel = a_logLevel;
   for ( int i=0; i<LogObject_MaxTypes; i++)
   {
       m_logLevelByType[i] = m_logLevel;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogObjectTypeEnum CLogger::LogObjectTypeFromString(std::string &a_logObjectType)
//
// Description: Determine the LogObjectType from a string
//
// Return: LogObjectTypeEnum
//
// Parameters: std::string &a_logObjectType
//
///////////////////////////////////////////////////////////////////////////////
LogObjectTypeEnum CLogger::LogObjectTypeFromString( std::string& a_logObjectType)
{
   if ( a_logObjectType == "all")
   {
      return LogObject_MaxTypes;
   }
   else if ( a_logObjectType == "system")
   {
      return LogObject_System;
   }
   else if ( a_logObjectType == "resource")
   {
      return LogObject_Resource;
   }
   else if ( a_logObjectType == "session")
   {
      return LogObject_Session;
   }
   else if ( a_logObjectType == "sessionmgr")
   {
      return LogObject_SessionMgr;
   }
   else if ( a_logObjectType == "taskprocessor")
   {
      return LogObject_TaskProcessor;
   }
   else if ( a_logObjectType == "handlemgr")
   {
      return LogObject_HandleMgr;
   }
   else if ( a_logObjectType == "cmdidmgr")
   {
      return LogObject_CmdIdMgr;
   }
   else if ( a_logObjectType == "main")
   {
      return LogObject_Command;
   }
   return LogObject_Invalid;
}

///////////////////////////////////////////////////////////////////////////////
// Function: int CLogger::Start()
//
// Description: Open the log file and start the logger worker thread
//
// Return: int - 0 on success, otherwise failure
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
int CLogger::Start()
{
   boost::mutex::scoped_lock l_lock( m_controlMutex);
   if ( m_state == LOGGER_STOPPED )
   {
      m_state = LOGGER_STARTING;
      m_logfile.open( "/var/snowshore/log/MrcpClientLibrary.log",std::ios_base::app);
      if ( m_logfile.is_open())
      {
         m_thread = boost::shared_ptr<boost::thread> ( new boost::thread( boost::bind( &CLogger::ProcessLogItems, this)));
      }
      else
      {
         return -1;
      }
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Function: int CLogger::Stop()
//
// Description: Flush the logger queue, stop the worker thread and close the 
// log file
//
// Return: int - 0 on success, otherwise failure
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
int CLogger::Stop()
{
   boost::mutex::scoped_lock l_controlLock( m_controlMutex);
   if ( m_state == LOGGER_RUNNING )
   {
      m_state = LOGGER_STOPPING;

      m_semaphore.notify_one();
      m_thread->join();

      boost::mutex::scoped_lock l_lock( m_queueMutex);

      LogItemSptr l_logItem;
      while ( ! m_queue.empty() )
      {
         l_logItem = m_queue.front();
         m_queue.pop();
         if ( ! ! l_logItem )
         {
            OutputLogItem( l_logItem);
         }
      }

      OutputLogItem( FormatedLogItem( LOG_LEVEL_ALWAYS, *this, "Stop", "stopped"));

      if ( m_logfile.is_open())
      {
         m_logfile.close();
      }

      m_state = LOGGER_STOPPED;
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::ProcessLogItems()
//
// Description: Pop log items from the log queue and outpout them
//
// Return: void
//
// Parameters: none
//
///////////////////////////////////////////////////////////////////////////////
void CLogger::ProcessLogItems()
{

   { // scope for lock
      boost::mutex::scoped_lock l_controlLock( m_controlMutex);
      m_state = LOGGER_RUNNING;
   }
 
   OutputLogItem( FormatedLogItem( LOG_LEVEL_ALWAYS, *this, "ProcessLogItems", "started"));

   LogItemSptr l_logItem;

   boost::mutex::scoped_lock l_semaphoreLock( m_semaphoreMutex);

   while( m_state == LOGGER_RUNNING )
   {

      if ( m_queue.empty())
      {
         m_semaphore.wait( l_semaphoreLock);
      }

      {  // scope for lock
         boost::mutex::scoped_lock l_lock( m_queueMutex);
         if ( !m_queue.empty())
         {
            l_logItem = m_queue.front();
            m_queue.pop();
         }
         else
         {
            continue;
         }
      }
      OutputLogItem( l_logItem);

      if ( m_logfile.is_open())
      {
         m_logfile.close();
         m_logfile.open( "/var/snowshore/log/MrcpClientLibrary.log",std::ios_base::app);
      }

   }
 
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::OutputLogItem(const LogItemSptr &a_logItem)
//
// Description: Write a logm item to the console and the log file
//
// Return: void
//
// Parameters: const LogItemSptr &a_logItem
//
///////////////////////////////////////////////////////////////////////////////
void CLogger::OutputLogItem( const LogItemSptr& a_logItem)
{
   m_logfile << a_logItem->str() << std::endl;
//   m_logfile.flush();
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::QueueLogItem(LogItemSptr &a_logItem)
//
// Description: Add a log item to the queue
//
// Return: void
//
// Parameters: LogItemSptr &a_logItem
//
///////////////////////////////////////////////////////////////////////////////
void CLogger::QueueLogItem( LogItemSptr& a_logItem)
{

   {  // scope for lock
      boost::mutex::scoped_lock l_lock( m_queueMutex);
      m_queue.push( a_logItem);
   }
   m_semaphore.notify_one();
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogItemSptr CLogger::BlankMessageHeader(LogLevelEnum a_logLevel)
//
// Description: Create a log message with a blank message header
//
// Return: LogItemSptr
//
// Parameters: LogLevelEnum a_logLevel
//
///////////////////////////////////////////////////////////////////////////////
LogItemSptr CLogger::BlankMessageHeader( LogLevelEnum a_logLevel)
{
   LogItemSptr l_logItem = BasicMessageHeader( a_logLevel);
   *l_logItem << "                     !                      ! ";
   return l_logItem;
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogItemSptr CLogger::BasicMessageHeader(LogLevelEnum a_logLevel)
//
// Description: Create a basig log message header that includes the timestamp 
// and thread ID
//
// Return: LogItemSptr
//
// Parameters: LogLevelEnum a_logLevel
//
///////////////////////////////////////////////////////////////////////////////
LogItemSptr CLogger::BasicMessageHeader( LogLevelEnum a_logLevel)
{
   LogItemSptr l_logItem( new std::stringstream());
   *l_logItem << LogLevelDesc[a_logLevel];

   struct timeb l_timebuffer;
   ftime( &l_timebuffer );
   struct tm* l_tm_data = localtime( &l_timebuffer.time);

   boost::format l_timestampFormat(" ! %02d/%02d/%02d %02d:%02d:%02d.%03d ! ");//0x%04X ! ");
      *l_logItem << l_timestampFormat   
      % ++l_tm_data->tm_mon
      % l_tm_data->tm_mday
      % (( l_tm_data->tm_year + 1900) % 100)
      % l_tm_data->tm_hour
      % l_tm_data->tm_min
      % l_tm_data->tm_sec
      % l_timebuffer.millitm;     
//	  % MrcpV2RefLib::MrcpUtils::ThreadID();  

   return l_logItem;
}
