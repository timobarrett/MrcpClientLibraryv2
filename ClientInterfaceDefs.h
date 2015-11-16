///////////////////////////////////////////////////////////////////////////////
//	File Name:		ClientInterfaceDefs.h
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
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ClientInterfaceDefs_h
#define ClientInterfaceDefs_h


#ifdef _cplusplus
namespace MrcpV2RefLib
{
//  class CMrcpSession;
#else
  typedef int bool;
#endif

#define MrcpSuccess 0
#define MrcpError -1
#define MrcpNonRegCmd 9
#define SocketError -9999

typedef int MrcpSessionHandle; 
typedef int MrcpAudioHandle;


#define IP_ADDRESS_SIZE 50
#define SERVER_PORT_SIZE 5

  //class CMrcpSession;
//The following structure is used to return values to client application
//via callback processing

	typedef struct{
		int referenceNumber;
		int MrcpReturnValue;
		MrcpSessionHandle asrSessionHandle;
		MrcpSessionHandle ttsSessionHandle;
		char MrcpReturnPkt [5000];
                char MrcpSrvrAddress [65];
                int  MrcpSrvrRtpPort;
		void* MrcpCallBackHandle;
	}MRCP_RESULT_STRUCT;

typedef void(*MrcpCallBackFunction)(MRCP_RESULT_STRUCT*);
typedef void(*MrcpCallBackHandle)();

	typedef struct{
		MrcpCallBackFunction callbackFunction;
		MrcpCallBackHandle  callbackHandle;
	}MrcpCallBackInfo;

//The following enums are MRCP values used to identify the
//Server resources provided
	typedef enum  {
		speechrecog,
		dtmfrecog,
		speechsynth,
		basicsynth,
		recorder
	}MrcpResourceType;

// The following structure is used to configure
// MRCP resource Servers
	typedef struct{
		int   mrcpVersion;
		char* serverAddress;
		int   serverPort;
		MrcpSessionHandle sessionHandle;
                char* rtpCodec;
	//added the following  IPMS
		MrcpCallBackInfo callBackInfo;
	}MRCP_SERVER_CONFIG;

	typedef struct {
		char* clientAddress;
		int   clientPort;
                int   sockConnectBackoff;
                int   keepAliveInterval;
                int   keepAliveCnt;
		void*  sessionInfo;
		MRCP_SERVER_CONFIG *asrServerConfig;
		MRCP_SERVER_CONFIG *ttsServerConfig;
	}RESOURCE_CFG_STRUCT;

	static __inline void INIT_MRCP_SERVER_CONFIG(MRCP_SERVER_CONFIG *aMrcpServerCfg)
	{
		aMrcpServerCfg->callBackInfo.callbackHandle = 0;
		aMrcpServerCfg->callBackInfo.callbackFunction = 0;
		aMrcpServerCfg->mrcpVersion = 0;
		aMrcpServerCfg->serverAddress = "\n";
                aMrcpServerCfg->rtpCodec = "\n";
		aMrcpServerCfg->serverPort = 0;
		aMrcpServerCfg->sessionHandle = 0;
	}

	static __inline void INIT_RESOURCE_CFG_STRUCT(RESOURCE_CFG_STRUCT *aResourceCfg)
	{
		aResourceCfg->clientAddress = "\n";
        	aResourceCfg->sessionInfo = 0;
		aResourceCfg->clientPort = 0;
                aResourceCfg->sockConnectBackoff = 0;
                aResourceCfg->keepAliveInterval = 0;
                aResourceCfg->keepAliveCnt = 0;
		aResourceCfg->asrServerConfig = 0;
		aResourceCfg->ttsServerConfig = 0;
	}

// The following is used in the GetParams call
	typedef struct{
		int numParams;
		char* parameterName;
	}GET_PARAMETERS;

//The folowing is used in the SetParams call
	typedef struct{
		int numParams;
		char** parameterName;
		char** parameterValue;
	}SET_PARAMETERS;

//The following is used in the DefinGrammar client method
	typedef struct{
		int referenceNumber;
		char* contentType;
		char* contentID;
		char* gramContent;
		MrcpCallBackInfo callBack;
//		MrcpCallBackFunction callBack;
	}DEFINE_GRAMMAR_STRUCT;

//The following structure is used in the recognize client method
	typedef struct{
		int referenceNumber;
                int cancelQueue;
		int startInputTimers;
                int recTimeout;
		char* contentType;
		char* gramContent;
		MrcpCallBackInfo callBack;
//                MrcpCallBackFunction callBack;
	}RECOGNIZE_STRUCT;

// The following structure is used in the interpret client method
	typedef struct{
		int referenceNumber;
		char* interpretText;
		char* contentType;
		char* contentId;
		char* content;
		MrcpCallBackInfo callBack;
//                MrcpCallBackFunction callBack;
	}INTERPRET_STRUCT;

//The following structure is used in the speak client method
	typedef struct {
		int referenceNumber;
//		char* killOnBargeIn;
                int killOnBargeIn;
//		char* voiceName;
		char* speechLanguage;
		char* contentType;
		char* content;
		MrcpCallBackInfo callBack;
//                MrcpCallBackFunction callBack;
	} SPEAK_STRUCT;


#ifdef _cplusplus
} //end namespace
#endif
#endif
