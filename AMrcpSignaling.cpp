///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpSignaling.cpp
//
//	Description:	AMrcpSignaling class implementation
//					part of the signaling state machine
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		AMrcpSignaling::AMrcpSignaling()
//		AMrcpSignaling::~AMrcpSignaling()
//		void AMrcpSignaling::BasicInitialization()
//		int AMrcpSignaling::Start()
//		int AMrcpSignaling::Reinvite(MrcpResourceType a_resourceToAdd)
//		int AMrcpSignaling::Connect()
//		int AMrcpSignaling::SendCommand()
//		int AMrcpSignaling::OnSignalingStarted()
//		int AMrcpSignaling::OnSignalingConnected()
//		int AMrcpSignaling::OnSignalingAvailable()
//		int AMrcpSignaling::SignalingStop()
//		int AMrcpSignaling::OnSignalingStopped()
//		int AMrcpSignaling::SignalingDisconnect()
//		int AMrcpSignaling::OnSignalingDisconnected()
//		int AMrcpSignaling::SetState(AMrcpSignalingState* a_state)
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "AMrcpSignaling.h"
#include "MrcpClientEvents.h"
#include "ClientInterfaceDefs.h"
#include "AMrcpEventHandler.h"
#include "MrcpUtils.h"

#include "AMrcpObserverSubject.h"

namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////

AMrcpSignaling::AMrcpSignaling()
	: m_state(0)
	, m_signalingUninitializedState(0)
	, m_signalingStartingState(0)
	, m_signalingStartedState(0)
	, m_signalingConnectingState(0)
	, m_signalingConnectedState(0)
	, m_signalingAvailableState(0)
	, m_signalingStoppingState(0)
	, m_signalingStoppedState(0)
	, m_signalingDisconnectingState(0)
	, m_signalingDisconnectedState(0)
	, m_mrcpSetupSocket(0)
	, m_mrcpCommandSocket(0)
        , m_mrcpSetupFd(0)
	, m_synthChannelID(" ")
	, m_recogChannelID(" ")
	, m_cSeq (100)
        , m_serverPort(0)
        , m_serverFd(0)
        , m_clientRtpPort(0)
        , m_serverRtpPort(0)
        , m_ttsSessionHandle(0)
        , m_asrSessionHandle(0)

