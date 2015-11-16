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

#ifndef CMrcpSendSignal_H
#define CMrcpSendSignal_H

#include "MrcpTasks.h"
#include <string>

namespace MrcpV2RefLib
{
	class CMrcpSendSignal : public MrcpTasks
	{
	public:       
		CMrcpSendSignal(AMrcpSignaling* a_signalObj);
		virtual ~CMrcpSendSignal();

	   void Execute();
	   void MessageBuffer(const std::string& a_messageBuffer) { m_messageBuffer = a_messageBuffer;}
	   const std::string& MessageBuffer() { return m_messageBuffer; }

	private:
		std::string m_messageBuffer;
	};
}//end namespace

#endif

