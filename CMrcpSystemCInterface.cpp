///////////////////////////////////////////////////////////////////////////////
//	File Name:		CMrcpSystemCInterface.cpp
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
#include "CMrcpSystemCInterface.h"
#include "CLogger.h"
#include "CMrcpSystem.h"
#include "CMrcpServerMgr.h"
#include "CMrcpServer.h"

////////////////////////////////////////////////////////////////////////// 
//
// Description - Start
// Input - 
//         
// Output - success or failure and returns pointer to object
//          calls into CMrcpSystem start to get things rolling
//			
///////////////////////////////////////////////////////////////////////////
CMrcpInterface* Start(int a_logLevel)
{
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem;
	int l_stat = 0;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface start");
	//instantiate the MRCP singleton object
	l_mrcpSystem = MrcpV2RefLib::CMrcpSystem::Instance();

	//start the MRCP Library
	l_stat = l_mrcpSystem->Start(a_logLevel);
	if (l_stat == MrcpError)
	{
		CLogger::Instance()->Log(LOG_LEVEL_ERROR, "Error on start"); 
	}
	CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface started");
	return l_mrcpSystem;


}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Stop
// Input - pointer to Mrcp object
//         
// Output - success or failure 
//          calls into CMrcpSystem stop to stop session
//			
///////////////////////////////////////////////////////////////////////////
int Stop(const CMrcpInterface* a_mrcpInterface)
{
   int l_stat = 0;
   MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;
   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface stop");

   l_stat = l_mrcpSystem->Stop();
   if (l_stat == MrcpError)
   {
      CLogger::Instance()->Log(LOG_LEVEL_ERROR,"Error C interface stop");
   }
   CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface stopped");
   return l_stat;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - SetCommonCfg
// Input - client IP address, socket backoff interval,
//         keep alive interval, keep alive cnt
//
// Output - success or failure
//          calls into CMrcpServerMgr to setcommoncfg
//          SHOULD BE CALLED ONE TIME ONLY
///////////////////////////////////////////////////////////////////////////
int SetCommonCfg(char* a_clientAddress, int a_sockConBckOff, int a_keepAlveInt, int a_keepAlveCnt, int a_mrcpVersion)
{
   int l_stat = 0;
   MrcpV2RefLib::CMrcpServerMgr* l_mrcpSrvrMgr;

   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface SetCommonCfg");
   l_mrcpSrvrMgr = MrcpV2RefLib::CMrcpServerMgr::Instance();

   l_stat = l_mrcpSrvrMgr->SetCommonCfg(a_clientAddress, a_sockConBckOff, a_keepAlveInt, a_keepAlveCnt,a_mrcpVersion);
   if (l_stat == MrcpError)
   {
       CLogger::Instance()->Log(LOG_LEVEL_ERROR,"Error C interface SetCommonCfg");
   }
   CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface setcommoncfg exited");
   return l_stat;
}

//////////////////////////////////////////////////////////////////////////
//
// Description - RegisterServer
// Input - resource type, server IP address, server port
//         
//
// Output - success or failure
//          calls into CMrcpServerMgr to RegisterServer
//          Called during config file processing 
///////////////////////////////////////////////////////////////////////////
int RegisterServer(char* a_resourceType, char* a_serverAddress, int a_serverPort,MrcpCallBackFunction a_callbackFunc)
{
   int l_stat = 0;
   MrcpV2RefLib::CMrcpServerMgr* l_mrcpSrvrMgr;

   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface RegisterServer");
   l_mrcpSrvrMgr = MrcpV2RefLib::CMrcpServerMgr::Instance();

   l_stat = l_mrcpSrvrMgr->RegisterServer(a_resourceType, a_serverAddress, a_serverPort, a_callbackFunc);
      if (l_stat == MrcpError)
   {
       CLogger::Instance()->Log(LOG_LEVEL_ERROR,"Error C interface RegisterServer");
   }
   CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface RegisterServer exit");
   return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - OpenSession
// Input - config structure and pointer to Mrcp obj
//         
// Output - success or failure 
//          calls into CMrcpSystem OpenSession which connects to MRCP server
//			
///////////////////////////////////////////////////////////////////////////
int OpenSession(RESOURCE_CFG_STRUCT* a_resourceCfg, const CMrcpInterface* a_mrcpInterface, char* a_resourceType,
                char* asrSrvrAddr, char* ttsSrvrAddr)
{
	int l_stat = 0;
        int l_serverPort;
        MrcpV2RefLib::CMrcpServerMgr* l_mrcpSrvrMgr = MrcpV2RefLib::CMrcpServerMgr::Instance();
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface OpenSession ");
//        l_stat = l_mrcpSrvrMgr->GetServer(a_resourceType,asrSrvrAddr,&l_serverPort);

        if (!strcmp(a_resourceType, "ASR"))
        {
           if (!l_mrcpSrvrMgr->GetNumAsrServers()&& !l_mrcpSrvrMgr->GetNumDualServers())
           {
              CLogger::Instance()->Log(LOG_LEVEL_ERROR,"No Asr Servers Configured - No combined Servers Configured");
              return -1;
           }
	   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Session Resource = ASR ");
           l_stat = l_mrcpSrvrMgr->GetServer(a_resourceType,asrSrvrAddr,&l_serverPort);
           a_resourceCfg->asrServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
           a_resourceCfg->asrServerConfig->serverAddress = asrSrvrAddr;
           a_resourceCfg->asrServerConfig->serverPort    = l_serverPort;
           //clientPort,codec & callbackInfo info populated in mrcpapp
        }
        if (!strcmp(a_resourceType,"TTS"))
        {
           if (!l_mrcpSrvrMgr->GetNumTtsServers()&& !l_mrcpSrvrMgr->GetNumDualServers())
           {
              CLogger::Instance()->Log(LOG_LEVEL_ERROR,"No TTS Servers Configured - No Combined Servers Configured");
              return -1;
           }
	   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Session Resource = TTS ");
           l_stat = l_mrcpSrvrMgr->GetServer(a_resourceType,ttsSrvrAddr,&l_serverPort);
//           strncpy(ttsSrvrAddr, asrSrvrAddr, strlen(asrSrvrAddr));
           a_resourceCfg->ttsServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
           a_resourceCfg->ttsServerConfig->serverAddress = ttsSrvrAddr;
           a_resourceCfg->ttsServerConfig->serverPort    = l_serverPort;
           //clientPort,codec & callbackInfo info populated in mrcpapp
        }

        if (!strcmp(a_resourceType,"BOTH"))
        {
           if (!l_mrcpSrvrMgr->GetNumDualServers())
           {
                CLogger::Instance()->Log(LOG_LEVEL_INFO,"No Combined TTS/ASR Servers Configured Getting individual");
                if (l_mrcpSrvrMgr->GetNumAsrServers())
                {
                   l_stat = l_mrcpSrvrMgr->GetServer("ASR",asrSrvrAddr,&l_serverPort);
                   a_resourceCfg->asrServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
                   a_resourceCfg->asrServerConfig->serverAddress = asrSrvrAddr;
                   a_resourceCfg->asrServerConfig->serverPort    = l_serverPort;
                }
                else
                {
                   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Combined TTS/ASR Request ASR not Configured");
                }
                if (l_mrcpSrvrMgr->GetNumTtsServers())
                { 
                   l_stat = l_mrcpSrvrMgr->GetServer("TTS",ttsSrvrAddr,&l_serverPort);
                   a_resourceCfg->ttsServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
                   a_resourceCfg->ttsServerConfig->serverAddress = ttsSrvrAddr;
                   a_resourceCfg->ttsServerConfig->serverPort    = l_serverPort;
                }
                else
                {
                   CLogger::Instance()->Log(LOG_LEVEL_INFO,"Combined TTS/ASR Request TTS not Configured");
                }
               
           }
           else
           {
              CLogger::Instance()->Log(LOG_LEVEL_INFO,"Session Resource = BOTH ");
        //ASSUMPTION - ASR and TTS information is identical for a configuration using type "BOTH". Use tts Addr

              l_stat = l_mrcpSrvrMgr->GetServer(a_resourceType,ttsSrvrAddr,&l_serverPort);
            //strncpy(ttsSrvrAddr, asrSrvrAddr, strlen(asrSrvrAddr));
              a_resourceCfg->ttsServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
              a_resourceCfg->ttsServerConfig->serverAddress = ttsSrvrAddr;
              a_resourceCfg->ttsServerConfig->serverPort    = l_serverPort;
              a_resourceCfg->asrServerConfig->mrcpVersion   = l_mrcpSrvrMgr->GetMrcpVersion();
//              a_resourceCfg->asrServerConfig->serverAddress = asrSrvrAddr;
              a_resourceCfg->asrServerConfig->serverAddress = ttsSrvrAddr;
              a_resourceCfg->asrServerConfig->serverPort    = l_serverPort;
              //clientPort,codec & callbackInfo info populated in mrcpapp
           }
       }
        

        a_resourceCfg->clientAddress      = l_mrcpSrvrMgr->GetClientAddr();
        a_resourceCfg->sockConnectBackoff = l_mrcpSrvrMgr->GetSockConBackoff();
        a_resourceCfg->keepAliveInterval  = l_mrcpSrvrMgr->GetKeepAliveInt();
        a_resourceCfg->keepAliveCnt       = l_mrcpSrvrMgr->GetKeepAliveCnt();

	l_stat = l_mrcpSystem->OpenSession(a_resourceCfg);
	if (l_stat != MrcpSuccess)
	   CLogger::Instance()->Log(LOG_LEVEL_ERROR,"Error C interface OpenSession");
        else
           CLogger::Instance()->Log(LOG_LEVEL_INFO, "Exiting C Interface OpenSession");
	return l_stat;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - CloseSession
// Input - session handle to close and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to close 
//			
///////////////////////////////////////////////////////////////////////////

int CloseSession(MrcpSessionHandle a_sessionHandle, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface CloseSession");

	if (a_sessionHandle != 0)
	{
	  l_stat = l_mrcpSystem->CloseSession(a_sessionHandle);
	  if (l_stat != MrcpSuccess)
	  	CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface CloseSession Error");
	}

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - AddSessionResource
// Input - session handle to close and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to reinvite adding resources 
//			
///////////////////////////////////////////////////////////////////////////

int AddSessionResource(MrcpSessionHandle a_sessionHandle, MrcpResourceType a_resourceType, const CMrcpInterface* a_mrcpInterface)
{
  int l_stat = 0;
  MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *) a_mrcpInterface;
 
  CLogger::Instance()->Log(LOG_LEVEL_INFO,"C interface AddSessionResources");
  if (a_sessionHandle != 0)
  {
     l_stat = l_mrcpSystem->AddSessionResources(a_sessionHandle, a_resourceType);
     if (l_stat != MrcpSuccess)
        CLogger::Instance()->Log(LOG_LEVEL_INFO, "C interface AddSessionResource Error");
  }

  return l_stat;
}
//recognizer functions
////////////////////////////////////////////////////////////////////////// 
//
// Description - DefineGrammar
// Input - session handle to asr, grammar and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to define grammar
//			
///////////////////////////////////////////////////////////////////////////
int DefineGrammar(MrcpSessionHandle a_sessionHandle,DEFINE_GRAMMAR_STRUCT* a_defineGrammarParams, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface DefineGrammar");

	l_stat = l_mrcpSystem->DefineGrammar(a_sessionHandle, a_defineGrammarParams);
	if (l_stat != MrcpSuccess)
		CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface DefineGrammar Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Recognize
// Input - session handle to asr,recognize struct and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to recognize
//			
///////////////////////////////////////////////////////////////////////////
int Recognize(MrcpSessionHandle a_sessionHandle,RECOGNIZE_STRUCT* a_recognizeParams,const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface Recognize");
        l_stat = l_mrcpSystem->Recognize(a_sessionHandle, a_recognizeParams);
	if (l_stat != MrcpSuccess)
		CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface Recognize Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Interpret
// Input - session handle to asr,interpret struct and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to interpret
//			
///////////////////////////////////////////////////////////////////////////
int Interpret(MrcpSessionHandle a_sessionHandle,INTERPRET_STRUCT* a_interpretParams,const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface Interpret");
	l_stat = l_mrcpSystem->Interpret(a_sessionHandle, a_interpretParams);
	if (l_stat != MrcpSuccess)
			CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface Interpret Error");

	return l_stat;

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StartInputTimers
// Input - session handle to asr session,callback function ptr, referenceNumber and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to interpret
//			
///////////////////////////////////////////////////////////////////////////
int StartInputTimers(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo, int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface StartInputTimers");
	l_stat = l_mrcpSystem->StartInputTimers(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
			CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface StartInputTimers Error");

	return l_stat;

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - GetResult
// Input - session handle to asr,callback function ptr, referenceNumber and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to interpret
//			
///////////////////////////////////////////////////////////////////////////
int GetResult(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum,const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface GetResult");
	l_stat = l_mrcpSystem->GetResult(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
	   CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface GetResult Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - StopRecog
// Input - session handle to asr,callback function ptr, referenceNumber and pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to interpret
//			
///////////////////////////////////////////////////////////////////////////
int StopRecog(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface StopRecog");
	l_stat = l_mrcpSystem->StopRecog(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
		CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface StopRecog Error");

	return l_stat;

}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Speak
// Input - session handle to TTS,Speak parameters,pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to Speak
//			
///////////////////////////////////////////////////////////////////////////
int Speak(MrcpSessionHandle a_sessionHandle,SPEAK_STRUCT* a_speakParams, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface Speak");
	l_stat = l_mrcpSystem->Speak(a_sessionHandle,a_speakParams);

	if (l_stat != MrcpSuccess)
		CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface Speak Error");

	return l_stat;
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - StopSpeak
// Input - session handle to TTS,Callback function ptr,pointer to reference number,pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to StopSpeak
//			
///////////////////////////////////////////////////////////////////////////
int StopSpeak(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface StopSpeak");
	l_stat = l_mrcpSystem->StopSpeak(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface StopSpeak Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - Pause
// Input - session handle to TTS,Callback function ptr,pointer to reference number,pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to Pause
//			
///////////////////////////////////////////////////////////////////////////
int Pause(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface Pause");
	l_stat = l_mrcpSystem->Pause(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface Pause Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - resume
// Input - session handle to TTS,Callback function ptr,pointer to reference number,pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to StopSpeak
//			
///////////////////////////////////////////////////////////////////////////
int Resume(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface Resume");
	l_stat = l_mrcpSystem->Resume(a_sessionHandle,a_callBackInfo,*a_referenceNum);
	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface Resume Error");

	return l_stat;
}
////////////////////////////////////////////////////////////////////////// 
//
// Description - BargeIn
// Input - session handle to TTS,Callback function ptr,pointer to reference number,pointer to Mrcp Obj
//         
// Output - success or failure 
//          calls into CMrcpSystem to BargeIn
//			
///////////////////////////////////////////////////////////////////////////
int BargeIn(MrcpSessionHandle a_sessionHandle,MrcpCallBackInfo a_callBackInfo,int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface BargeIn");
    l_stat = l_mrcpSystem->BargeIn(a_sessionHandle, a_callBackInfo, *a_referenceNum);
	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface BargeIn Error");

	return l_stat;

}
//////////////////////////////////////////////////////////////////////////
//
// Description - GetParams
// Input - session handle,Callback function ptr,parameter struct, pointer to reference number,pointer to Mrcp Obj
//
// Output - success or failure
//          calls into CMrcpSystem to BargeIn
//
///////////////////////////////////////////////////////////////////////////
int GetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, GET_PARAMETERS a_parameters, int* a_referenceNum,const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface GetParams");
	l_stat = l_mrcpSystem->GetParams(a_sessionHandle,a_callBackInfo,a_parameters,*a_referenceNum);
	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface getParams Error");

	return l_stat;

}
//////////////////////////////////////////////////////////////////////////
//
// Description - SetParams
// Input - session handle,Callback function ptr,parameter structure, pointer to reference number,pointer to Mrcp Obj
//
// Output - success or failure
//          calls into CMrcpSystem to BargeIn
//
///////////////////////////////////////////////////////////////////////////
int SetParams(MrcpSessionHandle a_sessionHandle, MrcpCallBackInfo a_callBackInfo, SET_PARAMETERS a_parameters, int* a_referenceNum, const CMrcpInterface* a_mrcpInterface)
{
	int l_stat = 0;
	MrcpV2RefLib::CMrcpSystem* l_mrcpSystem = (MrcpV2RefLib::CMrcpSystem *)a_mrcpInterface;

	CLogger::Instance()->Log(LOG_LEVEL_INFO,"Entering C interface SetParams");
	l_stat = l_mrcpSystem->SetParams(a_sessionHandle,a_callBackInfo,a_parameters,*a_referenceNum);

	if (l_stat != MrcpSuccess)
	    CLogger::Instance()->Log(LOG_LEVEL_ERROR,"C interface SetParams Error");

	return l_stat;

}


