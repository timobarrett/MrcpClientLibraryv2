///////////////////////////////////////////////////////////////////////////////
//	File Name:		AMrcpResourceState.cpp
//
//	Description:	AMrcpResourceState class implementation -
//				    Methods in this class are called if the command object is in a bad state
//
//	Declared in this file:
//		none
//
//	Implemented in this file:
//  AMrcpResourceState::AMrcpResourceState()
//	AMrcpResourceState::~AMrcpResourceState()
//	int AMrcpResourceState::Unitialized(AMrcpResource& a_state)
// 	int AMrcpResourceState::DefineGrammar(AMrcpResource& a_state,CRecDefineGrammar& a_task)
//	int AMrcpResourceState::Recognize(AMrcpResource& a_state,CRecRecognize& a_task)
//	int AMrcpResourceState::Interpret(AMrcpResource& a_state,CRecInterpret& a_task)
//  int AMrcpResourceState::GetResult(AMrcpResource& a_state,CRecGetResult& a_task)
//	int AMrcpResourceState::StartInputTimers(AMrcpResource& a_state,CRecStartInputTimers& a_task)
//	int AMrcpResourceState::StopRec(AMrcpResource& a_state,CRecStopRec& a_task)
//	int AMrcpResourceState::Idle(AMrcpResource& a_state)
//	int AMrcpResourceState::DefiningGrammar(AMrcpResource& a_state)
//	int AMrcpResourceState::GettingResult(AMrcpResource& a_state)
//	int AMrcpResourceState::Interpreting(AMrcpResource& a_state)
//	int AMrcpResourceState::StartingInputTimers(AMrcpResource& a_state)
//	int AMrcpResourceState::StoppingCommand(AMrcpResource& a_state)
//	int AMrcpResourceState::Recognizing(AMrcpResource& a_state)
//	int AMrcpResourceState::Speak(AMrcpResource& a_state,CSpkSpeak& a_task)
//  int AMrcpResourceState::Speaking(AMrcpResource& a_state)	
//	int AMrcpResourceState::StopSpeak(AMrcpResource& a_state,CSpkStop& a_task)
//  int AMrcpResourceState::StopSpeaking(AMrcpResource& a_state)
//  int AMrcpResourceState::Pause(AMrcpResource& a_state,CSpkPause& a_task)
//  int AMrcpResourceState::Pausing(AMrcpResource& a_state)
//  int AMrcpResourceState::Resume(AMrcpResource& a_state,CSpkResume& a_task)
//  int AMrcpResourceState::Resuming(AMrcpResource& a_state)
//  int AMrcpResourceState::BargeIn(AMrcpResource& a_state,CSpkBargeIn& a_task)
//  int AMrcpResourceState::BargingIn(AMrcpResource& a_state)
//  int AMrcpResourceState::Stopped(AMrcpResource& a_state)
//	int AMrcpResourceState::GetParams(AMrcpResource& a_state,CGetParams& a_task)
//	int AMrcpResourceState::SetParams(AMrcpResource& a_state,CSetParams& a_task)
//	void AMrcpResourceState::PerformBadStateCallback(int a_cmdID)		
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------

///////////////////////////////////////////////////////////////////////////////

