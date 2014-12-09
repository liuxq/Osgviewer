#if !defined _PRONET_
#define	_PRONET_

#include "stdafx.h"                        /* main include file */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>

#define bcopy( src, dst, len )	\
		memcpy( dst, src, len )


#define READY_FOR_VGFRAME	1

#define	MAXCLINETNUMBER	3
bool RunByNetwork;
int  numClients=1;
int clientPortNumber[MAXCLINETNUMBER];



void checkGethostname( int returnValue )
{
	
	if( returnValue != SOCKET_ERROR ) return;

	switch( WSAGetLastError() )
	{
		case WSAEFAULT:			printf( "WSAEFAULT\n" );			break;
		case WSANOTINITIALISED:	printf( "WSANOTINITIALISED\n" );	break;
		case WSAENETDOWN:		printf( "WSAENETDOWN\n" );			break;
		case WSAEINPROGRESS:	printf( "WSAEINPROGRESS\n" );		break;
	}
	exit( 1 );
}

void checkAccept( int returnValue )
{
	if( returnValue >= 0 ) return;

	switch( WSAGetLastError() )
	{
		case WSANOTINITIALISED:	printf( "WSANOTINITIALISED\n" );	break;
		case WSAENETDOWN:		printf( "WSAENETDOWN\n" );			break;
		case WSAEFAULT:			printf( "WSAEFAULT\n" );			break;
		case WSAEINTR:			printf( "WSAEINTR\n" );				break;
		case WSAEINPROGRESS:	printf( "WSAEINPROGRESS\n" );		break;
		case WSAEINVAL:			printf( "WSAEINVAL\n" );			break;
		case WSAEMFILE:			printf( "WSAEMFILE\n" );			break;
		case WSAENOBUFS:		printf( "WSAENOBUFS\n" );			break;
		case WSAENOTSOCK:		printf( "WSAENOTSOCK\n" );			break;
		case WSAEOPNOTSUPP:		printf( "WSAEOPNOTSUPP\n" );		break;
		case WSAEWOULDBLOCK:	printf( "WSAEWOULDBLOCK\n" );       break;
	}

	exit( 1 );
}

void getWinsockDll( void )
{
	WORD				wVersionRequested;
	WSADATA				wsaData;
	int					err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		// Tell the user that we could not find a usable WinSock DLL.
		printf( "Could not find a usable WinSock DLL.\n" );
		exit( 1 );
	}
 
	// Confirm that the WinSock DLL supports 2.2. Note that if the DLL 
	// supports versions greater than 2.2 in addition to 2.2, it will 
	// still return 2.2 in wVersion since that is the version we requested.
 
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		// Tell the user that we could not find a usable WinSock DLL.
		printf( "Unable to find a usable WinSock DLL.\n" );
		WSACleanup( );
		exit( 1 );
	}
 
	// The WinSock DLL is acceptable. Proceed. 
}

void checkConnect( int returnValue )
{
	
	if( returnValue != SOCKET_ERROR ) return;

	switch( WSAGetLastError() )
	{
		case WSANOTINITIALISED:	printf( "WSANOTINITIALISED\n" );	break;
		case WSAENETDOWN:		printf( "WSAENETDOWN\n" );			break;
		case WSAEADDRINUSE:		printf( "WSAEADDRINUSE\n" );		break;
		case WSAEINTR:			printf( "WSAEINTR\n" );				break;
		case WSAEINPROGRESS:	printf( "WSAEINPROGRESS\n" );		break;
		case WSAEALREADY:		printf( "WSAEALREADY\n" );			break;
		case WSAEADDRNOTAVAIL:	printf( "WSAEADDRNOTAVAIL\n" );		break;
		case WSAEAFNOSUPPORT:	printf( "WSAEAFNOSUPPORT\n" );		break;
		case WSAECONNREFUSED:	printf( "WSAECONNREFUSED\n" );		break;
		case WSAEFAULT:			printf( "WSAEFAULT\n" );			break;
		case WSAEISCONN:		printf( "WSAEISCONN\n" );			break;
		case WSAENETUNREACH:	printf( "WSAENETUNREACH\n" );		break;
		case WSAENOBUFS:		printf( "WSAENOBUFS\n" );			break;
		case WSAENOTSOCK:		printf( "WSAENOTSOCK\n" );			break;
		case WSAETIMEDOUT:		printf( "WSAETIMEDOUT\n" );			break;
		case WSAEWOULDBLOCK:	printf( "WSAEWOULDBLOCK\n" );		break;
		case WSAEACCES:			printf( "WSAEACCES\n" );			break;
	}
	exit(1);
}

