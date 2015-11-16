///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpSignalingState.cpp
//
//	Description:	 AMrcpSignalingState class implementation
//					 Part of the signaling state machine
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		AMrcpSignalingState::AMrcpSignalingState()
//		AMrcpSignalingState::~AMrcpSignalingState()
//		int AMrcpSignalingState::Start(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::OnSignalingStarted(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::Connect(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::OnSignalingConnected(AMrcpSignaling& a_signaling)
//              int AMrcpSignalingState::OnSignalingConnectFailed(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::OnSignalingAvailable(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::SendCommand(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::Reinvite(AMrcpSignaling& a_signaling,MrcpResourceType a_resourceType)
//		int AMrcpSignalingState::SignalingDisconnect(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::OnSignalingDisconnected(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::SignalingStop(AMrcpSignaling& a_signaling)
//		int AMrcpSignalingState::OnSignalingStopped(AMrcpSignaling& a_signaling)
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "AMrcpSignalingState.h"
#include "MrcpUtils.h"
#include "AMrcpSignaling.h"

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

AMrcpSignalingState::AMrcpSignalingState()
{
	ClassName("AMrcpSignalingState");
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
AMrcpSignalingState::~AMrcpSignalingState()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Start 
// Input - signaling object by reference
//         
// Output - -1 indicating bad state
//          called if start called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::Start(AMrcpSignaling& a_signaling)
{
	Name("Start");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingStarting 
// Input - signaling object by reference
//         
// Output - -1 signaling bad state
//      called if onsignalingstarted called with signaling object in wrong state    
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingStarted(AMrcpSignaling& a_signaling)
{
	Name("OnSignalingStarted");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingRedirect
// Input - signaling object by reference
//
// Output - -1 signaling bad state
//      called if onsignalingRedirect called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingRedirect(AMrcpSignaling& a_signaling)
{
        Name("OnSignalingRedirect");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
        return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Connect 
// Input - signaling object by reference
//         
// Output - -1 signaling bad state
//         called if connect called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::Connect(AMrcpSignaling& a_signaling)
{
	Name("Connect");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingConnected 
// Input - signaling object by reference
//         
// Output - -1 signaling bad state
//       called if onSignalingConnected called with signaling object in wrong state   
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingConnected(AMrcpSignaling& a_signaling)
{
	Name("OnSignalingConnected");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingFailed
// Input - signaling object by reference
//
// Output - -1 signaling bad state
//       called if onSignalingConnectFailed called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////
int AMrcpSignalingState::OnSignalingFailed(AMrcpSignaling& a_signaling)
{
        Name("OnSignalingFailed");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
        return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingAvailable 
// Input - signaling object
//         
// Output - -1 signaling bad state
//         called if onsignalingavailable called with signaling object in wrong state 
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingAvailable(AMrcpSignaling& a_signaling)
{
	Name("OnSignalingAvailable");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Reinvite 
// Input - signaling object
//         resource type
// Output - -1 signaling bad state
//          called if reinvite called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::Reinvite(AMrcpSignaling& a_signaling,MrcpResourceType a_resourceType)
{
	Name("AMrcpSignalingState::Reinvite");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SignalingDisconnect 
// Input - signaling object
//         
// Output - -1 signaling bad state
//          called if signalingdisconnect called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::SignalingDisconnect(AMrcpSignaling& a_signaling)
{
	Name("SignalingDisconnect");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingDisconnected 
// Input - signaling object
//         
// Output - -1 signaling bad state
//      called if onsignalingdisconnected called with signaling object in wrong state    
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingDisconnected(AMrcpSignaling& a_signaling)
{
	Name("OnSignalingDisconnected");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingUnsolicitedBye
// Input - signaling object
//
// Output - -1 signaling bad state
//      called if onsignalingunsolicitedbye called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingUnsolicitedBye(AMrcpSignaling& a_signaling)
{
        Name("OnSignalingUnsolicitedBye");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
        return -1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingUnsolicitedByeStop
// Input - signaling object
//
// Output - -1 signaling bad state
//      called if onsignalingunsolicitedbye called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingUnsolicitedByeStop(AMrcpSignaling& a_signaling)
{
        Name("OnSignalingUnsolicitedByeStop");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
        return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Stop 
// Input - signaling object
//         
// Output - -1 signaling bad state
//        called if signalingstop called with signaling object in wrong state  
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::SignalingStop(AMrcpSignaling& a_signaling)
{
	Name("SignalingStop");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SginalingAck 
// Input - signaling object
//         
// Output - -1 signaling bad state
//        called if signalingAck called with signaling object in wrong state  
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::SignalingAck(AMrcpSignaling& a_signaling)
{
	Name("SignalingAck");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStopped 
// Input - signaling object
//         
// Output - -1 indicating a bad state
//        called if onsignalingstopped called with signaling object in wrong state  
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnSignalingStopped(AMrcpSignaling& a_signaling)
{
	Name("OnSignalingStopped");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
	return -1;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - OnCommsFailed
// Input - signaling object
//
// Output - -1 indicating a bad state
//        called if OnCommsFailed called with signaling object in wrong state
///////////////////////////////////////////////////////////////////////////

int AMrcpSignalingState::OnCommsFailed(AMrcpSignaling& a_signaling)
{
        Name("OnCommsFailed");
        std::string lResults = TranslateState(a_signaling);
        CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",lResults);
        return -1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - TranslateState
// Input - signaling object
//
// Output - returns a string containing state of object
//        
///////////////////////////////////////////////////////////////////////////

std::string AMrcpSignalingState::TranslateState(AMrcpSignaling& a_signaling)
{
   std::string lResults;
   std::string lCallID = a_signaling.GetCallID();

   if (a_signaling.m_state == a_signaling.m_signalingUninitializedState)
      lResults = "Uninitialized - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingStartingState)    
      lResults= "Starting - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingStartedState)
      lResults = "Started - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingConnectingState)
      lResults = "Connecting - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingConnectedState)
      lResults = "Connected - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingAvailableState)
      lResults = "Available - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingStoppingState)
      lResults = "Stopping - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingStoppedState)
      lResults = "Stopped - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingDisconnectingState)
      lResults = "Disconnecting - CallID = " + lCallID;
   else if (a_signaling.m_state == a_signaling.m_signalingDisconnectedState)
      lResults = "Disconnected - CallID = " + lCallID;

   return lResults;
}

} //end namespace
