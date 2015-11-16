///////////////////////////////////////////////////////////////////////////////
//	File Name:		CSpeakCommands.h
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

#ifndef CSpeakCommands_H
#define CSpeakCommands_H

#define MRCP_ENDL "\r\n"
#define ENDL "\r"

#include <string>

#include "MrcpTasks.h"
#include "ClientInterfaceDefs.h"

namespace MrcpV2RefLib
{
	class AMrcpClient;
	class AMrcpResource;

	class CSpeakCommands
	{
		friend class CSpeechSynthesizer;
	public:
		CSpeakCommands(int a_version) : m_version (a_version)
		{
                        Speak_1 =
                                "SPEAK (2) MRCP/1.0" MRCP_ENDL
                                "(4)" 
                                "(6)" 
                                "Content-Type: (7)" MRCP_ENDL
                                "Content-Length: (8)" MRCP_ENDL MRCP_ENDL
                                "(9)";// MRCP_ENDL;

			Speak_2 =
				"MRCP/2.0 (1) SPEAK (2)" MRCP_ENDL 
				"Channel-Identifier: (3)" MRCP_ENDL 
				"(4)" 
//				"(5)"
				"(6)"
				"Content-Type: (7)" MRCP_ENDL 
				"Content-Length: (8)" MRCP_ENDL MRCP_ENDL
				"(9)";MRCP_ENDL;
			
                        Stop_1 =
                                "STOP (2) MRCP/1.0" MRCP_ENDL MRCP_ENDL;

			Stop_2 =
				"MRCP/2.0 (1) STOP (2)" MRCP_ENDL
				"Channel-Identifier:(4)" MRCP_ENDL MRCP_ENDL;

                        BargeIn_1 =
                                "BARGE-IN-OCCURRED (2) MRCP/1.0" MRCP_ENDL
                                "Proxy-Sync-Id: (3)" MRCP_ENDL MRCP_ENDL; 
			BargeIn_2 =
				"MRCP/2.0 (1) BARGE-IN-OCCURRED (2)" MRCP_ENDL
				"Channel-Identifier:(4)" MRCP_ENDL MRCP_ENDL;
                        Pause_1 = 
                                "PAUSE (2) MRCP/1.0" MRCP_ENDL MRCP_ENDL;
			Pause_2 =
				"MRCP/2.0 (1) PAUSE (2)" MRCP_ENDL
				"Channel-Identifier:(3)" MRCP_ENDL MRCP_ENDL;
                        Resume_1 =
                                "RESUME (2) MRCP/1.0" MRCP_ENDL MRCP_ENDL;

			Resume_2 =
				"MRCP/2.0 (1) RESUME (2)" MRCP_ENDL
				"Channel-Identifier:(3)" MRCP_ENDL MRCP_ENDL;
		};
		virtual ~CSpeakCommands(){ m_version = 0;};
	protected:

	private:
                std::string Speak_1;
		std::string Speak_2;
                std::string Stop_1;
		std::string Stop_2;
                std::string BargeIn_1;
		std::string BargeIn_2;
                std::string Pause_1;
		std::string Pause_2;
                std::string Resume_1;
		std::string Resume_2;
		int m_version;
	};

	///////////
	class CSpkSpeak : public MrcpTasks
	{
	public:
		CSpkSpeak(int a_cmdID, AMrcpResource* a_command, SPEAK_STRUCT* a_speakParams);
	    virtual ~CSpkSpeak();
		void Execute();

		int m_cmdID;
//		std::string m_killOnBargeIn;
                int m_killOnBargeIn;
		std::string m_voiceName;
		std::string m_speechLanguage;
		std::string m_contentType;
		std::string m_content;
		std::string m_contentID;

	protected:
	private:
		AMrcpResource* m_command;
		CSpkSpeak();
	};

	///////////
	class CSpkStop : public MrcpTasks
	{
	public:
		CSpkStop(int a_cmdID, AMrcpResource* a_command);
	    virtual ~CSpkStop();
		void Execute();

		int m_cmdID;
		std::string m_contentType;
		std::string m_contentID;
		std::string m_gramContent;

	protected:
	private:
		AMrcpResource* m_command;
		CSpkStop();
	};

	////////////////
	class CSpkBargeIn : public MrcpTasks
	{
	public:
		CSpkBargeIn(int a_cmdID, AMrcpResource* a_command);
	    virtual ~CSpkBargeIn();
		void Execute();

		int m_cmdID;
		std::string m_contentType;
		std::string m_contentID;
		std::string m_gramContent;

	protected:
	private:
		AMrcpResource* m_command;
		CSpkBargeIn();
	};

	///////////////////
	class CSpkPause : public MrcpTasks
	{
	public:
		CSpkPause(int a_cmdID, AMrcpResource* a_command);
	    virtual ~CSpkPause();
		void Execute();

		int m_cmdID;
		std::string m_contentType;
		std::string m_contentID;
		std::string m_gramContent;

	protected:
	private:
		AMrcpResource* m_command;
		CSpkPause();
	};

	/////////////////
	class CSpkResume : public MrcpTasks
	{
	public:
		CSpkResume(int a_cmdID, AMrcpResource* a_command);
	    virtual ~CSpkResume();
		void Execute();

		int m_cmdID;
		std::string m_contentType;
		std::string m_contentID;
		std::string m_gramContent;

	protected:
	private:
		AMrcpResource* m_command;
		CSpkResume();
	};



}//end namespace

#endif