SOCKET startClient( char *servername, int port )
{
	struct hostent		*hp;
	struct sockaddr_in	sin;
	SOCKET				s;
	int					returnValue;
 
	// Next, we need to look up the network address for our host.
	if( ( hp = gethostbyname( servername ) ) == NULL )
	{
		fprintf( stderr, "%s: unknown host.\n", servername );
		exit( 1 );
	}

	// Get a socket to work with.  This socket will be in the 
	// Internet domain, and will be a stream socket.
	if( (s = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "client: socket" );
		exit( 1 );
	}

	// Create the address we will be connecting to.
	// Put it into network byte order.  Also, we use bcopy to copy the 
	// network number.
	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );
	bcopy( hp->h_addr, &sin.sin_addr, hp->h_length );


	// Try to connect to the address.  For this to succeed,
	// the server must already have bound this address, and must have 
	// issued a listen() request.
    printf("Looking for server on port %d...", port );
	returnValue = connect( s, (struct sockaddr *)&sin, sizeof(sin) );
	checkConnect( returnValue );
    printf( "found.\n" );

	return s;
}

void checkSend( int returnValue )
{
	if( returnValue != SOCKET_ERROR ) return;

	switch( WSAGetLastError() )
	{
		case WSANOTINITIALISED:	printf( "WSANOTINITIALISED\n" );	break;
		case WSAENETDOWN:		printf( "WSAENETDOWN\n" );			break;
		case WSAEACCES:			printf( "WSAEACCES\n" );			break;
		case WSAEINTR:			printf( "WSAEINTR\n" );				break;
		case WSAEINPROGRESS:	printf( "WSAEINPROGRESS\n" );		break;
		case WSAEFAULT:			printf( "WSAEFAULT\n" );			break;
		case WSAENETRESET:		printf( "WSAENETRESET\n" );			break;
		case WSAENOBUFS:		printf( "WSAENOBUFS\n" );			break;
		case WSAENOTCONN:		printf( "WSAENOTCONN\n" );			break;
		case WSAENOTSOCK:		printf( "WSAENOTSOCK\n" );			break;
		case WSAEOPNOTSUPP:		printf( "WSAEOPNOTSUPP\n" );		break;
		case WSAESHUTDOWN:		printf( "WSAESHUTDOWN\n" );			break;
		case WSAEWOULDBLOCK:	printf( "WSAEWOULDBLOCK\n" );		break;
		case WSAEMSGSIZE:		printf( "WSAEMSGSIZE\n" );			break;
		case WSAEHOSTUNREACH:	printf( "WSAEHOSTUNREACH\n" );		break;
		case WSAEINVAL:			printf( "WSAEINVAL\n" );			break;
		case WSAECONNABORTED:	printf( "WSAECONNABORTED\n" );		break;
		case WSAECONNRESET:		printf( "WSAECONNRESET\n" );		break;
		case WSAETIMEDOUT:		printf( "WSAETIMEDOUT\n" );			break;
	}

	exit( 1 );
}

void checkRecv( int returnValue )
{
	if( returnValue != SOCKET_ERROR ) return;

	switch( WSAGetLastError() )
	{
		case WSANOTINITIALISED:	printf( "WSANOTINITIALISED\n" );	break;
		case WSAENETDOWN:		printf( "WSAENETDOWN\n" );			break;
		case WSAEFAULT:			printf( "WSAEFAULT\n" );			break;
		case WSAENOTCONN:		printf( "WSAENOTCONN\n" );			break;
		case WSAEACCES:			printf( "WSAEACCES\n" );			break;
		case WSAEINTR:			printf( "WSAEINTR\n" );				break;
		case WSAEINPROGRESS:	printf( "WSAEINPROGRESS\n" );		break;
		case WSAENETRESET:		printf( "WSAENETRESET\n" );			break;
		case WSAENOTSOCK:		printf( "WSAEACCES\n" );			break;
		case WSAEOPNOTSUPP:		printf( "WSAEOPNOTSUPP\n" );		break;
		case WSAESHUTDOWN:		printf( "WSAESHUTDOWN\n" );			break;
		case WSAEWOULDBLOCK:	printf( "WSAEWOULDBLOCK\n" );		break;
		case WSAEMSGSIZE:		printf( "WSAEMSGSIZE\n" );			break;
		case WSAEINVAL:			printf( "WSAEINVAL\n" );			break;
		case WSAECONNABORTED:	printf( "WSAECONNABORTED\n" );		break;
		case WSAETIMEDOUT:		printf( "WSAETIMEDOUT\n" );			break;
		case WSAECONNRESET:		printf( "WSAECONNRESET\n" );		break;
	}
	exit( 1 );
}


#endif