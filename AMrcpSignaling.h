///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpSignaling.h
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
#ifndef AMrcpSignaling_h
#define AMrcpSignaling_h

#include "AMrcpObserverSubject.h"
#include "ClientInterfaceDefs.h"
#include "CMrcpSignalingStates.h"
#include "CLogger.h"

#include <string>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
typedef int SOCKET;
#endif
namespace MrcpV2RefLib
{
	class AMrcpSignaling : public AMrcpObserverSubject
	{
	   friend class AMrcpSignalingState;
	   friend class CMrcpSignalingUninitializedState;
	   friend class CMrcpSignalingStartingState;
	   friend class CMrcpSignalingStartedState; 
	   friend class CMrcpSignalingConnectingState;
	   friend class CMrcpSignalingConnectedState;
	   friend class CMrcpSignalingAvailableState;
	   friend class CMrcpSignalingStoppingState;
	   friend class CMrcpSignalingStoppedState;
	   friend class CMrcpSignalingDisconnectingState;
	   friend class CMrcpSignalingDisconnectedState;

	   friend class CSignalingStartCommand;
	   friend class CSignalingOnRedirectEvent;
	   friend class CSignalingConnectCommand;
	   friend class CSignalingOnStartedEvent;
	   friend class CSignalingOnFailedEvent;
	   friend class CSignalingOnAvailableEvent;
	   friend class CSignalingOnConnectedEvent;
           friend class CSignalingUnsolicitedByeAck;
           friend class CSignalingUnsolicitedByeStop;
	   friend class CSignalingStopCommand;
	   friend class CSignalingAckCommand;
	   friend class CSignalingDisconnectCommand;
	   friend class CSignalingOnStoppedEvent;
	   friend class CSignalingOnDisconnectedEvent;
	   friend class CSipInviteCommand;
	   friend class CSipReinviteCommand;
	   friend class CSignalingReinviteCommand;
	   friend class CSipProcessMessage;
	   friend class CRtspProcessMessage;
	   friend class CSipAckResponseMessage;
	   friend class CSipByeMessage;
	   friend class CMrcpSendCommand;
           friend class CMrcpSendSignal;
	   friend class CMrcpProcessCommandResults;
	   friend class CSpeechRecognizer;
	   friend class CSpeechSynthesizer;
	   friend class CMrcpSipSignal;
           friend class CMrcpRtspSignal;
	   friend class CMrcpGeneric;
	   friend class CMrcpSession;
	   friend class MrcpTasks;
	   friend class AMrcpResourceState;
           friend class CSignalingCommsFailed;
           friend class CSignalingCleanupComms;

	public:
		AMrcpSignaling();
		virtual ~AMrcpSignaling();
		void BasicInitialization();
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
		AMrcpSignalingState* m_state;
	   AMrcpSignalingState* m_signalingUninitializedState;
	   AMrcpSignalingState* m_signalingStartingState;
	   AMrcpSignalingState* m_signalingStartedState;
	   AMrcpSignalingState* m_signalingConnectingState;
	   AMrcpSignalingState* m_signalingConnectedState;
	   AMrcpSignalingState* m_signalingAvailableState;
	   AMrcpSignalingState* m_signalingStoppingState;
	   AMrcpSignalingState* m_signalingStoppedState;
	   AMrcpSignalingState* m_signalingDisconnectingState;
	   AMrcpSignalingState* m_signalingDisconnectedState;

	   virtual int PrimConnect()   = 0;
	   virtual int PrimStart()     = 0;
	   virtual int PrimOnSignalingStarted() = 0;
           virtual int PrimOnSignalingConnected() = 0;
           virtual int PrimRedirect() = 0;
           virtual int PrimOnSignalingFailed() = 0;
	   virtual int PrimSendCommand(const std::string& a_message, std::string a_messageType) = 0;
	   virtual int PrimSendSignal(const std::string& a_message) = 0;
	   virtual int PrimProcessResults(const std::string& a_message) = 0;
	   virtual int PrimStop()      = 0;
	   virtual int PrimSendAck() = 0;
	   virtual int PrimOnStopped() = 0;
	   virtual int PrimDisconnect() = 0;
	   virtual int PrimReinvite(MrcpResourceType a_resourceType) = 0;
           virtual int PrimUnsolicitedByeStop() = 0;
           virtual int PrimOnCommsFailed() = 0;
           virtual int PrimByeOk() = 0;
	   virtual void SetAckToValue(std::string a_value) = 0;
           virtual void SetToTag(std::string a_value) = 0;
           virtual void SetFromTag(std::string a_value) = 0;
           virtual void SetSignalFail(bool a_value) = 0;
           virtual void SetRedirect(bool a_sigRedirect) = 0;
           virtual bool IsSignalingStopped()=0;
           virtual void ConnectBackoff(int a_value) = 0;
           virtual int  ConnectBackoff() = 0;
           virtual bool IsSipEventReceiverRunning() = 0;
           virtual bool IsCommandEventReceiverRunning() = 0;
           virtual std::string GetCallID() = 0;