{
        m_recogChannelID.erase();
        m_synthChannelID.erase();
        m_serverAddress.erase();
        m_clientAddress.erase();
	BasicInitialization();
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
AMrcpSignaling::~AMrcpSignaling()
{
	m_state = 0;
	delete m_signalingUninitializedState;
	delete m_signalingStartingState;
	delete m_signalingStartedState;
	delete m_signalingConnectingState;
	delete m_signalingConnectedState;
	delete m_signalingAvailableState;
	delete m_signalingStoppingState;
	delete m_signalingStoppedState;
	delete m_signalingDisconnectingState;
	delete m_signalingDisconnectedState;

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - BasicInitialization
// Input - None
//         
// Output - None
//          Instantiate each of the state classes
//			set the initial signaling state
///////////////////////////////////////////////////////////////////////////
void AMrcpSignaling::BasicInitialization()
{
	m_signalingUninitializedState = new CMrcpSignalingUninitializedState();
	m_signalingStartingState = new CMrcpSignalingStartingState();
	m_signalingStartedState = new CMrcpSignalingStartedState();
	m_signalingConnectingState = new CMrcpSignalingConnectingState();
	m_signalingConnectedState = new CMrcpSignalingConnectedState();
	m_signalingAvailableState = new CMrcpSignalingAvailableState();
	m_signalingStoppingState = new CMrcpSignalingStoppingState();
	m_signalingStoppedState = new CMrcpSignalingStoppedState();
	m_signalingDisconnectingState = new CMrcpSignalingDisconnectingState();
	m_signalingDisconnectedState = new CMrcpSignalingDisconnectedState();

	m_state = m_signalingUninitializedState;

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Start
// Input - None
//         
// Output - call to start state
//        start the signaling object  
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::Start()
{
	return m_state->Start( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Reinvite
// Input - resource to add to a session
//         
// Output - results of set state ot reinvite
//      call reinvite method for the signaling state    
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::Reinvite(MrcpResourceType a_resourceToAdd)
{
	return m_state->Reinvite( *this ,a_resourceToAdd);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Connect
// Input - None
//         
// Output - results of setting state to connect
//          call the connect method for the signaling state
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::Connect()
{
	return m_state->Connect( *this );
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingStarted
// Input - None
//         
// Output - result of setting state to signaling started
//         call signaling states on signaling started method 
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingStarted()
{
	return m_state->OnSignalingStarted( *this );
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingRedirect
// Input - None
//
// Output - result of setting state to signaling started
//         call signaling states on signaling started method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingRedirect()
{
        return m_state->OnSignalingRedirect( *this );
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingFailed
// Input - None
//         
// Output - result of setting state to signaling failed
//         call signaling states on signaling failed method 
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingFailed()
{
//	NotifyObservers(SIGNALING_PROTOCOL_FAILED);
        return m_state->OnSignalingFailed( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingConnected
// Input - None
//         
// Output - results of setting state to signaling connected
//          call signaling states on signaling connected method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingConnected()
{	
//	NotifyObservers(SIGNALING_PROTOCOL_CONNECTED);
  std::string lResults;
        if (m_state == m_signalingUninitializedState)
           lResults = "Uninitialized";
        else if (m_state == m_signalingStartingState)
           lResults= "Starting";
        else if (m_state == m_signalingStartedState)
           lResults = "Started";
        else if (m_state == m_signalingConnectingState)
           lResults = "Connecting";
        else if (m_state == m_signalingConnectedState)
           lResults = "Connected";
        else if (m_state == m_signalingAvailableState)
           lResults = "Available";
        else if (m_state == m_signalingStoppingState)
           lResults = "Stopping";
        else if (m_state == m_signalingStoppedState)
           lResults = "Stopped";
        else if (m_state == m_signalingDisconnectingState)
           lResults = "Disconnecting";
        else if (m_state == m_signalingDisconnectedState)
           lResults = "Disconnected";

        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, "OnSignalingConnected",lResults);
	return m_state->OnSignalingConnected( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingAvailable
// Input - None
//         
// Output - results of setting state to signaling available
//         call signaling states on signaling available method 
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingAvailable()
{
	return m_state->OnSignalingAvailable( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Ack
// Input - None
//         
// Output - results of setting state to send Ack
//         call signaling objects Ack method  
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::SignalingAck()
{
	return m_state->SignalingAck( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Stop
// Input - None
//         
// Output - results of setting state to signaling stop
//         call signaling objects stop method  
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::SignalingStop()
{
	return m_state->SignalingStop( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingStopped
// Input - None
//         
// Output - rtesults of setting state to signaling stopped
//          call signaling states on signaling stopped method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingStopped()
{
	return m_state->OnSignalingStopped( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SignalingDisconnect
// Input - None
//         
// Output - results of setting state to signaling disconnect
//          call signaling states signaling disconnect method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::SignalingDisconnect()
{
	return m_state->SignalingDisconnect( *this );
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingDisconnected
// Input - None
//         
// Output - results of setting state to signaling disconnected
//         call signaling states on discconected method 
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingDisconnected()
{
	return m_state->OnSignalingDisconnected( *this );
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingUnsolicitedBye
// Input - None
//
// Output - results of setting state to signaling disconnected
//         call signaling states on discconected method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingUnsolicitedBye()
{
        return m_state->OnSignalingUnsolicitedBye( *this );
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingUnsolicitedByeStop
// Input - None
//
// Output - results of setting state to signaling disconnected
//         call signaling states on discconected method
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnSignalingUnsolicitedByeStop()
{
        return m_state->OnSignalingUnsolicitedByeStop( *this );
}

//////////////////////////////////////////////////////////////////////////
//
// Description - OnCommsFail
// Input - None
//
// Output - result of comms (socket) failure to Mrcp Server
//         
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::OnCommsFail()
{
    std::string lResults;

        Name("OnCommsFail");
        if (m_state == m_signalingUninitializedState)
           lResults = "Uninitialized";
        else if (m_state == m_signalingStartingState)
           lResults= "Starting";
        else if (m_state == m_signalingStartedState)
           lResults = "Started";
        else if (m_state == m_signalingConnectingState)
           lResults = "Connecting";
        else if (m_state == m_signalingConnectedState)
           lResults = "Connected";
        else if (m_state == m_signalingAvailableState)
           lResults = "Available";
        else if (m_state == m_signalingStoppingState)
           lResults = "Stopping";
        else if (m_state == m_signalingStoppedState)
           lResults = "Stopped";
        else if (m_state == m_signalingDisconnectingState)
           lResults = "Disconnecting";
        else if (m_state == m_signalingDisconnectedState)
           lResults = "Disconnected";

        CLogger::Instance()->Log( LOG_LEVEL_DEBUG,*this, "OnCommsFail State = ",lResults);
        if (!m_state)
           return -1;
        else  
           return m_state->OnCommsFailed( *this );
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - SetState
// Input - pointer to state to set
//         
// Output - success
//          used to set the state for the signaling object
///////////////////////////////////////////////////////////////////////////
int AMrcpSignaling::SetState(AMrcpSignalingState* a_state)
{
	m_state = a_state;

	return MrcpSuccess;
}


}//end namespace

