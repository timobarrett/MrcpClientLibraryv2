///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSystemCInterface.h
//
//	Description:	CMrcpSystemCInterface class implementation
//				 Implements the 'C' interface to the MRCP Client Library
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
#ifndef CMrcpSystemCInterface_H
#define CMrcpSystemCInterface_H

//#include "CMrcpSystem.h"  -- causes cstdio errors
#include "ClientInterfaceDefs.h"

//namespace MrcpV2RefLib
//{
//	class CMrcpSystem;
#ifdef _cplusplus
typedef MrcpV2RefLib::RESOURCE_CFG_STRUCT RESOURCE_CFG_STRUCT;
typedef MrcpV2RefLib::DEFINE_GRAMMAR_STRUCT DEFINE_GRAMMAR_STRUCT;
typedef MrcpV2RefLib::RECOGNIZE_STRUCT RECOGNIZE_STRUCT;
typedef MrcpV2RefLib::INTERPRET_STRUCT INTERPRET_STRUCT;
typedef MrcpV2RefLib::SPEAK_STRUCT SPEAK_STRUCT;
typedef MrcpV2RefLib::GET_PARAMETERS GET_PARAMETERS;
typedef MrcpV2RefLib::SET_PARAMETERS SET_PARAMETERS;
typedef MrcpV2RefLib::MrcpSessionHandle MrcpSessionHandle;
typedef MrcpV2RefLib::MrcpCallBackFunction MrcpCallBackFunction;
typedef MrcpV2RefLib::MrcpCallBackInfo MrcpCallBackInfo;
typedef MrcpV2RefLib::MrcpResourceType MrcpResourceType;
#endif

typedef void CMrcpInterface;
//variables that are needed for interface

//the following are the 'C' interfaces
#ifdef _cplusplus
extern "C"
{
#endif
		CMrcpInterface* Start(int a_logLevel);
                int SetCommonCfg(char* a_clientAddress,int a_sockConBckOff,int a_keepAlveInt,int a_keepAlveCnt,
                                 int a_mrcpVersion);
                int RegisterServer(char* a_resourceType, char* a_serverAddress, int a_serverPort,
                                   MrcpCallBackFunction a_callbackFunc); 
		int Stop(const CMrcpInterface* a_mrcpInterface);
		int OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg,const CMrcpInterface* a_mrcpInterface, 
                                char* a_resourceType,char* a_asrSrvrAddr, char* a_ttsSrvrAddr);
		int CloseSession(MrcpSessionHandle a_sessionHandle, const CMrcpInterface* a_mrcpInterface);
                int AddSessionResource(MrcpSessionHandle a_sessionHandle, MrcpResourceType a_resourceType, 
                                       const CMrcpInterface* a_mrcpInterface);
  //Recognizer functions
		int DefineGrammar(MrcpSessionHandle a_sessionHandle,DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams, 
                                  const CMrcpInterface* a_mrcpInterface);
		int Recognize(MrcpSessionHandle a_sessionHandle,RECOGNIZE_STRUCT* a_recognizeParams,
                              const CMrcpInterface* a_mrcpInterface);
		int Interpret(MrcpSessionHandle a_sessionHandle,INTERPRET_STRUCT* a_interpretParams,
                              const CMrcpInterface* a_mrcpInterface);
		int StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int* a_referenceNum,
                                     const CMrcpInterface* a_mrcpInterface);
		int GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum,
                              const CMrcpInterface* a_mrcpInterface);
		int StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, 
                              const CMrcpInterface* a_mrcpInterface);
  //TTS functions
		int Speak(MrcpSessionHandle a_sessionHandle,SPEAK_STRUCT* a_speakParams,
                          const CMrcpInterface* a_mrcpInterface);
		int StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, 
                              const CMrcpInterface* a_mrcpInterface);
		int Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, 
                          const CMrcpInterface* a_mrcpInterface);
		int Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, 
                           const CMrcpInterface* a_mrcpInterface);
		int BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, 
                            const CMrcpInterface* a_mrcpInterface);
  //Generic
		int GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo,GET_PARAMETERS a_parameters,
                              int* a_referenceNum, const CMrcpInterface* a_mrcpInterface);
		int SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo,SET_PARAMETERS a_parameters,                              int* a_referenceNum, const CMrcpInterface* a_mrcpInterface);




#ifdef _cplusplus
}
#endif
//} //end namespace
#endif

