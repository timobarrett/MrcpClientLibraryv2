///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpResourceState.h
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


#ifndef CMrcpResourceState_H
#define CMrcpResourceState_H

#include "AMrcpResourceState.h"

namespace MrcpV2RefLib
{
	class CMrcpResourceUninitializedState: public AMrcpResourceState
	{
	public:
		CMrcpResourceUninitializedState();
		virtual ~CMrcpResourceUninitializedState();

		char* Description()
		{
			return "Uninitialized";
		};
		//operations
		int OnStarted(AMrcpResource& a_subject);
	};


	class CMrcpResourceIdleState: public AMrcpResourceState
	{
	public:
		CMrcpResourceIdleState();
		virtual ~CMrcpResourceIdleState();

		char* Description()
		{
			return "Idle";
		};
		//operations
		int DefineGrammar(AMrcpResource& a_command,CRecDefineGrammar& a_task);
		int Recognize(AMrcpResource& a_command,CRecRecognize& a_task);
		int Interpret(AMrcpResource& a_command,CRecInterpret& a_task);
		int Speak(AMrcpResource& a_command,CSpkSpeak& a_task);
		int GetParams(AMrcpResource& a_command, CGetParams& a_task);
		int GetResult(AMrcpResource& a_command,CRecGetResult& a_task);
		int SetParams(AMrcpResource& a_command, CSetParams& a_task);
	};

	class CMrcpResourceDefiningGrammarState: public AMrcpResourceState
	{
	public:
		CMrcpResourceDefiningGrammarState();
		virtual ~CMrcpResourceDefiningGrammarState();

		char* Description()
		{
			return "Defining Grammar";
		};
		//operations
		int OnDefineGrammarComplete(AMrcpResource& a_subject);
                int StopSessionCmds (AMrcpResource& a_subject);

	};

	class CMrcpResourceRecognizingState: public AMrcpResourceState
	{
	public:
		CMrcpResourceRecognizingState();
		virtual ~CMrcpResourceRecognizingState();

		char* Description()
		{
			return "Recognizing";
		};
		//operations 
		int OnRecognitionComplete (AMrcpResource& a_subject);
		int GetResult(AMrcpResource& a_command,CRecGetResult& a_task);
		int StopRec(AMrcpResource& a_command,CRecStopRec& a_task);
		int StartInputTimers(AMrcpResource& a_command,CRecStartInputTimers& a_task);
                int StopSessionCmds (AMrcpResource& a_subject);

	};

	class CMrcpResourceInterpretingState: public AMrcpResourceState
	{
	public:
			CMrcpResourceInterpretingState();
		virtual ~CMrcpResourceInterpretingState();

		char* Description()
		{
			return "Interpreting";
		};
		//operations 
		int OnInterpretingComplete (AMrcpResource& a_subject);
	};
	class CMrcpResourceStartingInputTimersState: public AMrcpResourceState
	{
	public:
		CMrcpResourceStartingInputTimersState();
		virtual ~CMrcpResourceStartingInputTimersState();

		char* Description()
		{
			return "Starting Input Timers";
		};
		//operations 
		int OnStartInputTimersComplete (AMrcpResource& a_subject);
		int StopRec(AMrcpResource& a_command,CRecStopRec& a_task);
                int StopSessionCmds (AMrcpResource& a_subject);

	};
	class CMrcpResourceGettingResultState: public AMrcpResourceState
	{
	public:
		CMrcpResourceGettingResultState();
		virtual ~CMrcpResourceGettingResultState();

		char* Description()
		{
			return "Getting Result";
		};
		//operations 
		int OnGettingResultComplete (AMrcpResource& a_subject);
	};
	class CMrcpResourceStoppingRecState: public AMrcpResourceState
	{
	public:
		CMrcpResourceStoppingRecState();
		virtual ~CMrcpResourceStoppingRecState();

		char* Description()
		{
			return "StoppingRec";
		};
		//operations 
		int OnStopRecComplete (AMrcpResource& a_subject);

	};

	class CMrcpResourceSpeakingState: public AMrcpResourceState
	{
	public:
		CMrcpResourceSpeakingState();
		virtual ~CMrcpResourceSpeakingState();

		char* Description()
		{
			return "Speaking";
		};
		//operations 
                int StopSessionCmds (AMrcpResource& a_subject);
		int OnSpeakingComplete (AMrcpResource& a_subject);
		int StopSpeak(AMrcpResource& a_command,CSpkStop& a_task);
		int Pause(AMrcpResource& a_command,CSpkPause& a_task);
		int BargeIn(AMrcpResource& a_command,CSpkBargeIn& a_task);

	};

	class CMrcpResourceStoppingSpeakState: public AMrcpResourceState
	{
	public:
		CMrcpResourceStoppingSpeakState();
		virtual ~CMrcpResourceStoppingSpeakState();

		char* Description()
		{
			return "Stopping Speak";
		};
		//operations 
		int OnStopSpeakComplete (AMrcpResource& a_subject);

	};

	class CMrcpResourcePausingState: public AMrcpResourceState
	{
	public:
		CMrcpResourcePausingState();
		virtual ~CMrcpResourcePausingState();

		char* Description()
		{
			return "Pausing";
		};
		//operations 
		int OnPausingComplete (AMrcpResource& a_subject);
                int StopSessionCmds (AMrcpResource& a_subject);

	};
	class CMrcpResourcePausedState: public AMrcpResourceState
	{
	public:
		CMrcpResourcePausedState();
		virtual ~CMrcpResourcePausedState();

		char* Description()
		{
			return "Paused";
		};
		//operations 
		int Resume(AMrcpResource& a_command,CSpkResume& a_task);

	};
	class CMrcpResourceResumingState: public AMrcpResourceState
	{
	public:
		CMrcpResourceResumingState();
		virtual ~CMrcpResourceResumingState();

		char* Description()
		{
			return "Resuming";
		};
		//operations 
		int OnResumingComplete (AMrcpResource& a_subject);
                int StopSessionCmds (AMrcpResource& a_subject);

	};

	class CMrcpResourceBargingInState: public AMrcpResourceState
	{
	public:
		CMrcpResourceBargingInState();
		virtual ~CMrcpResourceBargingInState();

		char* Description()
		{
			return "Barging In";
		};
		//operations 
		int OnBargeInComplete (AMrcpResource& a_subject);
                int StopSessionCmds (AMrcpResource& a_subject);

	};
////
	class CMrcpResourceSettingParamsState: public AMrcpResourceState
	{
	public:
		CMrcpResourceSettingParamsState();
		virtual ~CMrcpResourceSettingParamsState();

		char* Description()
		{
			return "Setting Params";
		};
		//operations 
		int OnSettingParamsComplete (AMrcpResource& a_subject);
                int StopSessionCmds (AMrcpResource& a_subject);

	};
////
	class CMrcpResourceStoppedState: public AMrcpResourceState
	{
	public:
		CMrcpResourceStoppedState();
		virtual ~CMrcpResourceStoppedState();

		char* Description()
		{
			return "Stopped";
		};
		//operations
		int OnStopped (AMrcpResource& a_subject);
	};
////
        class CMrcpResourceStoppingState: public AMrcpResourceState
        {
        public:
                CMrcpResourceStoppingState();
                virtual ~CMrcpResourceStoppingState();

                char* Description()
                {
                        return "Stopping";
                };
                //operations
                int StopSessionCmds (AMrcpResource& a_subject);
        };


}//end namespace
#endif

