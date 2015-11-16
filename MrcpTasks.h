///////////////////////////////////////////////////////////////////////////////
//	File Name:		MrcpTasks.h
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

///////////////////////////////////////////////////////////////////////////////
#ifndef MrcpTasks_h
#define MrcpTasks_h

#include "ClientInterfaceDefs.h"
#include "AMrcpSignaling.h"
#include <string>

namespace MrcpV2RefLib
{
	class AMrcpSignaling;
/////////
	class MrcpTasks
	{
	   friend class CMrcpTaskProcessor;
	   friend class CSpeechRecognizer;

	public:
	   MrcpTasks(AMrcpSignaling* a_signalObj);
	   virtual ~MrcpTasks();
	   MrcpSessionHandle TtsSessionHandle() {return m_signalObj->TtsSessionHandle();};
	   MrcpSessionHandle AsrSessionHandle() {return m_signalObj->AsrSessionHandle();};

	protected:
		MrcpTasks(){}; // no implementation
	   AMrcpSignaling* m_signalObj;

	private:
	   virtual void Execute() = 0;
	};
//////////

	class CSignalingStartCommand : public MrcpTasks
	{
	public:
	   CSignalingStartCommand( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingStartCommand();
	   void Execute();

	private:
	   CSignalingStartCommand(); // no implementation
	};

//////////
	class CSignalingConnectCommand : public MrcpTasks
	{
	public:
	   CSignalingConnectCommand( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingConnectCommand();
	   void Execute();

	private:
	   CSignalingConnectCommand(); // no implementation
	};
//////////
	class CSignalingReinviteCommand : public MrcpTasks
	{
	public:
	   CSignalingReinviteCommand( AMrcpSignaling* a_signalObj,MrcpResourceType a_resourceToAdd);
	   virtual ~CSignalingReinviteCommand();
	   void Execute();
	private:
	   MrcpResourceType m_resourceToAdd;
	   CSignalingReinviteCommand(); // no implementation
	};
//////////////

	class CSignalingOnStartedEvent : public MrcpTasks
	{
	public:
	   CSignalingOnStartedEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnStartedEvent();
	   void Execute();

	private:
	   CSignalingOnStartedEvent(); // no implementation
	};
//////////////

        class CSignalingOnRedirectEvent : public MrcpTasks
        {
        public:
           CSignalingOnRedirectEvent( AMrcpSignaling* a_signalObj);
           virtual ~CSignalingOnRedirectEvent();
           void Execute();

        private:
           CSignalingOnRedirectEvent(); // no implementation
        };
//////////////

	class CSignalingOnFailedEvent : public MrcpTasks
	{
	public:
	   CSignalingOnFailedEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnFailedEvent();
	   void Execute();

	private:
	   CSignalingOnFailedEvent(); // no implementation
	};
/////////
	class CSignalingOnConnectedEvent : public MrcpTasks
	{
	public:
	   CSignalingOnConnectedEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnConnectedEvent();
	   void Execute();
           void SetSipResult(int a_sipResult){m_sipResult = a_sipResult;};
           int getSipResult(){return m_sipResult;};
	private:
	   CSignalingOnConnectedEvent(); // no implementation
           int m_sipResult;
	};
/////////
	class CSignalingOnAvailableEvent : public MrcpTasks
	{
	public:
	   CSignalingOnAvailableEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnAvailableEvent();
	   void Execute();

	private:
	   CSignalingOnAvailableEvent(); // no implementation
	};


//////////
	class CSignalingStopCommand : public MrcpTasks
	{
	public:
	   CSignalingStopCommand( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingStopCommand();
	   void Execute();

	private:
	   CSignalingStopCommand(); // no implementation
	};
/////////
	class CSignalingAckCommand : public MrcpTasks
	{
	public:
	   CSignalingAckCommand( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingAckCommand();
	   void Execute();
	private:
	   CSignalingAckCommand(); // no implementation
	   std::string m_messageBuffer;
	};
//////////

	class CSignalingOnStoppedEvent : public MrcpTasks
	{
	public:
	   CSignalingOnStoppedEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnStoppedEvent();
	   void Execute();

	private:
	   CSignalingOnStoppedEvent(); // no implementation
	};
//////////

	class CSignalingDisconnectCommand : public MrcpTasks
	{
	public:
	   CSignalingDisconnectCommand( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingDisconnectCommand();
	   void Execute();

	private:
	   CSignalingDisconnectCommand(); // no implementation
	};
//////////

	class CSignalingOnDisconnectedEvent : public MrcpTasks
	{
	public:
	   CSignalingOnDisconnectedEvent( AMrcpSignaling* a_signalObj);
	   virtual ~CSignalingOnDisconnectedEvent();
	   void Execute();
           void SetSipResult(int a_sipResult){m_sipResult = a_sipResult;};
           int getSipResult(){return m_sipResult;};

	private:
	   CSignalingOnDisconnectedEvent(); // no implementation
           int m_sipResult;
	};
//////////
	class CMrcpProcessCommandResults : public MrcpTasks
	{
	public:       
		CMrcpProcessCommandResults(AMrcpSignaling* a_signalObj);
		virtual ~CMrcpProcessCommandResults();

	   void Execute();
	   void MessageBuffer(const std::string& a_messageBuffer) { m_messageBuffer = a_messageBuffer;}
	   const std::string& MessageBuffer() { return m_messageBuffer; }

	private:
		std::string m_messageBuffer;
	};

//////////

        class CSignalingUnsolicitedByeAck : public MrcpTasks
        {
        public:
           CSignalingUnsolicitedByeAck( AMrcpSignaling* a_signalObj);
           virtual ~CSignalingUnsolicitedByeAck();
           void Execute();
           void SetSipResult(int a_sipResult){m_sipResult = a_sipResult;};
           int getSipResult(){return m_sipResult;};

        private:
           CSignalingUnsolicitedByeAck(); // no implementation
           int m_sipResult;
        };
//////////
        class CSignalingUnsolicitedByeStop : public MrcpTasks
        {
        public:
           CSignalingUnsolicitedByeStop( AMrcpSignaling* a_signalObj);
           virtual ~CSignalingUnsolicitedByeStop();
           void Execute();
           void SetSipResult(int a_sipResult){m_sipResult = a_sipResult;};
           int getSipResult(){return m_sipResult;};

        private:
           CSignalingUnsolicitedByeStop(); // no implementation
           int m_sipResult;
        };

//////////
        class CSignalingCommsFailed : public MrcpTasks
        {
        public:
           CSignalingCommsFailed( AMrcpSignaling* a_signalObj);
           virtual ~CSignalingCommsFailed();
           void Execute();

        private:
           CSignalingCommsFailed(); // no implementation
        };

}//end namespace
#endif

