///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpResourceState.cpp
//
//	Description:	CMrcpResourceState class implementation
//					Part of command state machine
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CMrcpResourceUninitializedState::CMrcpResourceUninitializedState()
//		CMrcpResourceUninitializedState::~CMrcpResourceUninitializedState()
//		int CMrcpResourceUninitializedState::OnStarted(AMrcpResource& a_subject)
//		CMrcpResourceIdleState::CMrcpResourceIdleState()
//		CMrcpResourceIdleState::~CMrcpResourceIdleState()
//		int CMrcpResourceIdleState::DefineGrammar(AMrcpResource& a_command,CRecDefineGrammar& a_task)
//		int CMrcpResourceIdleState::Recognize (AMrcpResource& a_command,CRecRecognize& a_task)
//		int CMrcpResourceIdleState::Interpret (AMrcpResource& a_command,CRecInterpret& a_task)
//		int CMrcpResourceRecognizingState::StartInputTimers (AMrcpResource& a_command,CRecStartInputTimers& a_task)
//		int CMrcpResourceRecognizingState::GetResult (AMrcpResource& a_command,CRecGetResult& a_task)
//		int CMrcpResourceRecognizingState::StopRec (AMrcpResource& a_command,CRecStopRec& a_task)
//		int CMrcpResourceIdleState::Speak (AMrcpResource& a_command,CSpkSpeak& a_task)
//		int CMrcpResourceSpeakingState::Pause (AMrcpResource& a_command,CSpkPause& a_task)
//		int CMrcpResourcePausedState::Resume (AMrcpResource& a_command,CSpkResume& a_task)
//		int CMrcpResourceSpeakingState::BargeIn (AMrcpResource& a_command,CSpkBargeIn& a_task)
//		int CMrcpResourceSpeakingState::StopSpeak (AMrcpResource& a_command,CSpkStop& a_task)
//		int CMrcpResourceIdleState::GetParams (AMrcpResource& a_command, CGetParams& a_task)
//		int CMrcpResourceIdleState::SetParams (AMrcpResource& a_command, CSetParams& a_task)
//		CMrcpResourceDefiningGrammarState::CMrcpResourceDefiningGrammarState()
//		CMrcpResourceDefiningGrammarState::~CMrcpResourceDefiningGrammarState()
//		int CMrcpResourceDefiningGrammarState::OnDefineGrammarComplete(AMrcpResource& a_subject)
//		CMrcpResourceRecognizingState::CMrcpResourceRecognizingState()
//		CMrcpResourceRecognizingState::~CMrcpResourceRecognizingState()
//		int CMrcpResourceRecognizingState::OnRecognitionComplete(AMrcpResource& a_subject)
//		CMrcpResourceInterpretingState::CMrcpResourceInterpretingState()
//		CMrcpResourceInterpretingState::~CMrcpResourceInterpretingState()
//		int CMrcpResourceInterpretingState::OnInterpretingComplete(AMrcpResource& a_subject)
//		CMrcpResourceGettingResultState::CMrcpResourceGettingResultState()
//		CMrcpResourceGettingResultState::~CMrcpResourceGettingResultState()
//		int CMrcpResourceGettingResultState::OnGettingResultComplete(AMrcpResource& a_subject)
//		CMrcpResourceStartingInputTimersState::CMrcpResourceStartingInputTimersState()
//		CMrcpResourceStartingInputTimersState::~CMrcpResourceStartingInputTimersState()
//		int CMrcpResourceStartingInputTimersState::OnStartInputTimersComplete(AMrcpResource& a_subject)
//		CMrcpResourceStoppingRecState::CMrcpResourceStoppingRecState()
//		CMrcpResourceStoppingRecState::~CMrcpResourceStoppingRecState()
//		int CMrcpResourceStoppingRecState::OnStopRecComplete(AMrcpResource& a_subject)
//		CMrcpResourceSpeakingState::CMrcpResourceSpeakingState()
//		CMrcpResourceSpeakingState::~CMrcpResourceSpeakingState()
//		int CMrcpResourceSpeakingState::OnSpeakingComplete(AMrcpResource& a_subject)
//		CMrcpResourceStoppingSpeakState::CMrcpResourceStoppingSpeakState()
//		CMrcpResourceStoppingSpeakState::~CMrcpResourceStoppingSpeakState()
//		int CMrcpResourceStoppingSpeakState::OnStopSpeakComplete(AMrcpResource& a_subject)
//		CMrcpResourcePausingState::CMrcpResourcePausingState()
//		CMrcpResourcePausingState::~CMrcpResourcePausingState()
//		int CMrcpResourcePausingState::OnPausingComplete(AMrcpResource& a_subject)
//		CMrcpResourceResumingState::CMrcpResourceResumingState()
//		CMrcpResourceResumingState::~CMrcpResourceResumingState()
//		int CMrcpResourceResumingState::OnResumingComplete(AMrcpResource& a_subject)
//		CMrcpResourceBargingInState::CMrcpResourceBargingInState()
//		CMrcpResourceBargingInState::~CMrcpResourceBargingInState()
//		int CMrcpResourceBargingInState::OnBargeInComplete(AMrcpResource& a_subject)
//		CMrcpResourceStoppedState::CMrcpResourceStoppedState()
//		CMrcpResourceStoppedState::~CMrcpResourceStoppedState()
//		int CMrcpResourceStoppedState::OnStopped(AMrcpResource& a_subject)
///////////////////////////////////////////////////////////////////////////////
//          The CMrcpSession module instantiates a command object & adds to task queue.
//             Task queue process comand calling objects execut method
//             Abstract AMrcpResource method called from the command objects execute method
//             State pointer calls command
//
//			   if state permits command
//					command state object calls primitive to execute command
//				else
//			        AMrcpCommandState method called to report bad state     
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
///////////////////////////////////////////////////////////////////////////////

