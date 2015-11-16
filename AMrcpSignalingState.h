///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpSignalingState.h
//
//	Description:	class implementation
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
///////////////////////////////////////////////////////////////////////////////
#ifndef AMrcpSignalingState_h
#define AMrcpSignalingState_h

#include "CLogger.h"
#include "ClientInterfaceDefs.h"

namespace MrcpV2RefLib
{
   class AMrcpSignaling;

	class AMrcpSignalingState  
	{
	public:
		AMrcpSignalingState();
		virtual ~AMrcpSignalingState();
	        virtual int Connect( AMrcpSignaling& a_signaling);
		virtual int Start( AMrcpSignaling& a_signaling);
		virtual int OnSignalingStarted( AMrcpSignaling& a_signaling);
		virtual int OnSignalingRedirect( AMrcpSignaling& a_signaling);
		virtual int OnSignalingConnected( AMrcpSignaling& a_signaling);
                virtual int OnSignalingFailed(AMrcpSignaling& a_signaling);
		virtual int OnSignalingAvailable( AMrcpSignaling& a_signaling);
		virtual int Reinvite( AMrcpSignaling& a_signaling,MrcpResourceType a_resourceType);
                virtual int SignalingStop( AMrcpSignaling& a_signaling);
		virtual int SignalingAck( AMrcpSignaling& a_signaling);
                virtual int OnSignalingStopped( AMrcpSignaling& a_signaling);
		virtual int SignalingDisconnect( AMrcpSignaling& a_signaling );
		virtual int OnSignalingDisconnected( AMrcpSignaling& a_signaling );
                virtual int OnSignalingUnsolicitedBye( AMrcpSignaling& a_signaling );
                virtual int OnSignalingUnsolicitedByeStop( AMrcpSignaling& a_signaling );
                virtual int OnCommsFailed( AMrcpSignaling& a_signaling);
		virtual char* Description(){return "N/A";};
	
		const char* StateDesc() { return "N/A";}
		const std::string& ClassName(){return m_className;};
		void ClassName( const std::string& a_className){m_className = a_className;};
                const std::string& Name(){{ // scope for lock
                   boost::mutex::scoped_lock l_controlLock (m_loggerMutex);
                   return m_name;
                   }};
                void Name( const std::string& a_name){ { // scope for lock
                   boost::mutex::scoped_lock l_controlLock( m_loggerMutex);
                   m_name = a_name;
                   }};
//		const std::string& Name(){return m_name;};
//		void Name( const std::string& a_name){m_name = a_name;};
                void SignalPtr(const std::string& a_signal) {m_signalPtr = a_signal;};
                const std::string& SignalPtr(){return m_signalPtr;};

		LogObjectTypeEnum LogObjectType(){return LogObject_System;};

	protected:

	private:
	   std::string m_name;
	   std::string m_className;
           std::string m_signalPtr;
           boost::mutex m_loggerMutex;
           std::string TranslateState( AMrcpSignaling& a_signaling);

	};
} //end namespace

#endif

