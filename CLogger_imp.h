///////////////////////////////////////////////////////////////////////////////
// File Name: CLogger_imp.h
//
// Description: Implementation of CLogger template functions
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CLogger_imp_h
#define CLogger_imp_h

#include <boost/format.hpp>

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::Log(LogLevelEnum a_logLevel, const TMsg &a_msg)
//
// Description: Log a message with content formatting based on function 
// template parameter type.
//
// Return: void
//
// Parameters: LogLevelEnum a_logLevel
//             const TMsg &a_msg
//
///////////////////////////////////////////////////////////////////////////////
template< typename TMsg>
void CLogger::Log( LogLevelEnum a_logLevel, const TMsg& a_msg )
{
   if ((m_logLevel)&& ( a_logLevel >= m_logLevel ) && ( m_state == LOGGER_RUNNING ) )
   {
      LogItemSptr l_logItem = BlankMessageHeader( a_logLevel);
      *l_logItem << a_msg;
      QueueLogItem( l_logItem);
   }
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::Log(LogLevelEnum a_logLevel, TSource &a_source, const TMethodName &a_methodName)
//
// Description: Log a message with content formatting based on function 
// template parameter types for the source and method name.
//
// Return: void
//
// Parameters: LogLevelEnum a_logLevel
//             TSource &a_source
//             const TMethodName &a_methodName
//
///////////////////////////////////////////////////////////////////////////////
template< typename TSource, typename TMethodName>
void CLogger::Log( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName)
{
   if ((m_logLevel)&&( a_logLevel >= m_logLevelByType[a_source.LogObjectType()] ) && ( m_state == LOGGER_RUNNING ) )
   {
      LogItemSptr l_logItem = SourceInfoMessageHeader( a_logLevel, a_source, a_methodName);
      QueueLogItem( l_logItem);
   }
}

///////////////////////////////////////////////////////////////////////////////
// Function: void CLogger::Log(LogLevelEnum a_logLevel, TSource &a_source, const TMethodName &a_methodName, TMsg &a_msg)
//
// Description: Log a message with content formatting based on function 
// template parameter types for the source, method name and message.
// Return: void
//
// Parameters: LogLevelEnum a_logLevel
//             TSource &a_source
//             const TMethodName &a_methodName
//             TMsg &a_msg
//
///////////////////////////////////////////////////////////////////////////////
template< typename TSource, typename TMethodName, typename TMsg>
void CLogger::Log( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName, TMsg& a_msg)
{
   if ((m_logLevel)&&( a_logLevel >= m_logLevelByType[a_source.LogObjectType()] ) && ( m_state == LOGGER_RUNNING ) )
   {
      LogItemSptr l_logItem = SourceInfoMessageHeader( a_logLevel, a_source, a_methodName);
      *l_logItem << a_msg;
      QueueLogItem( l_logItem);
   }
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogItemSptr CLogger::SourceInfoMessageHeader(LogLevelEnum a_logLevel, TSource &a_source, const TMethodName &a_methodName)
//
// Description: Format a log message header based on function template 
// parmaeter types for message source and method name.
//
// Return: LogItemSptr
//
// Parameters: LogLevelEnum a_logLevel
//             TSource &a_source
//             const TMethodName &a_methodName
//
///////////////////////////////////////////////////////////////////////////////
template< typename TSource, typename TMethodName>
LogItemSptr CLogger::SourceInfoMessageHeader( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName)
{
   LogItemSptr l_logItem = BasicMessageHeader( a_logLevel);
   boost::format l_sourceInfoFormat("%25.25s ! %10.10s ! %s::%s ");
   *l_logItem << l_sourceInfoFormat  
      % a_source.Name()
//      % a_source.StateDesc()
      % a_source.SignalPtr()
      % a_source.ClassName()
      % a_methodName;
   return l_logItem;
}

///////////////////////////////////////////////////////////////////////////////
// Function: LogItemSptr CLogger::FormatedLogItem(LogLevelEnum a_logLevel, TSource &a_source, const TMethodName &a_methodName, TMsg &a_msg)
//
// Description: Format a log message based on function template parmaeter types
// for message source and method name.
//
// Return: LogItemSptr
//
// Parameters: LogLevelEnum a_logLevel
//             TSource &a_source
//             const TMethodName &a_methodName
//             TMsg &a_msg
//
///////////////////////////////////////////////////////////////////////////////
template< typename TSource, typename TMethodName, typename TMsg>
LogItemSptr CLogger::FormatedLogItem( LogLevelEnum a_logLevel, TSource& a_source, const TMethodName& a_methodName, TMsg& a_msg)
{
   LogItemSptr l_logItem = SourceInfoMessageHeader( a_logLevel, a_source, a_methodName);
   *l_logItem << a_msg;
   return l_logItem;
}

#endif
