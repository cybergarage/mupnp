/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/net/socket.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/time.h>
/*
#include <mupnp/ssdp/ssdp.h>
*/

#include <mupnp/util/log.h>

#include <string.h>

#if !defined(WINCE)
#include <errno.h>
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
#include <winsock2.h>
#include <ws2tcpip.h>
#if defined(CG_USE_OPENSSL)
#pragma comment(lib, "libeay32MD.lib")
#pragma comment(lib, "ssleay32MD.lib")
#endif
#else
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
#include <typedef.h>
#include <net/sock_com.h>
#include <btron/bsocket.h>
#include <string.h> //for mem___()
#elif defined(ITRON)
#include <kernel.h>
	#if defined(NORTiAPI)
	#include <nonet.h>
	#endif
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
#include <tk/tkernel.h>
#include <btron/kasago.h>
#include <sys/svc/ifkasago.h>
#include <string.h> //for mem___()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#endif
#endif

/*
#define SOCKET_DEBUG
*/

/****************************************
* static variable
****************************************/

static int socketCnt = 0;

#if defined(CG_NET_USE_SOCKET_LIST)
static mUpnpSocketList *socketList;
#endif

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
ttUserInterface kaInterfaceHandle;
#endif

/****************************************
* prototype
****************************************/

#if !defined(ITRON)
BOOL mupnp_socket_tosockaddrin(const char *addr, int port, struct sockaddr_in *sockaddr, BOOL isBindAddr);
#endif

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
BOOL mupnp_socket_tosockaddrinfo(int sockType, const char *addr, int port, struct addrinfo **addrInfo, BOOL isBindAddr);
#endif

#define mupnp_socket_getrawtype(socket) (((socket->type & CG_NET_SOCKET_STREAM) == CG_NET_SOCKET_STREAM) ? SOCK_STREAM : SOCK_DGRAM)

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
#define mupnp_socket_getprototype(socket) ((socket->type == CG_NET_SOCKET_STREAM) ? IPPROTO_TCP : IPPROTO_UDP)
#endif

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
BOOL mupnp_socket_setmulticastinterface(mUpnpSocket *sock, char *ifaddr);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
static int mupnp_socket_getavailableid(int type);
static int mupnp_socket_getavailableport();
#endif

#if defined(ITRON)
BOOL mupnp_socket_initwindowbuffer(mUpnpSocket *sock);
BOOL mupnp_socket_freewindowbuffer(mUpnpSocket *sock);
static ER mupnp_socket_udp_callback(ID cepid, FN fncd, VP parblk);
static ER mupnp_socket_tcp_callback(ID cepid, FN fncd, VP parblk);
static BOOL mupnp_socket_getavailablelocaladdress(T_IPV4EP *localAddr);
#endif

/****************************************
*
* Socket
*
****************************************/

/****************************************
* mupnp_socket_startup
****************************************/