#include "CMrcpResourceState.h"
#include "ClientInterfaceDefs.h"
#include "AMrcpResource.h"

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
CMrcpResourceUninitializedState::CMrcpResourceUninitializedState()
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
CMrcpResourceUninitializedState::~CMrcpResourceUninitializedState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStarted
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceUninitializedState::OnStarted(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
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
CMrcpResourceIdleState::CMrcpResourceIdleState()
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

CMrcpResourceIdleState::~CMrcpResourceIdleState()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - DefineGrammar
// Input - resource object
//         task object
// Output - success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::DefineGrammar(AMrcpResource& a_command,CRecDefineGrammar& a_task)
{
	if(a_command.PrimDefineGrammar(a_task) != -1)
		a_command.SetStateToDefiningGrammar();
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - Resource Object
//         Task Object
// Output - Success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::Recognize (AMrcpResource& a_command,CRecRecognize& a_task)
{
	if(a_command.PrimRecognize(a_task) != -1)
		a_command.SetStateToRecognizing();
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpret
// Input - resource object 
//         task object
// Output - success
//            
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::Interpret (AMrcpResource& a_command,CRecInterpret& a_task)
{
	if(a_command.PrimInterpret(a_task) != -1)
		a_command.SetStateToInterpreting();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - GetResult
// Input - Resource Object
//         Task Object
// Output - success
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceIdleState::GetResult (AMrcpResource& a_command,CRecGetResult& a_task)
{
        if(a_command.PrimGetResult(a_task) != -1)
                a_command.SetStateToGettingResult();
        return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StartInputTimers
// Input - Resource Object
//         Task Object
// Output - Success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceRecognizingState::StartInputTimers (AMrcpResource& a_command,CRecStartInputTimers& a_task)
{
	if(a_command.PrimStartInputTimers(a_task) != -1)
		a_command.SetStateToStartingInputTimers();
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetResult
// Input - Resource Object
//         Task Object
// Output - success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceRecognizingState::GetResult (AMrcpResource& a_command,CRecGetResult& a_task)
{
	if(a_command.PrimGetResult(a_task) != -1)
		a_command.SetStateToGettingResult();
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopRec
// Input - resource object
//         task object
// Output - success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceRecognizingState::StopRec (AMrcpResource& a_command,CRecStopRec& a_task)
{
	if(a_command.PrimStopRec(a_task) != -1)
		a_command.SetStateToStoppingRec();
	return MrcpSuccess;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - GetParams
// Input - resource object
//         task object
// Output - success
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::GetParams (AMrcpResource& a_command, CGetParams& a_task)
{
	if(a_command.PrimGetParams(a_task) != -1)
	{;}
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SetParams
// Input - resource object
//         task object
// Output - Success
//        
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::SetParams (AMrcpResource& a_command, CSetParams& a_task)
{
	if(a_command.PrimSetParams(a_task) != -1)
		a_command.SetStateToSettingParams();

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
CMrcpResourceSettingParamsState::CMrcpResourceSettingParamsState()
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
CMrcpResourceSettingParamsState::~CMrcpResourceSettingParamsState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSettingParamsComplete
// Input - Resource Object
//         
// Output - Success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceSettingParamsState::OnSettingParamsComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceSettingParamsState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceDefiningGrammarState::CMrcpResourceDefiningGrammarState()
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
CMrcpResourceDefiningGrammarState::~CMrcpResourceDefiningGrammarState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnDefineGrammarComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceDefiningGrammarState::OnDefineGrammarComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - StopSessionCmds
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceDefiningGrammarState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceRecognizingState::CMrcpResourceRecognizingState()
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
CMrcpResourceRecognizingState::~CMrcpResourceRecognizingState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnRecognitionComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceRecognizingState::OnRecognitionComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceRecognizingState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceInterpretingState::CMrcpResourceInterpretingState()
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
CMrcpResourceInterpretingState::~CMrcpResourceInterpretingState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnInterpretingComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceInterpretingState::OnInterpretingComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
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
CMrcpResourceGettingResultState::CMrcpResourceGettingResultState()
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
CMrcpResourceGettingResultState::~CMrcpResourceGettingResultState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnGettingResultComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceGettingResultState::OnGettingResultComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
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
CMrcpResourceStartingInputTimersState::CMrcpResourceStartingInputTimersState()
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
CMrcpResourceStartingInputTimersState::~CMrcpResourceStartingInputTimersState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStartingInputTimersComplete
// Input - Resource Object
//         
// Output - Success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceStartingInputTimersState::OnStartInputTimersComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToRecognizing();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceStartingInputTimersState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
        return MrcpSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - StopRec
// Input - resource object
//         task object
// Output - success
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceStartingInputTimersState::StopRec (AMrcpResource& a_command,CRecStopRec& a_task)
{
        if(a_command.PrimStopRec(a_task) != -1)
                a_command.SetStateToStoppingRec();
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
CMrcpResourceStoppingRecState::CMrcpResourceStoppingRecState()
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
CMrcpResourceStoppingRecState::~CMrcpResourceStoppingRecState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStoppingRecComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceStoppingRecState::OnStopRecComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speak
// Input - resource object 
//         task object
// Output - success
//           
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceIdleState::Speak (AMrcpResource& a_command,CSpkSpeak& a_task)
{
	if(a_command.PrimSpeak(a_task) != -1)
		a_command.SetStateToSpeaking();
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Pause
// Input - resource object
//         task object
// Output - success
//               
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceSpeakingState::Pause (AMrcpResource& a_command,CSpkPause& a_task)
{
	if(a_command.PrimPause(a_task) != -1)
		a_command.SetStateToPausing();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceSpeakingState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourcePausedState::CMrcpResourcePausedState()
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
CMrcpResourcePausedState::~CMrcpResourcePausedState()
{
}


////////////////////////////////////////////////////////////////////////// 
//
// Description - Resume
// Input - resource object
//         task object
// Output - success
//                 
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourcePausedState::Resume (AMrcpResource& a_command,CSpkResume& a_task)
{
	if(a_command.PrimResume(a_task) != -1)
		a_command.SetStateToSpeaking();
	return MrcpSuccess;
}


////////////////////////////////////////////////////////////////////////// 
//
// Description - BargeIn
// Input - resource object
//         task object
// Output - success
//    
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceSpeakingState::BargeIn (AMrcpResource& a_command,CSpkBargeIn& a_task)
{
	if(a_command.PrimBargeIn(a_task) != -1)
		a_command.SetStateToBargingIn();
	return MrcpSuccess;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SpkStop
// Input - resource object
//         task object
// Output - success
//               
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceSpeakingState::StopSpeak (AMrcpResource& a_command,CSpkStop& a_task)
{
	if(a_command.PrimStopSpk(a_task) != -1)
		a_command.SetStateToStoppingSpk();
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
CMrcpResourceSpeakingState::CMrcpResourceSpeakingState()
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
CMrcpResourceSpeakingState::~CMrcpResourceSpeakingState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSpeakComplete
// Input - resource object
//        
// Output - Success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceSpeakingState::OnSpeakingComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
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
CMrcpResourceStoppingSpeakState::CMrcpResourceStoppingSpeakState()
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
CMrcpResourceStoppingSpeakState::~CMrcpResourceStoppingSpeakState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStoppingSpeakComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceStoppingSpeakState::OnStopSpeakComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
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
CMrcpResourcePausingState::CMrcpResourcePausingState()
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
CMrcpResourcePausingState::~CMrcpResourcePausingState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnPausingComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourcePausingState::OnPausingComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToPaused();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourcePausingState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceResumingState::CMrcpResourceResumingState()
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
CMrcpResourceResumingState::~CMrcpResourceResumingState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnResumingComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceResumingState::OnResumingComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToSpeaking();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceResumingState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceBargingInState::CMrcpResourceBargingInState()
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
CMrcpResourceBargingInState::~CMrcpResourceBargingInState()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - OnBargingInComplete
// Input - resource object
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////	
int CMrcpResourceBargingInState::OnBargeInComplete(AMrcpResource& a_subject)
{
	a_subject.SetStateToIdle();
	return MrcpSuccess;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceBargingInState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
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
CMrcpResourceStoppedState::CMrcpResourceStoppedState()
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
CMrcpResourceStoppedState::~CMrcpResourceStoppedState()
{
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStopped
// Input - resource subject by reference
//         
// Output - success
//          
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceStoppedState::OnStopped(AMrcpResource& a_subject)
{
	a_subject.SetStateToUninitialized();
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
CMrcpResourceStoppingState::CMrcpResourceStoppingState()
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
CMrcpResourceStoppingState::~CMrcpResourceStoppingState()
{
}
//////////////////////////////////////////////////////////////////////////
//
// Description - OnStopped
// Input - resource subject by reference
//
// Output - success
//
///////////////////////////////////////////////////////////////////////////
int CMrcpResourceStoppingState::StopSessionCmds(AMrcpResource& a_subject)
{
        a_subject.SetStateToIdle();
        return MrcpSuccess;
}

}//end namespace

