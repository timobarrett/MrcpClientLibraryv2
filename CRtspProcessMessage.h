///////////////////////////////////////////////////////////////////////////////
//      File Name:              CRtspProcessMessages.h
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
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CRtspProcessMessage_h
#define CRtspProcessMessage_h

#define M_ENDL "\r\n"

#include "MrcpTasks.h"
#include "CLogger.h"
#include "CMrcpRtspSignal.h"

namespace MrcpV2RefLib
{
      class CRtspProcessMessage : public MrcpTasks
      {
        public:
                CRtspProcessMessage(CMrcpRtspSignal* a_signalObj);
                virtual ~CRtspProcessMessage();
                void Execute();
                void SetMessageBuffer(std::string a_messageBuffer)
                {
                        boost::mutex::scoped_lock l_controlLock(m_messageMutex);
                        m_messageBuffer.clear();
                        m_messageBuffer = a_messageBuffer;
                };
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
            std::string m_messageBuffer;
            std::string m_className;
            std::string m_name;
            std::string m_signalPtr;
            MrcpV2RefLib::CMrcpRtspSignal* m_signalObj;
            boost::mutex m_loggerMutex;
            boost::mutex m_messageMutex;
      };

} //end namespace

#endif
