///////////////////////////////////////////////////////////////////////////////
//	File Name:		CGenericCommands.cpp
//
//	Description:	 CGeneric class implementation
//					 Task queue processors for get/set params
//	Declared in this file:
//		none
//
//	Implemented in this file:
//		CGetParams::CGetParams(int a_cmdID, AMrcpResource* a_command, int a_version)
//		CGetParams::~CGetParams()
//		void CGetParams::Execute()
//		CSetParams::CSetParams(int a_cmdID, AMrcpResource* a_command, int a_version)
//		CSetParams::~CSetParams()
//		void CSetParams::Execute()
//
///////////////////////////////////////////////////////////////////////////////
//	Revisions:
//	Date		Initial 	Description
//	----		------- 	-----------
//
//
///////////////////////////////////////////////////////////////////////////////
#include "ClientInterfaceDefs.h"
#include "CGenericCommands.h"
#include "AMrcpResource.h"


namespace MrcpV2RefLib
{
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - command ID
//         command
//		   mrcp version
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
CGetParams::CGetParams(int a_cmdID, AMrcpResource* a_command, int a_version)
	:m_command(a_command)
	,m_cmdID(a_cmdID)
	,m_version(a_version)
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
CGetParams::~CGetParams()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Execute
// Input - None
//         
// Output - None
//          call command using state machine
///////////////////////////////////////////////////////////////////////////
void CGetParams::Execute()
{	
	m_command->GetParams(*m_command, *this);
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Constructor
// Input - command ID, 
//         command,
//		   mrcp version
// Output - None
//          
///////////////////////////////////////////////////////////////////////////
	CSetParams::CSetParams(int a_cmdID, AMrcpResource* a_command, int a_version)
		:m_command(a_command)
		,m_cmdID(a_cmdID)
		,m_version(a_version)
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
	CSetParams::~CSetParams()
{
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Execute
// Input - None
//         
// Output - None
//          call command using state machine
///////////////////////////////////////////////////////////////////////////
	void CSetParams::Execute()
{	
	m_command->SetParams(*m_command, *this);
}
//////////////////////////////////////////////////////////////////////////
//
// Description - Constructor
// Input - command ID,
//         command,
//                 mrcp version
// Output - None
//
///////////////////////////////////////////////////////////////////////////
        CCmdStop::CCmdStop(AMrcpResource* a_command)
                :m_command(a_command)
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
        CCmdStop::~CCmdStop()
{
}

//////////////////////////////////////////////////////////////////////////
//
// Description - Execute
// Input - None
//
// Output - None
//          call command using state machine
///////////////////////////////////////////////////////////////////////////
        void CCmdStop::Execute()
{
        m_command->StopSessionCmds(*m_command, *this);
}

} //end namespace

