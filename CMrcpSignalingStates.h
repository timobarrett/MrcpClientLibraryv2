///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSignalingStates.h
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

#ifndef CMrcpSignalingStates_h
#define CMrcpSignalingStates_h

#include "AMrcpSignalingState.h"
#include <string>

namespace MrcpV2RefLib
{
	class CMrcpSignalingUninitializedState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingUninitializedState();
		virtual ~CMrcpSignalingUninitializedState();

		// Accessors
		char* Description()
		{
			return "Uninitialized";
		};

		// Operations
		int Start(AMrcpSignaling& subject);
                int OnSignalingFailed(AMrcpSignaling& a_subject);
	};

//////
	class CMrcpSignalingStartingState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingStartingState();
		virtual ~CMrcpSignalingStartingState();

		// Accessors
		char* Description()
		{
			return "Starting";
		};

		// Operations
		int OnSignalingStarted(AMrcpSignaling& a_subject);
                int OnSignalingFailed(AMrcpSignaling& a_subject);
	};

//////
	class CMrcpSignalingStartedState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingStartedState();
		virtual ~CMrcpSignalingStartedState();

		// Accessors
		char* Description()
		{
			return "Started";
		};

		// Operations
		int Connect(AMrcpSignaling& a_subject);

		int OnSignalingConnecting(AMrcpSignaling& a_subject);
	};
///////////////////
	class CMrcpSignalingConnectingState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingConnectingState();
		virtual ~CMrcpSignalingConnectingState();

		// Accessors
		char* Description()
		{
			return "Connecting";
		};

		// Operations
		int OnSignalingConnected(AMrcpSignaling& a_subject);
                int OnSignalingFailed(AMrcpSignaling& a_subject);
		int SignalingAck(AMrcpSignaling& a_subject);
                int OnSignalingRedirect(AMrcpSignaling& a_subject);
                int OnCommsFailed(AMrcpSignaling& a_subject);
                int ConnectCmdSocket(AMrcpSignaling& a_subject, int a_cmdSocket);
	};

/////////////////////
	class CMrcpSignalingConnectedState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingConnectedState();
		virtual ~CMrcpSignalingConnectedState();

		// Accessors
		char* Description()
		{
			return "Connected";
		};

		// Operations
		int OnSignalingAvailable(AMrcpSignaling& a_subject);
	};

/////
	class CMrcpSignalingAvailableState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingAvailableState();
		virtual ~CMrcpSignalingAvailableState();

		// Accessors
		char* Description()
		{
			return "Available";
		};

		// Operations
		int Reinvite(AMrcpSignaling& subject, MrcpResourceType a_resourceType);
		int SignalingDisconnect(AMrcpSignaling& a_subject);
                int OnSignalingUnsolicitedBye(AMrcpSignaling& a_subject);
                int OnSignalingUnsolicitedByeStop(AMrcpSignaling& a_subject);
                int OnCommsFailed(AMrcpSignaling& a_subject);
	        int OnSignalingFailed(AMrcpSignaling& a_subject);
	};

//////////////
	class CMrcpSignalingDisconnectingState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingDisconnectingState();
		virtual ~CMrcpSignalingDisconnectingState();

		// Accessors
		char* Description()
		{
			return "Disconnecting";
		};

		// Operation&
		int OnSignalingDisconnected(AMrcpSignaling& a_subject);
	};
//////////////
	class CMrcpSignalingDisconnectedState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingDisconnectedState();
		virtual ~CMrcpSignalingDisconnectedState();

		// Accessors
		char* Description()
		{
			return "Disconnected";
		};

		// Operation&
		int SignalingStop(AMrcpSignaling& a_subject);
	};

//////////////
	class CMrcpSignalingStoppingState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingStoppingState();
		virtual ~CMrcpSignalingStoppingState();

		// Accessors
		char* Description()
		{
			return "Stopping";
		};

		// Operation&
		int OnSignalingStopped(AMrcpSignaling& a_subject);
	};
////////
	//////////////
	class CMrcpSignalingStoppedState : public AMrcpSignalingState
	{
	public:
		CMrcpSignalingStoppedState();
		virtual ~CMrcpSignalingStoppedState();

		// Accessors
		char* Description()
		{
			return "Stopped";
		};

		// Operation&
		int OnSignalingUnintialized(AMrcpSignaling& a_subject); 
	};

} //end namespace
#endif

