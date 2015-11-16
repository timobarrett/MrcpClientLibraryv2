///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSignalingStates.cpp
//
//	Description:	CMrcpSignalingStates class implementation
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpSignalingUninitializedState::CMrcpSignalingUninitializedState;
//		CMrcpSignalingUninitializedState::~CMrcpSignalingUninitializedState;
//	    CMrcpSignalingStartingState::CMrcpSignalingStartingState
//		CMrcpSignalingStartingState::~CMrcpSignalingStartingState
//		CMrcpSignalingStartedState::CMrcpSignalingStartedState
//		CMrcpSignalingStartedState::~CMrcpSignalingStartedState
//		CMrcpSignalingConnectingState::CMrcpSignalingConnectingState
//		CMrcpSignalingConnectingState::~CMrcpSignalingConnectingState
//	    CMrcpSignalingConnectedState::CMrcpSignalingConnectedState
//	    CMrcpSignalingConnectedState::~CMrcpSignalingConnectedState
//		CMrcpSignalingAvailableState::CMrcpSignalingAvailableState
//		CMrcpSignalingAvailableState::~CMrcpSignalingAvailableState
//		CMrcpSignalingDisconnectingState::CMrcpSignalingDisconnectingState
//		CMrcpSignalingDisconnectingState::~CMrcpSignalingDisconnectingState
//		CMrcpSignalingDisconnectedState::CMrcpSignalingDisconnectedState
//		CMrcpSignalingDisconnectedState::~CMrcpSignalingDisconnectedState
//		CMrcpSignalingStoppingState::CMrcpSignalingStoppingState
//		CMrcpSignalingStoppingState::~CMrcpSignalingStoppingState
//		CMrcpSignalingStoppedState::CMrcpSignalingStoppedState
//		CMrcpSignalingStoppedState::~CMrcpSignalingStoppedState
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "CMrcpSignalingStates.h"
#include "MrcpClientEvents.h"
#include "ClientInterfaceDefs.h"
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
CMrcpSignalingUninitializedState::CMrcpSignalingUninitializedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingUninitializedState::~CMrcpSignalingUninitializedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Start
// Input - signaling subject by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingUninitializedState::Start(AMrcpSignaling& a_subject)
{

	if (a_subject.PrimStart() != -1)
		a_subject.SetStateToStarting();
	//else and trigger callback
	return MrcpSuccess;
}  
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingFailed
// Input - signaling object by reference
//
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingUninitializedState::OnSignalingFailed(AMrcpSignaling& a_subject)
{
        a_subject.PrimOnSignalingFailed();
        return MrcpSuccess;
}
 
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStartingState::CMrcpSignalingStartingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStartingState::~CMrcpSignalingStartingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingStarted
// Input - signaling object by reference
//         
// Output - success or failure
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingStartingState::OnSignalingStarted(AMrcpSignaling& a_subject)
{
	int l_rc = a_subject.PrimOnSignalingStarted();

	if (l_rc != -1)
	{
		a_subject.SetStateToStarted();
	}

	return l_rc;  
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingFailed
// Input - signaling object by reference
//
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingStartingState::OnSignalingFailed(AMrcpSignaling& a_subject)
{
        a_subject.PrimOnSignalingFailed();
        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Redirect
// Input - signaling object by reference
//
// Output - success or failure
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
//int CMrcpSignalingRedirectingState::Redirect(AMrcpSignaling& a_subject)
int CMrcpSignalingConnectingState::OnSignalingRedirect(AMrcpSignaling& a_subject)
{
        a_subject.SetStateToUninitialized();
        int l_rc = a_subject.PrimRedirect();

        return l_rc;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStartedState::CMrcpSignalingStartedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStartedState::~CMrcpSignalingStartedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Connect
// Input - signaling object by reference
//         
// Output - success or failure
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingStartedState::Connect(AMrcpSignaling& a_subject)
{   
	int l_rc = a_subject.PrimConnect();
	if (l_rc != -1)
	{
		a_subject.SetStateToConnecting();
	}

	return l_rc;

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingConnectingState::CMrcpSignalingConnectingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingConnectingState::~CMrcpSignalingConnectingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingConnected
// Input - signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectingState::OnSignalingConnected(AMrcpSignaling& a_subject)
{
//	a_subject.SetStateToConnected();
        a_subject.PrimOnSignalingConnected();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingFailed
// Input - signaling object by reference
//
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectingState::OnSignalingFailed(AMrcpSignaling& a_subject)
{
        a_subject.PrimOnSignalingFailed();
        return MrcpSuccess;
}
   
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingConnected - sends ACK to server
// Input - Signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectingState::SignalingAck(AMrcpSignaling& a_subject)
{
	int result = a_subject.PrimSendAck();
	return MrcpSuccess;
}  
//////////////////////////////////////////////////////////////////////////
//
// Description - ConnectCmdSocket - connects Command socket
// Input - Signaling object by reference, socket to connect to
//
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectingState::ConnectCmdSocket(AMrcpSignaling& a_subject,int a_cmdSocket)
{
        int result = a_subject.SetupCommandProcessing(a_cmdSocket);
        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Communications socket failed to MRCP Server
// Input - signaling object by reference
//
// Output - result of a failure in the sip signaling socket
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectingState::OnCommsFailed(AMrcpSignaling& a_subject)
{
        int l_result = a_subject.PrimOnCommsFailed();  

/*        l_result = a_subject.PrimDisconnect();
        if ( l_result == 0 )
        {
             a_subject.SetStateToDisconnecting();
        }
*/
        return l_result;
}
 
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingConnectedState::CMrcpSignalingConnectedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingConnectedState::~CMrcpSignalingConnectedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingAvailable
// Input - signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingConnectedState::OnSignalingAvailable(AMrcpSignaling& a_subject)
{
	a_subject.SetStateToAvailable();
	return MrcpSuccess;
}  
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingAvailableState::CMrcpSignalingAvailableState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingAvailableState::~CMrcpSignalingAvailableState()
{

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Reinvite
// Input - signaling subject by reference
//         resource type
// Output - status from primreinvite call
//         part of the signaling state machine 
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::Reinvite(AMrcpSignaling& a_subject, MrcpResourceType a_resourceType)
{
	int l_result = a_subject.PrimReinvite(a_resourceType);
	if ( l_result == 0)
	{
		a_subject.SetStateToConnecting();
	}
	return l_result;
} 
////////////////////////////////////////////////////////////////////////// 
//
// Description - SignalingDisconnect
// Input - signaling object by reference
//         
// Output - result of call to PrimDisconnect
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::SignalingDisconnect(AMrcpSignaling& a_subject)
{
	int l_result = a_subject.PrimDisconnect();
	if ( l_result == 0 )
	{
		a_subject.SetStateToDisconnecting();
	}
	return l_result;
}  
//////////////////////////////////////////////////////////////////////////
//
// Description - Unsolicited Bye from MRCP Server
// Input - signaling object by reference
//
// Output - result of call to PrimUnsolicitedByeStop
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::OnSignalingUnsolicitedBye(AMrcpSignaling& a_subject)
{
        int l_result = a_subject.PrimByeOk();  // send OK for BYE

        return l_result;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Unsolicited Bye from MRCP Server
// Input - signaling object by reference
//
// Output - result of call to PrimUnsolicitedByeStop
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::OnSignalingUnsolicitedByeStop(AMrcpSignaling& a_subject)
{
        int l_result = 0;
        l_result = a_subject.PrimUnsolicitedByeStop();
        if ( l_result == 0 )
        {
                a_subject.SetStateToDisconnected();
        }

        return l_result;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Communications socket failed to MRCP Server
// Input - signaling object by reference
//
// Output - result of a failure in the sip signaling socket
//          part of the signaling state machine                
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::OnCommsFailed(AMrcpSignaling& a_subject)
{
        int l_result = a_subject.PrimOnCommsFailed();  // start shutting down the threads for session

/*        l_result = a_subject.PrimDisconnect();
        if ( l_result == 0 )
        {
            a_subject.SetStateToDisconnecting();
        }
*/
        return l_result;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnSignalingFailed
// Input - signaling object by reference
//
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingAvailableState::OnSignalingFailed(AMrcpSignaling& a_subject)
{
        a_subject.PrimOnSignalingFailed();
        return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingDisconnectingState::CMrcpSignalingDisconnectingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingDisconnectingState::~CMrcpSignalingDisconnectingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingDisconnected
// Input - signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingDisconnectingState::OnSignalingDisconnected(AMrcpSignaling& a_subject)
{
	a_subject.SetStateToDisconnected();
	return MrcpSuccess;
}
   
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingDisconnectedState::CMrcpSignalingDisconnectedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingDisconnectedState::~CMrcpSignalingDisconnectedState()
{

}	
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingDisconnected
// Input - Signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingDisconnectedState::SignalingStop(AMrcpSignaling& a_subject)
{
	a_subject.SetStateToStopping();
	int result = a_subject.PrimStop();
	return MrcpSuccess;
}   
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStoppingState::CMrcpSignalingStoppingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStoppingState::~CMrcpSignalingStoppingState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingStopped
// Input - signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingStoppingState::OnSignalingStopped(AMrcpSignaling& a_subject)
{
	a_subject.SetStateToStopped();
	int result = a_subject.PrimOnStopped();

	return MrcpSuccess;
}  
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStoppedState::CMrcpSignalingStoppedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CMrcpSignalingStoppedState::~CMrcpSignalingStoppedState()
{

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSignalingUninitialized
// Input - signaling object by reference
//         
// Output - success
//          part of the signaling state machine
///////////////////////////////////////////////////////////////////////////
int CMrcpSignalingStoppedState::OnSignalingUnintialized(AMrcpSignaling& a_subject)
{
	a_subject.SetStateToUninitialized();
	return MrcpSuccess;
}   
} //end namespace

