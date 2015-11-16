///////////////////////////////////////////////////////////////////////////////
//	File Name:		MrcpClientEvents.h
//
//	Description:	Defines all the event IDs for the MRCP Client Library
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

#ifndef MrcpClientEvents_H
#define MrcpClientEvents_H

#define SIGNALING_BASE			          0xE0000000
#define SIGNALING_CONNECTED		          SIGNALING_BASE + 0x00000001
#define SIGNALING_DISCONNECTED            SIGNALING_BASE + 0x00000002
#define SIGNALING_PROTOCOL_CONNECTED      SIGNALING_BASE + 0x00000003
#define SIGNALING_PROTOCOL_DISCONNECTED   SIGNALING_BASE + 0x00000004
#define MRCP_SERVER_UNREACHABLE           SIGNALING_BASE + 0x00000005
#define SIGNALING_PROTOCOL_UNSOLICT_BYE   SIGNALING_BASE + 0x00000006 
#define SESSIONS_STOPPED                  SIGNALING_BASE + 0x00000007
#define SESSION_MGR_STOPPED               SIGNALING_BASE + 0x00000008
#define SIGNALING_PROTOCOL_FAILED         SIGNALING_BASE + 0x00000009

#define COMMAND_BASE                      0xE0000020
#define COMMAND_COMPLETE                  COMMAND_BASE + 0x00000001
#define RECOG_COMMAND_COMPLETE            COMMAND_BASE + 0x00000002
#define SYNTH_COMMAND_COMPLETE            COMMAND_BASE + 0x00000003
#define SYNTH_IDLE_STATE_REACHED          COMMAND_BASE + 0x00000004
#define RECOG_IDLE_STATE_REACHED          COMMAND_BASE + 0x00000005
#define SESSION_COMPLETE                  COMMAND_BASE + 0x00000006
#define COMMAND_SOCKET_FAILED             COMMAND_BASE + 0x00000007
//#define COMMAND_IN_PROGRESS				  COMMAND_BASE + 0x00000002

#endif //MrcpClientEvents_H
