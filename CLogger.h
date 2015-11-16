///////////////////////////////////////////////////////////////////////////////
// File Name: CLogger.h
//
// Description: CLogger class declaration
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CLogger_h
#define CLogger_h

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include "LoggerDefs.h"
#include "TypeDefs.h"
#include "Singleton.h"

#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Enumeration: ::LoggerStateEnum
//
// Description: Logger states
//
///////////////////////////////////////////////////////////////////////////////
typedef enum 
{
   LOGGER_STOPPED,
   LOGGER_STARTING,
   LOGGER_RUNNING,
   LOGGER_STOPPING
} LoggerStateEnum;

///////////////////////////////////////////////////////////////////////////////
// Class: ::CLogger
//
// Description: The singleton Logger class is responsible for formatting log 
// messages, displaying them to a console and writing them to a file.  Log 
// messages are assigned a log level and each component is assighed a log 
// level.  Only messages that are qualified for the component log level are 
// formatted and logged. 
//
///////////////////////////////////////////////////////////////////////////////
class CLogger : public Singleton< CLogger>
{
   friend class Singleton< CLogger>;

public:
   virtual ~CLogger();

   int Start();
   int Stop();

   template< typename TMsg>
   void Log( LogLevelEnum a_logLevel, const TMsg& a_msg );

   template< typename TSource, typename TMethodName>
   void Log( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName);

   template< typename TSource, typename TMethodName, typename TMsg>
   void Log( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName, TMsg& a_msg);

   LogLevelEnum LogLevel( LogObjectTypeEnum a_objectType);
   void LogLevel( LogObjectTypeEnum a_objectType, LogLevelEnum a_logLevel);
   void LogLevel( LogLevelEnum a_logLevel);

   const char* ClassName();
   const char* Name();
   const char* StateDesc();
   const char* SignalPtr();

   bool IsRunning();
   bool IsStopped();

//   static LogLevelEnum LogLevelFromString( std::string& a_logLevel);
   static LogObjectTypeEnum LogObjectTypeFromString( std::string& a_logObjectType);

private:
   static const char* LogLevelDesc[];
   static const char* LogObjectLevelID[];

   LoggerStateEnum m_state;
   boost::shared_ptr<boost::thread> m_thread;
   boost::mutex m_controlMutex;
   boost::mutex m_queueMutex;
   boost::mutex m_semaphoreMutex;
   boost::condition m_semaphore;
   std::queue< LogItemSptr> m_queue;
   std::ofstream m_logfile;
   LogLevelEnum m_logLevel;
   LogLevelEnum m_logLevelByType[LogObject_MaxTypes];

   CLogger();

   void QueueLogItem( LogItemSptr& a_logItem);
   void ProcessLogItems();
   void OutputLogItem( const LogItemSptr& a_logItem);

   LogItemSptr BlankMessageHeader( LogLevelEnum a_logLevel);

   template< typename TSource, typename TMethodName>
   LogItemSptr SourceInfoMessageHeader( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName);

   LogItemSptr BasicMessageHeader( LogLevelEnum a_logLevel);

   template< typename TSource, typename TMethodName, typename TMsg>
   LogItemSptr FormatedLogItem( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName, TMsg& a_msg);
};

#include "CLogger_imp.h"
#endif