           SOCKET SetupFd() { return m_mrcpSetupFd;};
           void SetupFd(SOCKET a_mrcpSocket) {m_mrcpSetupFd = a_mrcpSocket;};

	   SOCKET SetupSocket() { return m_mrcpSetupSocket;};
	   void SetupSocket(SOCKET a_mrcpSocket) { m_mrcpSetupSocket = a_mrcpSocket;};

	   SOCKET CommandSocket() { return m_mrcpCommandSocket;};
	   void CommandSocket(SOCKET a_mrcpSocket) {m_mrcpCommandSocket = a_mrcpSocket;};

	   MrcpSessionHandle TtsSessionHandle() {return m_ttsSessionHandle;};
	   void TtsSessionHandle(MrcpSessionHandle a_sessionHandle){m_ttsSessionHandle = a_sessionHandle;};

	   MrcpSessionHandle AsrSessionHandle() {return m_asrSessionHandle;};
	   void AsrSessionHandle(MrcpSessionHandle a_sessionHandle) {m_asrSessionHandle = a_sessionHandle;};


	   std::string ComRecogChannelID() { return m_recogChannelID; };
	   void ComRecogChannelID(std::string a_recogChannelID) { m_recogChannelID = a_recogChannelID; };

	   std::string ComSynthChannelID() { return m_synthChannelID; };
	   void ComSynthChannelID(std::string a_synthChannelID) { m_synthChannelID = a_synthChannelID; };


	   int ClientRtpPort() {return m_clientRtpPort;};
	   void ClientRtpPort(int a_clientRtpPort) {m_clientRtpPort = a_clientRtpPort;};

	   int ServerRtpPort() {return m_serverRtpPort;};
	   void ServerRtpPort(int a_serverRtpPort) {m_serverRtpPort = a_serverRtpPort;};

	   std::string ServerAddress() {return m_serverAddress;};
	   void ServerAddress(std::string a_serverAddress) {m_serverAddress = a_serverAddress;};

	   std::string RedirectServerAddress() {return m_redirectServerAddress;};
	   void RedirectServerAddress(std::string a_serverAddress) {m_redirectServerAddress = a_serverAddress;};
	   
           int ServerPort() {return m_serverPort;};
	   void ServerPort(int a_serverPort) {m_serverPort = a_serverPort;};

           int RedirectServerPort() {return m_redirectServerPort;};
	   void RedirectServerPort(int a_serverPort) {m_redirectServerPort = a_serverPort;};
	   
           std::string ClientAddress() {return m_clientAddress;};
	   void ClientAddress(std::string a_clientAddress) {m_clientAddress = a_clientAddress;};


	private:
	   int SetState(AMrcpSignalingState* state);
	   int SetStateToStarting() { return SetState (m_signalingStartingState );};
	   int SetStateToStarted()	{ return SetState( m_signalingStartedState);};
	   int SetStateToConnecting() { return SetState( m_signalingConnectingState );};
	   int SetStateToConnected()  { return SetState( m_signalingConnectedState );};
	   int SetStateToAvailable() { return SetState( m_signalingAvailableState );};
	   int SetStateToStopping() { return SetState( m_signalingStoppingState );};
	   int SetStateToStopped() { return SetState( m_signalingStoppedState );};
	   int SetStateToDisconnecting() { return SetState( m_signalingDisconnectingState );};
	   int SetStateToDisconnected() { return SetState( m_signalingDisconnectedState ); };
	   int SetStateToUninitialized() { return SetState( m_signalingUninitializedState );};



	   int Start();
	   int Connect();
	   int Reinvite(MrcpResourceType a_resourceToAdd);
	   int OnSignalingStarted();
           int OnSignalingRedirect();
	   int OnSignalingFailed();
	   int OnSignalingConnected();
	   int OnSignalingAvailable();
	   int OnSignalingStopped();
	   int OnSignalingDisconnected();
           int OnSignalingUnsolicitedBye();
           int OnSignalingUnsolicitedByeStop();
           int CleanupUnsolicitedBye();
	   int SignalingDisconnect();
	   int SignalingStop();
	   int SignalingAck();
	   int OnStopped();
           int CleanupCommsFailure(); 
           int OnCommsFail();
	   virtual int SetupCommandProcessing(int a_commandSocket) = 0;


	    SOCKET m_mrcpSetupSocket;
            SOCKET m_mrcpSetupFd;
		SOCKET m_mrcpCommandSocket;

	   //config storage
	   std::string m_serverAddress;
           std::string m_redirectServerAddress;
	   int m_serverPort;
           int m_redirectServerPort;
           int m_serverFd;
	   std::string m_clientAddress;
	   int m_clientRtpPort;
	   int m_serverRtpPort;
	   int m_cSeq;
           std::string m_className;
           std::string m_name;
           boost::mutex m_loggerMutex;
           std::string m_signalPtr;

	   MrcpSessionHandle m_ttsSessionHandle;
	   MrcpSessionHandle m_asrSessionHandle;
		std::string m_recogChannelID;
		std::string m_synthChannelID;
	};

} //end namespace
#endif
