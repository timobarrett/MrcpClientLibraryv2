
///////////////////////////////////////////////////////////////////////////////
// File Name: LoggerDefs.h
//
// Description: Log Level and Log Object Type definitions
//
///////////////////////////////////////////////////////////////////////////////
#ifndef LoggerDefs_h
#define LoggerDefs_h

///////////////////////////////////////////////////////////////////////////////
// Enumeration: ::LogLevelEnum
//
// Description: Log Levels used for setting the level of a message and the 
// logging threshold for an object
//
///////////////////////////////////////////////////////////////////////////////
typedef enum 
{
   LOG_LEVEL_INVALID = 0,
   LOG_LEVEL_ALWAYS,
   LOG_LEVEL_DEBUG,
   LOG_LEVEL_INFO,
   LOG_LEVEL_WARNING,
   LOG_LEVEL_ERROR,
} LogLevelEnum;

///////////////////////////////////////////////////////////////////////////////
// Enumeration: ::LogObjectTypeEnum
//
// Description: Log Object Types that each may have an independent log level 
// threshold settings.
//
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
   LogObject_Invalid = 0,
   LogObject_System,
   LogObject_Resource,
   LogObject_Session,
   LogObject_SessionMgr,
   LogObject_TaskProcessor,
   LogObject_HandleMgr,
   LogObject_CmdIdMgr,
   LogObject_Command,
   LogObject_MaxTypes
} LogObjectTypeEnum;

#endif
