//=============================================================================
//
//	File Name:		MrcpUtils.cpp
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


#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "MrcpUtils.h"

#define M_ENDL "\r\n"

static const char * enc_start = "<s_encoded>", * enc_end = "</s_encoded>" ; 


//////////////////////////////////////////////////////////////////////////////////////
//
//Description - connects a socket to the server
// Input - hostname dotted ip address , port # to connect to
// Output - Socket number connected
//
/////////////////////////////////////////////////////////////////////////////////////

SOCKET MrcpUtils::ConnectServerSocket( std::string hostname, int port, int& fd, int connectBackoff,int& sockStatus )
{
      int sock_flags, error;
      fd_set  rset, wset;
      struct timeval tval;	
      socklen_t       len;
      socklen_t       sockAddrSize = sizeof(struct sockaddr);     
      struct sockaddr remoteSockCon;
      sockStatus = 0;
     
      time_t start_time = time ( 0 ) ; 
//FYI
	//SOCK_DGRAM == UDP socket
	//SOCK_STREAM == TCP socket

	SOCKET sock ;
//the following is needed for linux
        SOCKET l_socket;

	sock = socket ( AF_INET, SOCK_STREAM, 0 ) ;
	if ( sock < 0 )
	{
	   int error_number = GetLastSocketError ( "MrcpUtils::GetConnectedSocket - getting a socket" ) ; 
           sockStatus = error_number;
	   return INVALID_SOCKET ; 
	}
	BOOL value = 1 ; 
        setsockopt ( sock,SOL_SOCKET, SO_ERROR, (const char*)&value, sizeof ( value ) ) ; 
	
        fd = sock;
	SOCKADDR_IN remoteAddr;
	IN_ADDR address ;
	
	memset(&remoteAddr, 0, sizeof(remoteAddr));
		
        // See if the host is specified in "dot address" form
	int this_addr = inet_addr(hostname.c_str());
	memcpy ( &address, &this_addr, sizeof ( this_addr)) ;
	if ( this_addr == -1) 
	{
	   CloseSocket ( sock ) ; 
	   sock = INVALID_SOCKET ;
	   return sock ; 
	}
	memcpy ( &remoteAddr.sin_addr, &this_addr, sizeof ( this_addr ) ) ;

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	
	time_t connect_time = time(0) ; 
        struct sockaddr_in local_addr ;
        memset((char *)&local_addr, 0, sizeof(local_addr)) ;

	socklen_t nameLen = sizeof(struct sockaddr);

        sock_flags = fcntl (sock, F_GETFL, 0);
        fcntl (sock, F_SETFL, sock_flags | O_NONBLOCK);  //set for non blocking for faster timeout.
	
	int res = connect(sock, (PSOCKADDR)&remoteAddr, sizeof(remoteAddr) );              
        if (res < 0)
        {
             FD_ZERO(&rset);
             FD_SET(sock, &rset);
             wset = rset;
             tval.tv_sec = 0;
             tval.tv_usec = 300;
//             tval.tv_usec = 600;
             if ((res = select(sock, &rset, &wset, NULL, &tval)) == 0)
             {
                len = sizeof(error);
                errno = 0;
                if (getpeername(sock, (struct sockaddr *)&remoteSockCon, &sockAddrSize) < 0)
                {
                     errno = 0;
                     tval.tv_usec = connectBackoff;
                     res = select(sock, &rset, &wset, NULL, &tval);
                     if(getpeername(sock,(struct sockaddr *)&remoteSockCon, &sockAddrSize) < 0)
                     {
                        CloseSocket(sock);
                        sock = INVALID_SOCKET;
                        printf("2nd getpeername Socket = %d\n",sock);
                        return sock;
                     }
                }
             }
        }
        fcntl (sock, F_SETFL, sock_flags);  //set back to blocking socket.
       	if(getsockname(sock, (struct sockaddr *)&local_addr, &nameLen)==-1)
                	printf("GetSockName ERROR\n");
       	l_socket = ntohs(local_addr.sin_port);
	int conn_duration = time ( 0 ) - start_time ;
		
	return l_socket ;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - converts integer to a string 
// Input - interger value and length
// Output - converted string
//
/////////////////////////////////////////////////////////////////////////////////////
std::string MrcpUtils::itos(int data, int len)
{
	char buf[100] ;
	
	char format[10] ; 
	
	if ( len )
		snprintf ( format, sizeof(format), "%%0%dd", len ) ; 
	else
		snprintf ( format, sizeof(format), "%%d" ) ; 
	
	snprintf ( buf, sizeof(buf), format, data ) ;
	return buf ;
}
//////////////////////////////////////////////////////////////////////////////////////
//
//Description - checks for numeric value in string
// Input - character pointer
// Output - integer length
//
/////////////////////////////////////////////////////////////////////////////////////
bool MrcpUtils::isNumeric(char* str, int len)
{
      for(int i=0;i<len && str[i];i++)
            if((str[i])<0x30 || (str[i]) >0x39)
                  return false;
      return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - sends data to the mrcp server
// Input - socket connected to, pointer to data and length of data
// Output - Socket return status
//
/////////////////////////////////////////////////////////////////////////////////////

//the following is used to send to the mrcp server
const CRV MrcpUtils::TcpSend ( SOCKET sock, const char * const data, int length, int status ) 
{
	
	CRV result = CRV_ok ;
	int res = 0, res2 = 0 ;
        int err = 0; 
	
	if ( sock == INVALID_SOCKET )
		return CRV_failure ; 
	
        struct sockaddr remoteSockCon;
        socklen_t       sockAddrSize = sizeof(struct sockaddr);     
	
        res = send ( sock, data, length, 0 ) ; 
      
	if ( res < 0 ) 
	{
		int error = GetLastSocketError( "MrcpUtils::TcpSend - sending data" ) ; 
		
			printf( "Unable to send to remote host %d-%d - %d/%d\n",
				res, length, sock, error) ; 
		status = error;
		result = CRV_failure ;
	}
	
	return result ;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - closes a socket connection to the server
// Input - socket
// Output - 
//
/////////////////////////////////////////////////////////////////////////////////////
void MrcpUtils::CloseSocket(SOCKET sock)
{
	
	time_t start = time(0) ; 
	
	int res = 0 ; 
	
	// if the socket is already closed, just return 
	if ( sock == INVALID_SOCKET || sock == 0 ) 
	{
		return ; 
	}
	
	res = close ( sock ) ; 
	
	if ( res )
	{
		GetLastSocketError ( "Closing socket" ) ; 
	}
	
	unsigned int duration = time(0) - start ; 
	
	
}
//////////////////////////////////////////////////////////////////////////////////////
//
//Description - gets the last socket error returned
// Input - not used
// Output - Socket error
//
/////////////////////////////////////////////////////////////////////////////////////

int MrcpUtils::GetLastSocketError( const char * what )
{
	
	int error_number ; 
#ifdef WIN32	
	error_number = WSAGetLastError () ; 
#else
	error_number = errno;
#endif
	
	return error_number ; 
}
//////////////////////////////////////////////////////////////////////////////////////
//
//Description - finds a string within a string
// Input - string and pattern to find
// Output - char*
//
/////////////////////////////////////////////////////////////////////////////////////


char *MrcpUtils::stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;

      for (start = (char *)String; *start != NULL; start++)
      {
            /* find start of pattern in string */
            for ( ; ((*start!=NULL) && (toupper(*start) != toupper(*Pattern))); start++)
                  ;
            if (NULL == *start)
                  return NULL;

            pptr = (char *)Pattern;
            sptr = (char *)start;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if (NULL == *pptr)
                        return (start);
            }
      }
      return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - parses for an element and returns after patten is found
// Input - ptr to target, sizeof target, ptr to source, ptr to tag to find, case sensitive,spaces before
// Output - ture false
//
/////////////////////////////////////////////////////////////////////////////////////

// puts into target a null terminated value of the element of ops transm. 
bool MrcpUtils::ParseElement(char *target, size_t targetsize, const char *source, char const *tag, 
							bool insensitive, bool spaces_before )
{
	bool ret = true;
	if ( !target || !source || !tag || !targetsize) return ret; 
	
	const char * orig = NULL ;
	
	if ( !insensitive )
		orig = strstr ( source, tag ) ; 
	else
		orig = MrcpUtils::stristr ( source, tag ) ; 

	if ( !orig )
		target[0] = 0 ; 
	else
	{
		const char * end = NULL ;
		orig += strlen ( tag ) ; 
		
		// if starts with a quote, go to the next char and end at the quote
		if ( *orig == '"' )
		{
			orig ++ ; 
			end = strchr ( orig, '"' ) ; 
		}
		else
		{
			if ( spaces_before )
				while ( *orig == ' ' ) 
					orig ++ ;	// move until we hit something

			end = strchr ( orig, ' ' ) ; 
			if ( !end )
				end = strchr ( orig, '\n' ) ; 
			if ( !end )
				end = strchr ( orig, '\r' ) ; 
			
			// see if there is an unprintable character prior to the end, if so end there.
			const char * cptr ; 
			for ( cptr = orig ; cptr <= end ; cptr ++ )
				if ( *cptr < 0x20 )
					break ; 
				
				if ( cptr < end ) end = cptr ; 
				
				if ( end == NULL ) end = (char*)source + strlen ( source ) ; 
		}
		size_t len = end - orig;
		if (len > (targetsize-1)) 
		{
			len = targetsize - 1;
			ret = false;
		}
		memcpy ( target, orig, len ) ; 
		target [len ] = 0 ; 
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////////////////////
//
//Description - gets packet from tcp socket
// Input - socket, ptr to data received, length of data received
// Output - Socket status
//
/////////////////////////////////////////////////////////////////////////////////////


const CRV MrcpUtils::TcpGet ( SOCKET sock, char * data, int & length )
{
	int len ; 
	CRV result = CRV_ok ; 
	
	len = read ( sock, data, length ) ; 
	
	if ( len == 0 ) 
	{
		result = CRV_illegal_state ;
		length = 0 ; 
	}
	else if ( len < 0 ) 
	{
		result = CRV_failure ;
		int err = GetLastSocketError( "SitUtils::TcpGet - getting data" ) ; 
		
	}
	else
	{
		result = CRV_ok ;
		length = len ; 
	}
	
	return result ;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - opens a udp listener port
// Input - port # to connect open, boolean to indicate broadcast 
// Output - Socket number connected
//
/////////////////////////////////////////////////////////////////////////////////////

SOCKET MrcpUtils::OpenUdpListener( unsigned short this_port, bool broadcast )
{
	SOCKADDR_IN local_addr ; 
#ifdef WIN32
		WSADATA WsaData;
	if (SOCKET_ERROR == WSAStartup(0x0202, &WsaData))
	{
		return INVALID_SOCKET;
	}
#endif
	
	SOCKET descr = socket ( AF_INET, SOCK_DGRAM, 0 ) ; 
	if ( descr == INVALID_SOCKET ) 
		return INVALID_SOCKET ;
	
	memset ( &local_addr, 0, sizeof ( local_addr ) ) ;
	local_addr.sin_port = htons ( this_port ) ; 
	local_addr.sin_family = AF_INET ;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ( broadcast )
	{
		BOOL value = 1 ; 
		setsockopt ( descr, SOL_SOCKET, SO_BROADCAST, (const char*)&value, sizeof ( value ) ) ; 
	}
	
	int err = bind ( descr, (PSOCKADDR)&local_addr, sizeof ( local_addr ) ) ;
	if ( err < 0 ) 
	{
	   	err = GetLastSocketError( "MrcpUtils::OpenUdpListener - binding" );
		printf ( "Error %d opening udp listener \n", err ) ; 
		return INVALID_SOCKET ; 
	}
		
	return descr ;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - performs a udp get on socket
// Input - socket, result, result length, origin and origin length
// Output - number of bytes gotten
//
/////////////////////////////////////////////////////////////////////////////////////
int MrcpUtils::UdpGet( SOCKET socket, char * result, int res_len )
{
	
	struct sockaddr srcS ; 
	struct sockaddr_in src ;
	socklen_t srcSize = sizeof ( srcS ) ; 
	
	int buf_len = recvfrom ( socket, result, res_len, 0, &srcS, &srcSize ) ;
	if (buf_len <= 0 )  
	{
//		int err = GetLastSocketError( "SitUtils::UdpGet - receiving" ) ;
                printf ("UdpGet Error = %d buf_len = %d\n",errno,buf_len);
		return -1 ;
	}
	
	return buf_len ; 
}

//////////////////////////////////////////////////////////////////////////////////////
//
//Description - returns the differance in length
// Input - ptr to input and size of substring
// Output - differance
//
/////////////////////////////////////////////////////////////////////////////////////

int MrcpUtils::GetLengthDiff ( const char * input, int & num_subst ) 
{
	int num_wide = 0, diff = 0 ; 
	const char * start_ptr = input ; 

	num_subst = 0 ; 
	while ( ( start_ptr = strstr ( start_ptr, enc_start ) ) != NULL ) 
	{
		char * end_ptr = strstr ( start_ptr, enc_end ) ; 
		if ( end_ptr )
		{
			num_wide += ( end_ptr - start_ptr - strlen ( enc_start ) ) / 4 ; 
			num_subst ++ ; 
		}
		else
		{
			return 0 ; 
		}

		start_ptr = end_ptr ; 
	}

	if ( num_subst != 0 ) 
	{
		// see what the increase in length is
		int was = num_subst * ( strlen ( enc_end ) + strlen ( enc_start ) ) + num_wide * 4 ; 
		int will_be = num_wide * sizeof ( wchar_t ) ; 
		diff = will_be - was ; 
	}

	return diff ; 
}

////////////////////////////////////////////////////////////////////////// 
//
// Description - Destructor
// Input - None
//         
// Output - None
//          
///////////////////////////////////////////////////////////////////////////

void MrcpUtils::MsSleep( unsigned int a_millisecs) 
{
#ifdef _WIN32

   Sleep( a_millisecs);     

#else

   usleep( a_millisecs * 1000);     

#endif     
}
///////////////////////////////////////////////////////////////////////////////
// Description: Operating System independent thread ID accessor
// Return: unsigned long
// Parameters: none
///////////////////////////////////////////////////////////////////////////////
unsigned long MrcpUtils::ThreadID()
{
#ifdef _WIN32
   return GetCurrentThreadId();
#else
   return pthread_self();
#endif

}
///////////////////////////////////////////////////////////////////////////////
// Description: Operating System independent integer to ascii
// Return: char *
// Parameters: int , char*, int
///////////////////////////////////////////////////////////////////////////////

char* MrcpUtils::itoa( int value, char* result, int base ) 
{	
	// check that the base if valid
	
	if (base < 2 || base > 16) { *result = 0; return result; }
	char* out = result;
	int quotient = value;
	
	do {
		*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
		++out;
		quotient /= base;
	} while ( quotient );

	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';
	std::reverse( result, out );
	*out = 0;
	return result;
}
