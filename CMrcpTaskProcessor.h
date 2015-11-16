///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpTaskProcessor.h
//
//	Description:	 class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CMrcpTaskProcessor_H
#define CMrcpTaskProcessor_H

#include "Singleton.h"
#include "CLogger.h"
#include "CMrcpSystem.h"
#include <map>
#include <string>

namespace MrcpV2RefLib
{

   class MrcpTasks;

class CMrcpTaskProcessor :public Singleton <CMrcpSystem>
	                     
{
   friend class Singleton<CMrcpTaskProcessor>;

public:
   virtual ~CMrcpTaskProcessor();

   int Start();
   int Stop();
   int QueueTask( MrcpTasks* a_mrcpTasks); 
   static CMrcpTaskProcessor* Instance(std::string a_idString = "TASK");

   bool EventProcessorRunning() { return m_eventProcessorRunning; };
   void EventProcessorRunning(bool eventProcessorRunning) { m_eventProcessorRunning = eventProcessorRunning;};
   

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
protected:

private:
   //
   typedef std::map<std::string, CMrcpTaskProcessor*> CMrcpTaskProcessorByStringMap;
   static CMrcpTaskProcessorByStringMap m_taskProcessors;

   CMrcpTaskProcessor(std::string a_idString);
   boost::shared_ptr<boost::thread> m_thread;
   boost::mutex m_controlMutex;
   boost::mutex m_queueMutex;
   boost::mutex m_semaphoreMutex;
   boost::mutex m_loggerMutex;

   static boost::mutex m_singleton;
   boost::condition m_semaphore;
   std::queue< MrcpTasks*> m_queue;
   std::string m_idString;


   bool m_eventProcessorRunning;
   void ProcessTasks();

   std::string m_className;
   std::string m_name;
   std::string m_signalPtr;

};

}//end namespace

#endif

