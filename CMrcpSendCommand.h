///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSendCommand.h
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

#ifndef CMrcpSendCommand_H
#define CMrcpSendCommand_H

#include "MrcpTasks.h"
#include <string>

namespace MrcpV2RefLib
{
	class CMrcpSendCommand : public MrcpTasks
	{
	public:       
		CMrcpSendCommand(AMrcpSignaling* a_signalObj);
		virtual ~CMrcpSendCommand();

	   void Execute();
	   void MessageBuffer(const std::string& a_messageBuffer) { m_messageBuffer = a_messageBuffer;}
	   const std::string& MessageBuffer() { return m_messageBuffer; }
           void ResourceType(const std::string& a_resourceType) {m_resourceType = a_resourceType;};
           const std::string& ResourceType() {return m_resourceType;};

	private:
	   std::string m_messageBuffer;
           std::string m_resourceType;      
	};
}//end namespace

#endif

