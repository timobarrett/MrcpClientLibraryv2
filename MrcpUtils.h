//=============================================================================
//
//	File Name:		MrcpUtils.h
//
//	Description:	Mrcp Client Library support methods 
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

#ifndef MrcpUtils_H
#define MrcpUtils_H


#ifdef WIN32
#pragma warning( disable : 4786 )  
#pragma warning( disable : 4251 ) 

#include <wchar.h>
#include <stdarg.h>
#include <winsock2.h>
#include <algorithm>
#define snprintf _snprintf
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
typedef int SOCKET;
#define BOOL int
#include <netinet/in.h>
typedef sockaddr_in SOCKADDR_IN, *PSOCKADDR_IN;
typedef sockaddr *PSOCKADDR;
extern "C" unsigned long inet_addr(const char*);
extern "C" char* inet_ntoa(in_addr ina);
#define SD_RECEIVE 0
#define INVALID_SOCKET -1
#define _snprintf snprintf
typedef in_addr *IN_ADDR;
#endif
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>


typedef enum CRV { 
	CRV_ok	=	0,
	CRV_failure,
	CRV_not_handled,
	CRV_illegal_state
} ; 


class MrcpUtils  
{
public:
	static std::string itos(int data, int len = 0 ) ; 
	static const std::string ltos(
#ifdef WIN32
		__int64
#else
		long long
#endif
		data) ; 
	int GetLengthDiff ( const char * input, int & num_subst ) ; 
	static bool ParseElement(char *target, size_t targetsize, const char *source, char const *tag, bool = false, bool = false ) ; 
	static bool isNumeric(char* str, int len);

	static void CloseSocket ( SOCKET sock );
	static int GetLastSocketError ( const char * what = NULL );
	static SOCKET ConnectServerSocket( std::string hostname, int port, int& fd, int connectBackoff, int& sockStatus ) ; 
	static const CRV TcpGet ( SOCKET sock, char * data, int & len ) ;
	static const CRV TcpSend ( SOCKET sock, const char * const data, int length, int status ) ; 
	static SOCKET OpenUdpListener( unsigned short this_port, bool broadcast = false ) ; 
	static char * stristr ( const char *, const char * ) ; 
	static int UdpGet( SOCKET socket, char * result, int res_len);

	static void MsSleep( unsigned int a_millisecs) ;
	static unsigned long ThreadID();
	static char* itoa(int value, char* result, int base);
};

#endif // !defined(AFX_MrcpUtils_H__E1DF8DD9_5E77_4848_B708_9FE7D7D086A8__INCLUDED_)
