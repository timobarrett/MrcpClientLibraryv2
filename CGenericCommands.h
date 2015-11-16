///////////////////////////////////////////////////////////////////////////////
//	File Name:		CGenericMrcpCommands.cpp
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

#ifndef CGenericCommands_H
#define CGenericCommands_H

#include <string>
#include "MrcpTasks.h"

#define MRCP_ENDL "\r\n"

namespace MrcpV2RefLib
{
	class AMrcpResource;

	class CGenericCommands 
	{
		friend class CMrcpGeneric;

	public:
		CGenericCommands(int a_version) : m_version (a_version)
		{
                set_params1 =
                        "SET-PARAMS (2) MRCP/1.0" MRCP_ENDL;

		set_params2 =
			"MRCP/2.0 (1) SET-PARAMS (2)" MRCP_ENDL
			"Channel-Identifier: (3)" MRCP_ENDL;
        
                get_params1 =
                        "GET-PARAMS (2) MRCP/1.0" MRCP_ENDL;

		get_params2 =
			"MRCP/2.0 (1) GET-PARAMS (2)" MRCP_ENDL
			"Channel-Identifier: (3)" MRCP_ENDL ;  //2 MRCP_ENDL lines needed
	
		get_params_arguments2 =
			"(1):" MRCP_ENDL;

		set_params_arguments2 =
			"(1): (2)" MRCP_ENDL;

		};
		virtual ~CGenericCommands(){ m_version = 0;};

	protected:

	private:
                std::string set_params1;
		std::string set_params2;
                std::string get_params1;
		std::string get_params2;
		std::string get_params_arguments2;
		std::string set_params_arguments2;
		int m_version;
		AMrcpSignaling* m_signalObj;

	};
	/////////
	class CGetParams : public MrcpTasks
	{
		friend class CMrcpGeneric;
	public:
		CGetParams(int a_cmdID, AMrcpResource* a_command, int a_version);
	    virtual ~CGetParams();
		void Execute();

		int m_cmdID;
		GET_PARAMETERS m_parameters;

	protected:
	private:
		AMrcpResource* m_command;
		CGetParams();
		int m_version;
	};
	/////////
	class CSetParams : public MrcpTasks
	{
		friend class CMrcpGeneric;
	public:
		CSetParams(int a_cmdID, AMrcpResource* a_command, int a_version);
	    virtual ~CSetParams();
		void Execute();

		int m_cmdID;
		SET_PARAMETERS m_parameters;

	protected:
	private:
		AMrcpResource* m_command;
		CSetParams();
		int m_version;
	};
        /////////
        class CCmdStop : public MrcpTasks
        {
                friend class CMrcpGeneric;
        public:
                CCmdStop(AMrcpResource* a_command);
            virtual ~CCmdStop();
                void Execute();


        protected:
        private:
                AMrcpResource* m_command;
                CCmdStop();
        };

}//end namespace

#endif