void mupnp_socket_startup()
{
	mupnp_log_debug_l4("Entering...\n");

	if (socketCnt == 0) {
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
		WSADATA wsaData;
		int err;
	
#if defined DEBUG_MEM
		memdiags_probe("STARTING WINSOCK");
#endif

		err = WSAStartup(MAKEWORD(2, 2), &wsaData);

#if defined (WINCE)
	//Theo Beisch unfriendly exit, at least for WINCE
		if (err) {
			//startup error
#if defined DEBUG
			printf("######## WINSOCK startup error %d\n", err);
#endif // DEBUG
		} 

#if defined DEBUG_MEM
		memdiags_probe("WINSOCK STARTED");
#endif

#endif // WINCE
#elif defined(ITRON) && defined(NORTiAPI)
		tcp_ini();
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		kaInterfaceHandle = ka_tfAddInterface(CG_NET_DEFAULT_IFNAME);
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
		// Thanks for Brent Hills (10/26/04)
		signal(SIGPIPE,SIG_IGN);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
		socketList = mupnp_socketlist_new();
#endif	

#if defined(CG_USE_OPENSSL)
		SSL_library_init(); 
#endif
	}
	socketCnt++;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_cleanup
****************************************/

void mupnp_socket_cleanup()
{
	mupnp_log_debug_l4("Entering...\n");

	socketCnt--;
	if (socketCnt <= 0) {
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
		WSACleanup( );
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
		// Thanks for Brent Hills (10/26/04)
		signal(SIGPIPE,SIG_DFL);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
		mupnp_socketlist_delete(socketList);
#endif
	}

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_new
****************************************/

mUpnpSocket *mupnp_socket_new(int type)
{
	mUpnpSocket *sock;

	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_startup();

	sock = (mUpnpSocket *)malloc(sizeof(mUpnpSocket));

	if ( NULL != sock )
	{
#if defined(WIN32) && !defined(__CYGWIN__) && !defined(__MINGW32__) && !defined(ITRON)
		sock->id = INVALID_SOCKET;
#else
		sock->id = -1;
#endif

		mupnp_socket_settype(sock, type);
		mupnp_socket_setdirection(sock, CG_NET_SOCKET_NONE);

		sock->ipaddr = mupnp_string_new();

		mupnp_socket_setaddress(sock, "");
		mupnp_socket_setport(sock, -1);

#if defined(ITRON)
		sock->sendWinBuf = NULL;
		sock->recvWinBuf = NULL;
#endif

#if defined(CG_USE_OPENSSL)
		sock->ctx = NULL;
		sock->ssl = NULL;
#endif
	}

	mupnp_log_debug_l4("Leaving...\n");

	return sock;
}

/****************************************
* mupnp_socket_delete
****************************************/

BOOL mupnp_socket_delete(mUpnpSocket *sock)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_close(sock);
	mupnp_string_delete(sock->ipaddr);
#if defined(ITRON)
	mupnp_socket_freewindowbuffer(sock);
#endif
	free(sock);
	mupnp_socket_cleanup();

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_socket_isbound
****************************************/

BOOL mupnp_socket_isbound(mUpnpSocket *sock)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_log_debug_l4("Leaving...\n");

#if defined(WIN32) && !defined(__CYGWIN__) && !defined(__MINGW32__) && !defined(ITRON)
	return (sock->id != INVALID_SOCKET) ? TRUE: FALSE;
#else
	return (0 < sock->id) ? TRUE : FALSE;
#endif
}

/****************************************
* mupnp_socket_setid
****************************************/

void mupnp_socket_setid(mUpnpSocket *socket, SOCKET value)
{
#if defined(WIN32) || defined(HAVE_IP_PKTINFO) || (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && defined(HAVE_SO_NOSIGPIPE)
	int on=1;
#endif

	mupnp_log_debug_l4("Entering...\n");

	socket->id=value;

#if defined(WIN32) || defined(HAVE_IP_PKTINFO)
	if ( CG_NET_SOCKET_DGRAM == mupnp_socket_gettype(socket) ) 
		setsockopt(socket->id, IPPROTO_IP, IP_PKTINFO,  &on, sizeof(on));
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && defined(HAVE_SO_NOSIGPIPE)
	setsockopt(socket->id, SOL_SOCKET, SO_NOSIGPIPE,  &on, sizeof(on));
#endif
	
	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_close
****************************************/

BOOL mupnp_socket_close(mUpnpSocket *sock)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_socket_isbound(sock) == FALSE)
		return TRUE;

#if defined(CG_USE_OPENSSL)
	if (mupnp_socket_isssl(sock) == TRUE) {
		if (sock->ctx) {
			SSL_shutdown(sock->ssl); 
			SSL_free(sock->ssl);
			sock->ssl = NULL;
		}
		if (sock->ctx) {
			SSL_CTX_free(sock->ctx);
			sock->ctx = NULL;
		}
	}
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(ITRON)
	#if !defined(WINCE)
	WSAAsyncSelect(sock->id, NULL, 0, FD_CLOSE);
	#endif
	shutdown(sock->id, SD_BOTH );
#if defined WINCE
	{
		int nRet = 1;
		char achDiscard[256];
		while (nRet && (nRet != SOCKET_ERROR)){
			if (nRet>0) {
				achDiscard[nRet]=(char)0;
#if defined DEBUG_SOCKET
				printf("DUMMY READ WHILE CLOSING SOCKET \n%s\n",achDiscard);
#endif
			}
			nRet = recv(sock->id,achDiscard,128,0);
		}
	}
#endif
	closesocket(sock->id);
	#if !defined(__CYGWIN__) && !defined(__MINGW32__)
	sock->id = INVALID_SOCKET;
	#else
	sock->id = -1;
	#endif
#else
	#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	so_shutdown(sock->id, 2);
	so_close(sock->id);
	#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ka_tfClose(sock->id);
	#elif defined(ITRON)
	if (mupnp_socket_issocketstream(sock) == TRUE) {
		tcp_can_cep(sock->id, TFN_TCP_ALL);
		tcp_sht_cep(sock->id);
		tcp_del_cep(sock->id);
		tcp_cls_cep(sock->id, TMO_FEVR);
		tcp_del_rep(sock->id);
	}
	else {
		udp_can_cep(sock->id, TFN_UDP_ALL);
		udp_del_cep(sock->id);
	}
	#else
	int flag = fcntl(sock->id, F_GETFL, 0);
	if (0 <= flag)
		fcntl(sock->id, F_SETFL, flag | O_NONBLOCK);
	shutdown(sock->id, 2);
	close(sock->id);
	#endif
	sock->id = -1;
#endif

	mupnp_socket_setaddress(sock, "");
	mupnp_socket_setport(sock, -1);

	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_listen
****************************************/

BOOL mupnp_socket_listen(mUpnpSocket *sock)
{
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	ERR ret = so_listen(sock->id, 10);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ERR ret = ka_listen(sock->id, 10);
#elif defined(ITRON)
	/**** Not Supported ****/
	int ret = 0;
#else
	int ret = listen(sock->id, SOMAXCONN);
#endif

	mupnp_log_debug_l4("Entering...\n");

	return (ret == 0) ? TRUE: FALSE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_bind
****************************************/

BOOL mupnp_socket_bind(mUpnpSocket *sock, int bindPort, const char *bindAddr, BOOL bindFlag, BOOL reuseFlag)
{
#if defined(BTRON) || defined(TENGINE)
	struct sockaddr_in sockaddr;
	ERR ret;
#elif defined(ITRON)
	T_UDP_CCEP udpccep = { 0, { IPV4_ADDRANY, UDP_PORTANY }, (FP)mupnp_socket_udp_callback };
	T_TCP_CREP tcpcrep = { 0, { IPV4_ADDRANY, 0 } };
	T_TCP_CCEP tcpccep = { 0, sock->sendWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, sock->recvWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, (FP)mupnp_socket_tcp_callback };
#else
	struct addrinfo *addrInfo;
	int ret;
#endif

	mupnp_log_debug_l4("Entering...\n");

	if (bindPort <= 0 /* || bindAddr == NULL*/)
		return FALSE;

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	if (mupnp_socket_tosockaddrin(bindAddr, bindPort, &sockaddr, bindFlag) == FALSE)
		return FALSE;
   	mupnp_socket_setid(sock, so_socket(PF_INET, mupnp_socket_getrawtype(sock), 0));
	if (sock->id < 0)
		return FALSE;
	if (reuseFlag == TRUE) {
		if (mupnp_socket_setreuseaddress(sock, TRUE) == FALSE) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
	ret = so_bind(sock->id, (SOCKADDR *)&sockaddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		mupnp_socket_close(sock);
		return FALSE;
	}
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	if (mupnp_socket_tosockaddrin(bindAddr, bindPort, &sockaddr, bindFlag) == FALSE)
		return FALSE;
	mupnp_socket_setid(sock, ka_socket( PF_INET, mupnp_socket_getrawtype(sock), mupnp_socket_getprototype(sock)));
	if (sock->id < 0)
		return FALSE;
	/*
	if (mupnp_socket_setmulticastinterface(sock, bindAddr) == FALSE)
		return FALSE;
	*/
	if (reuseFlag == TRUE) {
		if (mupnp_socket_setreuseaddress(sock, TRUE) == FALSE) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
	ret = ka_bind(sock->id, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		mupnp_socket_close(sock);
		return FALSE;
	}
#elif defined(ITRON)
	mupnp_socket_setid(sock, mupnp_socket_getavailableid(mupnp_socket_issocketstream(sock)));
	if (sock->id < 0)
		return FALSE;
	if (mupnp_socket_issocketstream(sock) == TRUE) {
		if (bindAddr != NULL)
			tcpcrep.myaddr.ipaddr = ascii_to_ipaddr(bindAddr);
		tcpcrep.myaddr.ipaddr = htons(bindPort);
		if (tcp_cre_rep(sock->id, &tcpcrep) != E_OK) {
			mupnp_socket_close(sock);
			return FALSE;
		}
		if (tcp_cre_cep(sock->id, &tcpccep) != E_OK) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
	else {
		if (bindAddr != NULL)
			udpccep.myaddr.ipaddr = ascii_to_ipaddr(bindAddr);
		udpccep.myaddr.ipaddr = htons(bindPort);
		if (udp_cre_cep(sock->id, &udpccep) != E_OK) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
#else
	if (mupnp_socket_tosockaddrinfo(mupnp_socket_getrawtype(sock), bindAddr, bindPort, &addrInfo, bindFlag) == FALSE)
		return FALSE;
	mupnp_socket_setid(sock, socket(addrInfo->ai_family, addrInfo->ai_socktype, 0));
	if (sock->id== -1) {
		mupnp_socket_close(sock);
		return FALSE;
	}
	if (reuseFlag == TRUE) {
		if (mupnp_socket_setreuseaddress(sock, TRUE) == FALSE) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
	ret = bind(sock->id, addrInfo->ai_addr, addrInfo->ai_addrlen);
	freeaddrinfo(addrInfo);
#endif

#if !defined(ITRON)
	if (ret != 0)
		return FALSE;
#endif

	mupnp_socket_setdirection(sock, CG_NET_SOCKET_SERVER);
	mupnp_socket_setaddress(sock, bindAddr);
	mupnp_socket_setport(sock, bindPort);

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_socket_accept
****************************************/

BOOL mupnp_socket_accept(mUpnpSocket *serverSock, mUpnpSocket *clientSock)
{
	struct sockaddr_in sockaddr;
	socklen_t socklen;
	char localAddr[CG_NET_SOCKET_MAXHOST];
	char localPort[CG_NET_SOCKET_MAXSERV];
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	struct sockaddr_in sockaddr;
	W nLength = sizeof(struct sockaddr_in);
	mupnp_socket_setid(clientSock, so_accept(serverSock->id, (SOCKADDR *)&sockaddr, &nLength));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct sockaddr_in sockaddr;
	int nLength = sizeof(struct sockaddr_in);
	mupnp_socket_setid(clientSock, ka_accept(serverSock->id, (struct sockaddr *)&sockaddr, &nLength));
#elif defined(ITRON)
	T_IPV4EP dstAddr;
	if (tcp_acp_cep(serverSock->id, serverSock->id, &dstAddr, TMO_FEVR) != E_OK)
		return FALSE;
	mupnp_socket_setid(clientSock, mupnp_socket_getid(serverSock));
#else
	struct sockaddr_storage sockClientAddr;
	socklen_t nLength = sizeof(sockClientAddr);
	mupnp_socket_setid(clientSock, accept(serverSock->id, (struct sockaddr *)&sockClientAddr, &nLength));
#endif

	mupnp_log_debug_l4("Entering...\n");

#ifdef SOCKET_DEBUG
mupnp_log_debug_s("clientSock->id = %d\n", clientSock->id);
#endif
	
#if defined (WIN32) && !defined(ITRON)
	if (clientSock->id == INVALID_SOCKET)
		return FALSE;
#else
	if (clientSock->id < 0)
		return FALSE;
#endif
	
	mupnp_socket_setaddress(clientSock, mupnp_socket_getaddress(serverSock));
	mupnp_socket_setport(clientSock, mupnp_socket_getport(serverSock));
	socklen = sizeof(struct sockaddr_in);
	
	if (getsockname(clientSock->id, (struct sockaddr *)&sockaddr, &socklen) == 0 &&
	    getnameinfo((struct sockaddr *)&sockaddr, socklen, localAddr, sizeof(localAddr), 
			localPort, sizeof(localPort), NI_NUMERICHOST | NI_NUMERICSERV) == 0) 
	{
		/* Set address for the sockaddr to real addr */
		mupnp_socket_setaddress(clientSock, localAddr);
	}
	
#ifdef SOCKET_DEBUG
mupnp_log_debug_s("clientSock->id = %s\n", mupnp_socket_getaddress(clientSock));
mupnp_log_debug_s("clientSock->id = %d\n", mupnp_socket_getport(clientSock));
#endif
	
	
	return TRUE;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_socket_connect
****************************************/

BOOL mupnp_socket_connect(mUpnpSocket *sock, const char *addr, int port)
{
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	ERR ret;
	struct sockaddr_in sockaddr;
	if (mupnp_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return FALSE;
	if (mupnp_socket_isbound(sock) == FALSE)
	   	mupnp_socket_setid(sock, so_socket(PF_INET, mupnp_socket_getrawtype(sock), 0));
	ret = so_connect(sock->id, (SOCKADDR *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ERR ret;
	struct sockaddr_in sockaddr;
	if (mupnp_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return FALSE;
	if (mupnp_socket_isbound(sock) == FALSE)
	   	mupnp_socket_setid(sock, ka_socket(PF_INET, mupnp_socket_getrawtype(sock), mupnp_socket_getprototype(sock)));
	ret = ka_connect(sock->id, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(ITRON)
	T_TCP_CCEP tcpccep = { 0, sock->sendWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, sock->recvWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, (FP)mupnp_socket_tcp_callback };
	T_IPV4EP localAddr;
	T_IPV4EP dstAddr;
	ER ret;
	if (mupnp_socket_getavailablelocaladdress(&localAddr) == FALSE)
		return FALSE;
	if (mupnp_socket_isbound(sock) == FALSE) {
		mupnp_socket_initwindowbuffer(sock);
		mupnp_socket_setid(sock, mupnp_socket_getavailableid(mupnp_socket_issocketstream(sock)));
		if (tcp_cre_cep(sock->id, &tcpccep) != E_OK)
			return FALSE;
	}
	dstAddr.ipaddr = ascii_to_ipaddr(addr);
	dstAddr.portno = htons(port);
	ret = tcp_con_cep(sock->id, &localAddr, &dstAddr, TMO_FEVR);
	if (ret == E_OK) {
		mupnp_socket_setaddress(sock, ""/*ipaddr_to_ascii(localAddr.ipaddr)*/);
		mupnp_socket_setport(sock, ntohs(localAddr.portno));
		ret = 0;
	}
	else 
		ret = -1;
#else
	struct addrinfo *toaddrInfo;
	int ret;
	if (mupnp_socket_tosockaddrinfo(mupnp_socket_getrawtype(sock), addr, port, &toaddrInfo, TRUE) == FALSE)
		return FALSE;
	if (mupnp_socket_isbound(sock) == FALSE)
		mupnp_socket_setid(sock, socket(toaddrInfo->ai_family, toaddrInfo->ai_socktype, 0));
	ret = connect(sock->id, toaddrInfo->ai_addr, toaddrInfo->ai_addrlen);
	freeaddrinfo(toaddrInfo);
#endif

	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_setdirection(sock, CG_NET_SOCKET_CLIENT);

#if defined(CG_USE_OPENSSL)
	if (mupnp_socket_isssl(sock) == TRUE) {
		sock->ctx = SSL_CTX_new( SSLv23_client_method());
		sock->ssl = SSL_new(sock->ctx);
		if (SSL_set_fd(sock->ssl, mupnp_socket_getid(sock)) == 0) {
			mupnp_socket_close(sock);
			return FALSE;
		}
		if (SSL_connect(sock->ssl) < 1) {
			mupnp_socket_close(sock);
			return FALSE;
		}
	}
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return (ret == 0) ? TRUE : FALSE;
}

/****************************************
* mupnp_socket_read
****************************************/

ssize_t mupnp_socket_read(mUpnpSocket *sock, char *buffer, size_t bufferLen)
{
	ssize_t recvLen;

#if defined(CG_USE_OPENSSL)
	if (mupnp_socket_isssl(sock) == FALSE) {
#endif

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
	recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
	recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(CG_USE_OPENSSL)
	}
	else {
		recvLen = SSL_read(sock->ssl, buffer, bufferLen);
	}
#endif

	mupnp_log_debug_l4("Entering...\n");

#ifdef SOCKET_DEBUG
	if (0 <= recvLen)
		buffer[recvLen] = '\0';
  mupnp_log_debug_s("r %d : %s\n", recvLen, (0 <= recvLen) ? buffer : "");
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return recvLen;
}

/****************************************
* mupnp_socket_write
****************************************/

#define CG_NET_SOCKET_SEND_RETRY_CNT 10
#define CG_NET_SOCKET_SEND_RETRY_WAIT_MSEC 20

size_t mupnp_socket_write(mUpnpSocket *sock, const char *cmd, size_t cmdLen)
{
	ssize_t nSent;
	size_t nTotalSent = 0;
	size_t cmdPos = 0;
	int retryCnt = 0;

	mupnp_log_debug_l4("Entering...\n");

	if (cmdLen <= 0)
		return 0;

	do {
#if defined(CG_USE_OPENSSL)
		if (mupnp_socket_isssl(sock) == FALSE) {
#endif

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
		nSent = so_send(sock->id, (B*)(cmd + cmdPos), cmdLen, 0);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		nSent = ka_send(sock->id, (B*)(cmd + cmdPos), cmdLen, 0);
#elif defined(ITRON)
		nSent = tcp_snd_dat(sock->id, cmd + cmdPos, cmdLen, TMO_FEVR);
#else
		nSent = send(sock->id, cmd + cmdPos, cmdLen, 0);
#endif
			
#if defined(CG_USE_OPENSSL)
		}
		else {
			nSent = SSL_write(sock->ssl, cmd + cmdPos, cmdLen);
		}
#endif

		/* Try to re-send in case sending has failed */
		if (nSent <= 0)
		{
			retryCnt++;
			if (CG_NET_SOCKET_SEND_RETRY_CNT < retryCnt)
			{
				/* Must reset this because otherwise return
				   value is interpreted as something else than
				   fault and this function loops forever */
				nTotalSent = 0;
				break;
			}

			mupnp_wait(CG_NET_SOCKET_SEND_RETRY_WAIT_MSEC);
		}
		else
		{
			nTotalSent += nSent;
			cmdPos += nSent;
			cmdLen -= nSent;
			retryCnt = 0;
		}

	} while (0 < cmdLen);

#ifdef SOCKET_DEBUG
mupnp_log_debug_s("w %d : %s\n", nTotalSent, ((cmd != NULL) ? cmd : ""));
#endif
	
	mupnp_log_debug_l4("Leaving...\n");

	return nTotalSent;
}
/****************************************
* mupnp_socket_readline
****************************************/

ssize_t mupnp_socket_readline(mUpnpSocket *sock, char *buffer, size_t bufferLen)
{
	ssize_t readCnt;
	ssize_t readLen;
	char c;
	
	mupnp_log_debug_l4("Entering...\n");

	readCnt = 0;
	while (readCnt < (bufferLen-1)) {
		readLen = mupnp_socket_read(sock, &buffer[readCnt], sizeof(char));
		if (readLen <= 0)
			return -1;
		readCnt++;
		if (buffer[readCnt-1] == CG_SOCKET_LF)
			break;
	}
	buffer[readCnt] = '\0';
	if (buffer[readCnt-1] != CG_SOCKET_LF) {
		do {
			readLen = mupnp_socket_read(sock, &c, sizeof(char));
			if (readLen <= 0)
				break;
		} while (c != CG_SOCKET_LF);
	}

	mupnp_log_debug_l4("Leaving...\n");

	return readCnt;	
}

/****************************************
* mupnp_socket_skip
****************************************/

size_t mupnp_socket_skip(mUpnpSocket *sock, size_t skipLen)
{
	ssize_t readCnt;
	ssize_t readLen;
	char c;
	
	mupnp_log_debug_l4("Entering...\n");

	readCnt = 0;
	while (readCnt < skipLen) {
		readLen = mupnp_socket_read(sock, &c, sizeof(char));
		if (readLen <= 0)
			break;
		readCnt++;
	}

	mupnp_log_debug_l4("Leaving...\n");

	return readCnt;
}

/****************************************
* mupnp_socket_sendto
****************************************/

size_t mupnp_socket_sendto(mUpnpSocket *sock, const char *addr, int port, const char *data, size_t dataLen)
{
#if defined(BTRON) || defined(TENGINE)
	struct sockaddr_in sockaddr;
#elif defined(ITRON)
	T_IPV4EP dstaddr;
	T_UDP_CCEP udpccep = { 0, { IPV4_ADDRANY, UDP_PORTANY }, (FP)mupnp_socket_udp_callback };
#else
	struct addrinfo *addrInfo;
#endif
	ssize_t sentLen;
	BOOL isBoundFlag;

	mupnp_log_debug_l4("Entering...\n");

	if (data == NULL)
		return 0;
	if (dataLen <= 0)
		dataLen = mupnp_strlen(data);
	if (dataLen <= 0)
		return 0;

	isBoundFlag = mupnp_socket_isbound(sock);
	sentLen = -1;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	if (mupnp_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE)
	   	mupnp_socket_setid(sock, so_socket(PF_INET, mupnp_socket_getrawtype(sock), 0));
	if (0 <= sock->id)
		sentLen = so_sendto(sock->id, (B*)data, dataLen, 0, (SOCKADDR*)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	if (mupnp_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE) {
	   	mupnp_socket_setid(sock, ka_socket(PF_INET, mupnp_socket_getrawtype(sock), mupnp_socket_getprototype(sock)));
		mupnp_socket_setmulticastinterface(sock, NULL);
	}
	if (0 <= sock->id)
		sentLen = ka_sendto(sock->id, data, dataLen, 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(ITRON)
	if (isBoundFlag == FALSE) {
		mupnp_socket_setid(sock, mupnp_socket_getavailableid(mupnp_socket_issocketstream(sock)));
		if (sock->id < 0)
			return FALSE;
		if (udp_cre_cep(sock->id, &udpccep) != E_OK)
			return FALSE;
	}
	dstaddr.ipaddr = ascii_to_ipaddr(addr);
	dstaddr.portno = htons(port);
	sentLen = udp_snd_dat(sock->id, &dstaddr, data, dataLen, TMO_FEVR);
#else
	if (mupnp_socket_tosockaddrinfo(mupnp_socket_getrawtype(sock), addr, port, &addrInfo, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE)
		mupnp_socket_setid(sock, socket(addrInfo->ai_family, addrInfo->ai_socktype, 0));
	
	/* Setting multicast time to live in any case to default */
	mupnp_socket_setmulticastttl(sock, CG_NET_SOCKET_MULTICAST_DEFAULT_TTL);
	
	if (0 <= sock->id)
		sentLen = sendto(sock->id, data, dataLen, 0, addrInfo->ai_addr, addrInfo->ai_addrlen);
	freeaddrinfo(addrInfo);
#endif

	if (isBoundFlag == FALSE)
		mupnp_socket_close(sock);

#ifdef SOCKET_DEBUG
mupnp_log_debug_s("sentLen : %d\n", sentLen);
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return sentLen;
}

/****************************************
* mupnp_socket_recv
****************************************/

ssize_t mupnp_socket_recv(mUpnpSocket *sock, mUpnpDatagramPacket *dgmPkt)
{
	ssize_t recvLen = 0;
	char recvBuf[CG_NET_SOCKET_DGRAM_RECV_BUFSIZE+1];
	char remoteAddr[CG_NET_SOCKET_MAXHOST];
	char remotePort[CG_NET_SOCKET_MAXSERV];
	char *localAddr;

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	struct sockaddr_in from;
	W fromLen = sizeof(from);
	recvLen = so_recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct sockaddr_in from;
	int fromLen = sizeof(from);
	recvLen = ka_recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#elif defined(ITRON)
	T_IPV4EP remoteHost;
	recvLen = udp_rcv_dat(sock->id, &remoteHost, recvBuf, sizeof(recvBuf)-1, TMO_FEVR);
#else
	struct sockaddr_storage from;
	socklen_t fromLen = sizeof(from);
	recvLen = recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#endif

	mupnp_log_debug_l4("Entering...\n");

	if (recvLen <= 0)
		return 0;

	recvBuf[recvLen] = '\0';
	mupnp_socket_datagram_packet_setdata(dgmPkt, recvBuf);

	mupnp_socket_datagram_packet_setlocalport(dgmPkt, mupnp_socket_getport(sock));
	mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, "");
	mupnp_socket_datagram_packet_setremoteport(dgmPkt, 0);

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	mupnp_socket_datagram_packet_setlocaladdress(dgmPkt, mupnp_socket_getaddress(sock));
	mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, inet_ntoa(from.sin_addr));
	mupnp_socket_datagram_packet_setremoteport(dgmPkt, ntohl(from.sin_port));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	mupnp_socket_datagram_packet_setlocaladdress(dgmPkt, mupnp_socket_getaddress(sock));
	ka_tfInetToAscii((unsigned long)from.sin_addr.s_addr, remoteAddr);
	mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
	mupnp_socket_datagram_packet_setremoteport(dgmPkt, ka_ntohl(from.sin_port));
#elif defined(ITRON)
	mupnp_socket_datagram_packet_setlocaladdress(dgmPkt, mupnp_socket_getaddress(sock));
	ipaddr_to_ascii(remoteAddr, remoteHost.ipaddr);
	mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
	mupnp_socket_datagram_packet_setremoteport(dgmPkt, ntohs(remoteHost.portno));
#else

	if (getnameinfo((struct sockaddr *)&from, fromLen, remoteAddr, sizeof(remoteAddr), remotePort, sizeof(remotePort), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
		mupnp_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
		mupnp_socket_datagram_packet_setremoteport(dgmPkt, mupnp_str2int(remotePort));
	}

	mupnp_log_debug_s("From pointer %p\n", &from);
	localAddr = mupnp_net_selectaddr((struct sockaddr *)&from);
	mupnp_socket_datagram_packet_setlocaladdress(dgmPkt, localAddr);
	free(localAddr);

#endif

	mupnp_log_debug_l4("Leaving...\n");

	return recvLen;
}

/****************************************
* mupnp_socket_setreuseaddress
****************************************/

BOOL mupnp_socket_setreuseaddress(mUpnpSocket *sock, BOOL flag)
{
	int sockOptRet;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	B optval
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	int optval;
#elif defined (WIN32)
	BOOL optval;
#else
	int optval;
#endif

	mupnp_log_debug_l4("Entering...\n");

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = so_setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (B *)&optval, sizeof(optval));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = ka_setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
#else
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
	#if defined(USE_SO_REUSEPORT) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
	if (sockOptRet == 0) {
    sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_REUSEPORT, (const char *)&optval, sizeof(optval));
  }
	#endif
#endif
	mupnp_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* mupnp_socket_setmulticastttl
****************************************/

BOOL mupnp_socket_setmulticastttl(mUpnpSocket *sock, int ttl)
{
	int sockOptRet;
	int ttl_;
	unsigned int len=0;
	
	mupnp_log_debug_l4("Entering...\n");

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	sockOptRet = so_setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (B *)&ttl, sizeof(ttl));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockOptRet = ka_setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl, sizeof(ttl));
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	sockOptRet = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl, sizeof(ttl));
#else
	mupnp_log_debug("Setting multicast time to live to %d\n", ttl);
	sockOptRet = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const unsigned char *)&ttl, sizeof(ttl));
	if (sockOptRet != 0)
	{
		mupnp_log_debug("setsockopt() failed with errno %d: %s, fd:%d\n", errno, strerror(errno),sock->id);
	}
	else
	{
		len = sizeof(ttl_);
		getsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, &ttl_, (socklen_t*)&len);
		mupnp_log_debug("Multicast time to live is %i\n", ttl_);
	}
#endif
	mupnp_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* mupnp_socket_settimeout
****************************************/

BOOL mupnp_socket_settimeout(mUpnpSocket *sock, int sec)
{
	int sockOptRet;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	sockOptRet = -1; /* TODO: Implement this */
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockOptRet = -1; /* TODO: Implement this */
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	mupnp_log_debug_l4("Entering...\n");

	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
	if (sockOptRet == 0)
		sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
#else
	struct timeval timeout;
	timeout.tv_sec = (time_t)sec;
	timeout.tv_usec = 0;

	mupnp_log_debug_l4("Entering...\n");

	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
	if (sockOptRet == 0)
		sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
#endif

	mupnp_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* mupnp_socket_joingroup
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

BOOL mupnp_socket_joingroup(mUpnpSocket *sock, const char *mcastAddr, const char *ifAddr)
{
	struct ip_mreq ipmr;
	u_long ifInetAddr = ka_inet_addr(ifAddr);
	
	BOOL joinSuccess;
	int sockOptRetCode;
	
	mupnp_log_debug_l4("Entering...\n");

	joinSuccess = TRUE;
	
	ka_inet_pton( AF_INET, mcastAddr, &(ipmr.imr_multiaddr) );
	memcpy(&ipmr.imr_interface, &ifInetAddr, sizeof(struct in_addr));
	sockOptRetCode = ka_setsockopt(sock->id, IP_PROTOIP, IPO_ADD_MEMBERSHIP, (char *)&ipmr, sizeof(ipmr));

	if (sockOptRetCode != 0)
		joinSuccess = FALSE;

	mupnp_string_setvalue(sock->ipaddr, (joinSuccess == TRUE) ? ifAddr : NULL);

	mupnp_log_debug_l4("Leaving...\n");

	return joinSuccess;
}

#elif defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))

BOOL mupnp_socket_joingroup(mUpnpSocket *sock, char *mcastAddr, char *ifAddr)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_log_debug_l4("Leaving...\n");

	/**** Not Suppoted ****/
	return TRUE;

}

#elif defined(ITRON)

BOOL mupnp_socket_joingroup(mUpnpSocket *sock, char *mcastAddr, char *ifAddr)
{
	mupnp_log_debug_l4("Entering...\n");

	UW optval;
	ER ret;

	optval = ascii_to_ipaddr(mcastAddr);
	ret = udp_set_opt(sock->id, IP_ADD_MEMBERSHIP, &optval, sizeof(optval));

	mupnp_log_debug_l4("Leaving...\n");

	return (ret == E_OK) ? TRUE : FALSE;
}
#else

BOOL mupnp_socket_joingroup(mUpnpSocket *sock, const char *mcastAddr, const char *ifAddr)
{
	struct addrinfo hints;
	struct addrinfo *mcastAddrInfo, *ifAddrInfo;
	
	/**** for IPv6 ****/
	struct ipv6_mreq ipv6mr;
	struct sockaddr_in6 toaddr6, ifaddr6;
	int scopeID;
	
	/**** for IPv4 ****/	
	struct ip_mreq ipmr;
	struct sockaddr_in toaddr, ifaddr;
	
	BOOL joinSuccess;
	int sockOptRetCode;
	
	mupnp_log_debug_l4("Entering...\n");

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags= AI_NUMERICHOST | AI_PASSIVE;

	if (getaddrinfo(mcastAddr, NULL, &hints, &mcastAddrInfo) != 0) 
		return FALSE;

	if (getaddrinfo(ifAddr, NULL, &hints, &ifAddrInfo) != 0) {
		freeaddrinfo(mcastAddrInfo);
		return FALSE;
	}

	joinSuccess = TRUE;
	
	if (mupnp_net_isipv6address(mcastAddr) == TRUE) {
		memcpy(&toaddr6, mcastAddrInfo->ai_addr, sizeof(struct sockaddr_in6));
		memcpy(&ifaddr6, ifAddrInfo->ai_addr, sizeof(struct sockaddr_in6));
		ipv6mr.ipv6mr_multiaddr = toaddr6.sin6_addr;	
		scopeID = mupnp_net_getipv6scopeid(ifAddr);
		ipv6mr.ipv6mr_interface = scopeID /*if_nametoindex*/;
		
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&scopeID, sizeof(scopeID));

		if (sockOptRetCode != 0)
			joinSuccess = FALSE;

		sockOptRetCode = setsockopt(sock->id, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *)&ipv6mr, sizeof(ipv6mr));

		if (sockOptRetCode != 0)
			joinSuccess = FALSE;

	}
	else {
		memcpy(&toaddr, mcastAddrInfo->ai_addr, sizeof(struct sockaddr_in));
		memcpy(&ifaddr, ifAddrInfo->ai_addr, sizeof(struct sockaddr_in));
		memcpy(&ipmr.imr_multiaddr.s_addr, &toaddr.sin_addr, sizeof(struct in_addr));
		memcpy(&ipmr.imr_interface.s_addr, &ifaddr.sin_addr, sizeof(struct in_addr));
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ipmr.imr_interface.s_addr, sizeof(struct in_addr));
		if (sockOptRetCode != 0)
			joinSuccess = FALSE;
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipmr, sizeof(ipmr));
		
		if (sockOptRetCode != 0)
			joinSuccess = FALSE;
	}

	freeaddrinfo(mcastAddrInfo);
	freeaddrinfo(ifAddrInfo);

	mupnp_log_debug_l4("Leaving...\n");

	return joinSuccess;
}

#endif

/****************************************
* mupnp_socket_tosockaddrin
****************************************/

#if !defined(ITRON)

BOOL mupnp_socket_tosockaddrin(const char *addr, int port, struct sockaddr_in *sockaddr, BOOL isBindAddr)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_startup();

	memset(sockaddr, 0, sizeof(struct sockaddr_in));

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = ka_htonl(INADDR_ANY);
	sockaddr->sin_port = ka_htons((unsigned short)port);
#else
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr->sin_port = htons((unsigned short)port);
#endif

	if (isBindAddr == TRUE) {
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
		sockaddr->sin_addr.s_addr = inet_addr(addr);
		if (sockaddr->sin_addr.s_addr == -1 /*INADDR_NONE*/) {
			struct hostent hent;
			B hostBuf[HBUFLEN];
			if (so_gethostbyname((B*)addr, &hent, hostBuf) != 0)
				return FALSE;
			memcpy(&(sockaddr->sin_addr), hent.h_addr, hent.h_length);
		}
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		sockaddr->sin_addr.s_addr = ka_inet_addr(addr);
#else
		sockaddr->sin_addr.s_addr = inet_addr(addr);
		if (sockaddr->sin_addr.s_addr == INADDR_NONE) {
			struct hostent *hent = gethostbyname(addr);
			if (hent == NULL)
				return FALSE;
			memcpy(&(sockaddr->sin_addr), hent->h_addr, hent->h_length);
		}
#endif
	}

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}
#endif

/****************************************
* mupnp_socket_tosockaddrinfo
****************************************/

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)

BOOL mupnp_socket_tosockaddrinfo(int sockType, const char *addr, int port, struct addrinfo **addrInfo, BOOL isBindAddr)
{
#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct addrinfo hints;
	char portStr[32];
#else
	struct addrinfo hints;
	char portStr[32];
	int errorn;
#endif

	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_startup();

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = sockType;
	hints.ai_flags= 0; /*AI_NUMERICHOST | AI_PASSIVE*/;
	sprintf(portStr, "%d", port);
	if (ka_getaddrinfo(addr, portStr, &hints, addrInfo) != 0)
		return FALSE;
	if (isBindAddr == TRUE)
		return TRUE;
	hints.ai_family = (*addrInfo)->ai_family;
	ka_freeaddrinfo(*addrInfo);
	if (ka_getaddrinfo(NULL, portStr, &hints, addrInfo) != 0)
		return FALSE;
	return TRUE;
#else
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = sockType;
	hints.ai_flags= /*AI_NUMERICHOST | */AI_PASSIVE;
	sprintf(portStr, "%d", port);
	mupnp_log_debug("Address: %s, port: %s\n", addr, portStr);
	if ( (errorn = getaddrinfo(addr, portStr, &hints, addrInfo)) != 0) {
#if !defined(WINCE)
		mupnp_log_debug_s("ERROR: %s\n", gai_strerror(errorn));
		mupnp_log_debug_s("SERROR: %s\n", strerror(errno));
#endif
		return FALSE;
	}
	if (isBindAddr == TRUE)
		return TRUE;
	hints.ai_family = (*addrInfo)->ai_family;
	freeaddrinfo(*addrInfo);
	if ((errorn = getaddrinfo(NULL, portStr, &hints, addrInfo)) != 0) {
#if !defined(WINCE)
		mupnp_log_debug_s("ERROR: %s\n", gai_strerror(errorn));
		mupnp_log_debug_s("SERROR: %s\n", strerror(errno));
#endif
		return FALSE;
	}
	return TRUE;
#endif
 }

#endif

/****************************************
* mupnp_socket_setmulticastinterface
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

BOOL mupnp_socket_setmulticastinterface(mUpnpSocket *sock, char *ifaddr)
{
	struct sockaddr_in sockaddr;
	BOOL sockAddrSuccess;
	int optSuccess;
	mUpnpNetworkInterfaceList *netIfList;
	mUpnpNetworkInterface *netIf;
	int netIfCnt;

	mupnp_log_debug_l4("Entering...\n");

	netIfList = NULL;
	if (mupnp_strlen(ifaddr) <= 0) {
		netIfList = mupnp_net_interfacelist_new();
		netIfCnt = mupnp_net_gethostinterfaces(netIfList);
		if (netIfCnt <= 0) {
			mupnp_net_interfacelist_delete(netIfList);
			return FALSE;
		}
		netIf = mupnp_net_interfacelist_gets(netIfList);
		ifaddr = mupnp_net_interface_getaddress(netIf);
	}

	sockAddrSuccess = mupnp_socket_tosockaddrin(ifaddr, 0, &sockaddr, TRUE);
	if (netIfList != NULL)
		mupnp_net_interfacelist_delete(netIfList);
	if (sockAddrSuccess == FALSE)
		return FALSE;

	optSuccess = ka_setsockopt(sock->id, IP_PROTOIP, IPO_MULTICAST_IF, (const char *)&sockaddr.sin_addr, sizeof(sockaddr.sin_addr));
	if (optSuccess != 0)
		return FALSE;

	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

#endif

/****************************************
* mupnp_socket_getavailableid
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

static int mupnp_socket_getavailableid(int type)
{
	mUpnpSocket *sock;
	int id;
	BOOL isIDUsed;

	mupnp_log_debug_l4("Entering...\n");

	id = 0;
	do {
		id++;
		isIDUsed = FALSE;
		for (sock = mupnp_socketlist_gets(socketList); sock != NULL; sock = mupnp_socket_next(sock)) {
			if (mupnp_socket_gettype(sock) != type)
				continue;
			if (mupnp_socket_getid(sock) == id) {
				isIDUsed = TRUE;
				break;
			}
		}
	} while (isIDUsed != FALSE);

	mupnp_log_debug_l4("Leaving...\n");

	return id;
}

#endif

/****************************************
* mupnp_socket_getavailableid
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

#define CG_NET_SOCKET_MIN_SOCKET_PORT 50000

static int mupnp_socket_getavailableport()
{
	mUpnpSocket *sock;
	int port;
	BOOL isPortUsed;

	mupnp_log_debug_l4("Entering...\n");

	port = CG_NET_SOCKET_MIN_SOCKET_PORT - 1;
	do {
		port++;
		isPortUsed = FALSE;
		for (sock = mupnp_socketlist_gets(socketList); sock != NULL; sock = mupnp_socket_next(sock)) {
			if (mupnp_socket_getport(sock) == port) {
				isPortUsed = TRUE;
				break;
			}
		}
	} while (isPortUsed != FALSE);

	mupnp_log_debug_l4("Leaving...\n");

	return port;
}

#endif

/****************************************
* mupnp_socket_*windowbuffer (ITRON)
****************************************/

#if defined(ITRON)

BOOL mupnp_socket_initwindowbuffer(mUpnpSocket *sock)
{
	mupnp_log_debug_l4("Entering...\n");

	if (sock->sendWinBuf == NULL)
		sock->sendWinBuf = (char *)malloc(sizeof(UH) * CG_NET_SOCKET_WINDOW_BUFSIZE);
	if (sock->sendWinBuf == NULL)
		sock->recvWinBuf = (char *)malloc(sizeof(UH) * CG_NET_SOCKET_WINDOW_BUFSIZE);
	
	mupnp_log_debug_l4("Leaving...\n");

	if ( ( NULL == sock->sendWinBuf ) || ( NULL == sock->sendWinBuf ) )
	{
		mupnp_log_debug_s("Memory allocation failure!\n");
		return FALSE;
	}
	else
		return TRUE;
}

BOOL mupnp_socket_freewindowbuffer(mUpnpSocket *sock)
{
	mupnp_log_debug_l4("Entering...\n");

	if (sock->sendWinBuf != NULL)
		free(sock->sendWinBuf);
	if (sock->sendWinBuf != NULL)
		free(sock->recvWinBuf);

	mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* mupnp_socket_*_callback (ITRON)
****************************************/

#if defined(ITRON)

static ER mupnp_socket_udp_callback(ID cepid, FN fncd, VP parblk)
{
	mupnp_log_debug_l4("Entering...\n");

    return E_OK;

	mupnp_log_debug_l4("Leaving...\n");
}

static ER mupnp_socket_tcp_callback(ID cepid, FN fncd, VP parblk)
{
	mupnp_log_debug_l4("Entering...\n");

    return E_OK;

	mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* mupnp_socket_getavailablelocaladdress (ITRON)
****************************************/

#if defined(ITRON)

static BOOL mupnp_socket_getavailablelocaladdress(T_IPV4EP *localAddr)
{
	ER ret;
	char *ifAddr;
	int localPort;

	mupnp_log_debug_l4("Entering...\n");

	mUpnpNetworkInterfaceList *netIfList;
	mUpnpNetworkInterface *netIf;
	int netIfCnt;
	netIfList = mupnp_net_interfacelist_new();
	netIfCnt = mupnp_net_gethostinterfaces(netIfList);
	if (netIfCnt <= 0) {
		mupnp_net_interfacelist_delete(netIfList);
		return FALSE;
	}
	netIf = mupnp_net_interfacelist_gets(netIfList);
	ifAddr = mupnp_net_interface_getaddress(netIf);
	localPort = mupnp_socket_getavailableport();
	localAddr->ipaddr = ascii_to_ipaddr(ifAddr);
	localAddr->portno = htons(localPort);
	mupnp_net_interfacelist_delete(netIfList);

	return FALSE;
}
#endif

#if defined (WIN32)
/****************************************
* mupnp_socket_getlasterror (WIN32)
****************************************/
int mupnp_socket_getlasterror()
{
	return WSAGetLastError();
};
#endif