#include "AMrcpResourceState.h"
#include "AMrcpResource.h"
#include "CRecognitionCommands.h"
#include "CSpeakCommands.h"
#include "CMrcpCmdIdMgr.h"
#include "CGenericCommands.h"


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
AMrcpResourceState::AMrcpResourceState()
{
	ClassName("AMrcpResourceState");
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
AMrcpResourceState::~AMrcpResourceState()
{

}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Unintialized
// Input - command passed by reference
//         
// Output - None
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Unintialized(AMrcpResource& a_state)
{
	Name("Uninitalized");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - DefineGrammar
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//         
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::DefineGrammar(AMrcpResource& a_state,CRecDefineGrammar& a_task)
{
	Name("DefineGrammar");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//         
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Recognize(AMrcpResource& a_state,CRecRecognize& a_task)
{
	Name("Recognize");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpret
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Interpret(AMrcpResource& a_state,CRecInterpret& a_task)
{
	Name("Interpret");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetResult
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//       
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::GetResult(AMrcpResource& a_state,CRecGetResult& a_task)
{
	Name("Get Result");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartInputTimers
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StartInputTimers(AMrcpResource& a_state,CRecStartInputTimers& a_task)
{
	Name("Start Input Timers");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StopRec
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StopRec(AMrcpResource& a_state,CRecStopRec& a_task)
{
	Name("Stop Rec");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - not in recognition state ");
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress(),"200 success ");

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Idle
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Idle(AMrcpResource& a_state)
{
	Name("Idle");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - DefiningGrammar
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::DefiningGrammar(AMrcpResource& a_state)
{
	Name("DefiningGrammar");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - GettingResult
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::GettingResult(AMrcpResource& a_state)
{
	Name("GettingResult");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpreting
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Interpreting(AMrcpResource& a_state)
{
	Name("Interpreting");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartingInputTimers
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StartingInputTimers(AMrcpResource& a_state)
{
	Name("StartingInputTimers");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StoppingCommand
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StoppingCommand(AMrcpResource& a_state)
{
	Name("StoppingCommand");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Recognizing(AMrcpResource& a_state)
{
	Name("Recognizing");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speak
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Speak(AMrcpResource& a_state,CSpkSpeak& a_task)
{
	Name("Speak");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speaking
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Speaking(AMrcpResource& a_state)
{
	Name("Speaking");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSpeak
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StopSpeak(AMrcpResource& a_state,CSpkStop& a_task)
{
	Name("StopSpeak");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress(),"200 success ");

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSpeaking
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StopSpeaking(AMrcpResource& a_state)
{
	Name("StopSpeaking");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Pause
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Pause(AMrcpResource& a_state,CSpkPause& a_task)
{
	Name("Pause");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Pausing
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Pausing(AMrcpResource& a_state)
{
	Name("Pausing");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Resume
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Resume(AMrcpResource& a_state,CSpkResume& a_task)
{
	Name("Resume");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Resuming
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Resuming(AMrcpResource& a_state)
{
	Name("Resuming");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - BargeIn
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::BargeIn(AMrcpResource& a_state,CSpkBargeIn& a_task)
{
	Name("BargeIn");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());
	return -1;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - BargingIn
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::BargingIn(AMrcpResource& a_state)
{
	Name("BargingIn");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Stopped
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::Stopped(AMrcpResource& a_state)
{
	Name("Stopped");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnBargeInComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnBargeInComplete(AMrcpResource& a_state)
{
	Name("OnBargeInComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnResumingComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnResumingComplete(AMrcpResource& a_state)
{
	Name("OnResuming Complete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnPausingComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnPausingComplete(AMrcpResource& a_state)
{
	Name("OnPausing Complete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStopSpeakComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnStopSpeakComplete(AMrcpResource& a_state)
{
	Name("OnStopSpeakComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSpeakingComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnSpeakingComplete(AMrcpResource& a_state)
{
	Name("OnSpeakingComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnDefineGrammarComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnDefineGrammarComplete(AMrcpResource& a_state)
{
	Name("OnDefineGrammarComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnRecognitionComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnRecognitionComplete(AMrcpResource& a_state)
{
	Name("OnRecognitionComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnInterpretingComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnInterpretingComplete(AMrcpResource& a_state)
{
	Name("OnInterpretingComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStartInputTimersComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnStartInputTimersComplete(AMrcpResource& a_state)
{
	Name("OnStartInputTimersComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnGettingResultComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnGettingResultComplete(AMrcpResource& a_state)
{
	Name("OnGettingResultComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnStopRecComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnStopRecComplete(AMrcpResource& a_state)
{
	Name("OnStopRecComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}	
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetParams
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::GetParams(AMrcpResource& a_state,CGetParams& a_task)
{
	Name("GetParams");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SetParams
// Input - command by reference
//         task by reference
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::SetParams(AMrcpResource& a_state,CSetParams& a_task)
{
	Name("SetParams");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	PerformBadStateCallback(a_task.m_cmdID,a_state.m_signalObj->AsrSessionHandle(), a_state.m_signalObj->TtsSessionHandle(),a_state.m_signalObj->ServerAddress());

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - SettingParams
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::SettingParams(AMrcpResource& a_state)
{
	Name("SettingParams");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);

	return -1;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OnSettingParamsComplete
// Input - command by reference
//         
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//		    from executing.
//          
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::OnSettingParamsComplete(AMrcpResource& a_state)
{
	Name("OnSettingParamsComplete");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
	return -1;
}
//////////////////////////////////////////////////////////////////////////
//
// Description - StopSessionCmds
// Input - command by reference
//
// Output - -1 indicating bad state
//          This method should only be called if the command state
//          machine is in a state that prevents the command primitive
//                  from executing.
//
///////////////////////////////////////////////////////////////////////////
int AMrcpResourceState::StopSessionCmds(AMrcpResource& a_state, CCmdStop& a_task)
{
        Name("StopSessionCmds");
        std::string l_state = TranslateState(a_state);
	CLogger::Instance()->Log( LOG_LEVEL_WARNING, *this, "Bad state - ",l_state);
        return -1;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - TranslateState
// Input - command by reference
//
// Output - text description of the state of the resource
//         
//          
//
///////////////////////////////////////////////////////////////////////////
std::string AMrcpResourceState::TranslateState(AMrcpResource& a_state)
{
        StateDesc("TranslateState");
        std::string l_transState;
        if(a_state.m_state == a_state.m_resourceUninitializedState)
           l_transState = "uninitialized";
        if(a_state.m_state == a_state.m_resourceIdleState)
           l_transState = "idle";
        if(a_state.m_state == a_state.m_resourceStoppedState)
           l_transState = "stopped";
        if(a_state.m_state == a_state.m_resourceDefiningGrammarState)
           l_transState = "defining grammar";
        if(a_state.m_state == a_state.m_resourceRecognizingState)
           l_transState = "recognizing";
        if(a_state.m_state == a_state.m_resourceInterpretingState)
           l_transState = "interpreting";
        if(a_state.m_state == a_state.m_resourceGettingResultState)
           l_transState = "getting result";
        if(a_state.m_state == a_state.m_resourceStartingInputTimersState)
           l_transState = "starting input timers";
        if(a_state.m_state == a_state.m_resourceStoppingRecState)
           l_transState = "stopping rec";
        if(a_state.m_state == a_state.m_resourceSpeakingState)
           l_transState = "speaking";
        if(a_state.m_state == a_state.m_resourceStoppingSpkState)
           l_transState = "stopping speak";
        if(a_state.m_state == a_state.m_resourcePausingState)
           l_transState = "pausing";
        if(a_state.m_state == a_state.m_resourcePausedState)
           l_transState = "paused";
        if(a_state.m_state == a_state.m_resourceResumingState)
           l_transState = "resuming";
        if(a_state.m_state == a_state.m_resourceBargingInState)
           l_transState = "barging in";
        if(a_state.m_state == a_state.m_resourceSettingParamsState)
           l_transState = "setting params";
        return l_transState;
}
	
////////////////////////////////////////////////////////////////////////// 
//
// Description - PerformBadStateCallback
// Input - command id, asr session handle, tts session handle
//         
// Output - None
//    This method reports the bad state back to the client application
//    via the callback mechanism
//
///////////////////////////////////////////////////////////////////////////
void AMrcpResourceState::PerformBadStateCallback(int a_cmdID, MrcpSessionHandle a_asrSessionHandle, MrcpSessionHandle a_ttsSessionHandle, std::string a_serverAddress, std::string a_statusMsg)
{
	MRCP_RESULT_STRUCT l_callResults;
	int l_stat = 0;
	MrcpCallBackInfo l_callBackInfo;
        std::string statusMessage;
//        MrcpCallBackFunction l_callBackInfo;

	l_callResults.MrcpReturnValue = -1;
	l_callResults.referenceNumber = a_cmdID;
	l_callResults.asrSessionHandle = a_asrSessionHandle;
	l_callResults.ttsSessionHandle = a_ttsSessionHandle;
        strncpy(l_callResults.MrcpSrvrAddress, a_serverAddress.c_str(), a_serverAddress.size()+1);
        statusMessage.erase();
	statusMessage = a_statusMsg + StateDesc();
	strncpy(l_callResults.MrcpReturnPkt,statusMessage.c_str(), statusMessage.size()+1);
        CLogger::Instance()->Log(LOG_LEVEL_INFO, *this,statusMessage);
//	MrcpCallBackFunction l_callBack = CMrcpCmdIdMgr::Instance()->GetCommandObj(a_cmdID);
	if(CMrcpCmdIdMgr::Instance()->GetCommandObj(a_cmdID, l_callBackInfo)==MrcpSuccess)
		CMrcpCmdIdMgr::Instance()->PerformCallbackProcessing(l_callResults,true,l_callBackInfo);
}
}//end namespace

