/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: chttp_packet.cpp
*
*	Revision:
*
*	01/25/05
*		- first revision
*	08/21/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_http_packet_read() to store length in HTTPPacket.
*	10/25/05
*		- The following functions move to define macros.
*		  cg_http_packet_setcontent()
*		  cg_http_packet_setcontentpointer()
*		  cg_http_packet_getcontent()
*		- cg_http_packet_setncontent() is added.
*	10/31/05
*		- cg_http_packet_sethost:
*		  port was an excess parameter for s(n)printf when port <= 0
*	11/11/05
*		- Added cg_http_packet_setheaderlonglong() and cg_http_packet_getheaderlonglong().
*		- Extended cg_http_packet_setcontentlength() and cg_http_packet_getcontentlength() to 64bit
*		  when the compiler is supported C99 or the platform is WIN32.
*
******************************************************************/

#include <cybergarage/http/chttp.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>

#include <limits.h>

/****************************************
* cg_http_packet_new
****************************************/

CgHttpPacket *cg_http_packet_new()
{
	CgHttpPacket *httpPkt= (CgHttpPacket *)malloc(sizeof(CgHttpPacket));
	cg_http_packet_init(httpPkt);
	return httpPkt;
}

/****************************************
* cg_http_packet_delete
****************************************/

void cg_http_packet_delete(CgHttpPacket *httpPkt)
{
	cg_http_packet_clean(httpPkt);
	free(httpPkt);
}

/****************************************
* cg_http_packet_init
****************************************/

void cg_http_packet_init(CgHttpPacket *httpPkt)
{
	httpPkt->headerList = cg_http_headerlist_new();
	httpPkt->content = cg_string_new();
}

/****************************************
* cg_http_packet_clean
****************************************/

void cg_http_packet_clean(CgHttpPacket *httpPkt)
{
	cg_http_headerlist_delete(httpPkt->headerList);
	cg_string_delete(httpPkt->content);
}

/****************************************
* cg_http_packet_clear
****************************************/

void cg_http_packet_clear(CgHttpPacket *httpPkt)
{
	cg_http_headerlist_clear(httpPkt->headerList);
	cg_string_setvalue(httpPkt->content, NULL);
}

/****************************************
* cg_http_packet_setheader
****************************************/

void cg_http_packet_setheadervalue(CgHttpPacket *httpPkt, char* name, char *value)
{
	cg_http_headerlist_set(httpPkt->headerList, name, value);
}

/****************************************
* cg_http_packet_setheaderinteger
****************************************/

void cg_http_packet_setheaderinteger(CgHttpPacket *httpPkt, char* name, int value)
{
	char svalue[CG_STRING_INTEGER_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_int2str(value, svalue, sizeof(svalue)));
}

/****************************************
* cg_http_packet_setheaderlong
****************************************/

void cg_http_packet_setheaderlong(CgHttpPacket *httpPkt, char* name, long value)
{
	char svalue[CG_STRING_LONG_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_long2str(value, svalue, sizeof(svalue)));
}

/****************************************
* cg_http_packet_setheaderlonglong
****************************************/

#if defined(__USE_ISOC99)
void cg_http_packet_setheaderlonglong(CgHttpPacket *httpPkt, char* name, long long value)
{
	char svalue[CG_STRING_LONGLONG_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_longlong2str(value, svalue, sizeof(svalue)));
}
#elif defined(WIN32)
void cg_http_packet_setheaderlonglong(CgHttpPacket *httpPkt, char* name, __int64 value)
{
	char svalue[CG_STRING_LONGLONG_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_longlong2str(value, svalue, sizeof(svalue)));
}
#endif

/****************************************
* cg_http_packet_getheadervalue
****************************************/

char *cg_http_packet_getheadervalue(CgHttpPacket *httpPkt, char* name)
{
	return cg_http_headerlist_getvalue(httpPkt->headerList, name);
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

int cg_http_packet_getheaderinteger(CgHttpPacket *httpPkt, char* name)
{
	char *value = cg_http_packet_getheadervalue(httpPkt, name); 
	return (value != NULL) ? atoi(value) : 0;
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

long cg_http_packet_getheaderlong(CgHttpPacket *httpPkt, char* name)
{
	char *value = cg_http_packet_getheadervalue(httpPkt, name); 
	return (value != NULL) ? atol(value) : 0;
}

/****************************************
* cg_http_packet_getheaderlonglong
****************************************/

#if defined(__USE_ISOC99)
long long cg_http_packet_getheaderlonglong(CgHttpPacket *httpPkt, char* name)
#elif defined(WIN32)
__int64 cg_http_packet_getheaderlonglong(CgHttpPacket *httpPkt, char* name)
#endif
{
	char *value = cg_http_packet_getheadervalue(httpPkt, name); 
	return (value != NULL) ? cg_str2longlong(value) : 0;
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

void cg_http_packet_sethost(CgHttpPacket *httpPkt, char *addr, int port)
{
	char host[CG_NET_IPV4_ADDRSTRING_MAXSIZE + CG_STRING_INTEGER_BUFLEN];

#if defined(HAVE_SNPRINTF)
	if (0 < port) {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, sizeof(host), "[%s]:%d", addr, port);
		else
			snprintf(host, sizeof(host), "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, sizeof(host), "[%s]", addr);
		else
			snprintf(host, sizeof(host), "%s", addr);
	}
#else	
	if (0 < port) {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]:%d", addr, port);
		else
			sprintf(host, "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]", addr);
		else
			sprintf(host, "%s", addr);
	}
#endif

	cg_http_packet_setheadervalue(httpPkt, CG_HTTP_HOST, host);
}

/****************************************
* cg_http_packet_post
****************************************/

void cg_http_packet_post(CgHttpPacket *httpPkt, CgSocket *sock)
{
	CgHttpHeader *header;
	char *name, *value;
	char *content;
	long contentLen;
	
	/**** send headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		if (name == NULL)
			continue;
		cg_socket_write(sock, name, cg_strlen(name));
		cg_socket_write(sock, CG_HTTP_COLON, sizeof(CG_HTTP_COLON)-1);
		cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		value = cg_http_header_getvalue(header);
		if (value != NULL)
			cg_socket_write(sock, value, cg_strlen(value));
		cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1
		);
	}
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	/**** send content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	if (content != NULL && 0 < contentLen)
		cg_socket_write(sock, content, contentLen);
}

/****************************************
* cg_http_packet_read_headers
****************************************/

void cg_http_packet_read_headers(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	CgStringTokenizer *strTok;
	CgHttpHeader *header;
	int readLen;
	char *name, *value;
	
	cg_http_packet_clear(httpPkt);

	while (1) {
		readLen = cg_socket_readline(sock, lineBuf, lineBufSize);
		if (readLen <= 2)
			break;
		name = NULL;
		value = NULL;
		strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_HEADERLINE_DELIM);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
			name = cg_string_tokenizer_nexttoken(strTok);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE) {
			value = cg_string_tokenizer_nextalltoken(strTok);
			cg_strrtrim(value, CG_HTTP_HEADERLINE_DELIM, cg_strlen(CG_HTTP_HEADERLINE_DELIM));
		}
		if (name != NULL) {
			if (value == NULL)
				value = "";
			header = cg_http_header_new();
			cg_http_header_setname(header, name);
			cg_http_header_setvalue(header, value);
			cg_http_packet_addheader(httpPkt, header);
		}
		cg_string_tokenizer_delete(strTok);
	}
}

/****************************************
* cg_http_packet_read_body
****************************************/

BOOL cg_http_packet_read_body(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	int readLen;
	long conLen;
	char *content;
	CgString *contentStr;
	char readChar[2];
	int tries = 0;

	conLen = cg_http_packet_getcontentlength(httpPkt);
	content = NULL;
	if (0 < conLen) {
		content = (char *)malloc(conLen+1);
		readLen = 0;
		
		/* Read content until conLen is reached, or tired of trying */
		while (readLen < conLen && tries < 20)
		{
			readLen += cg_socket_read(sock, (content+readLen), (conLen-readLen));
			tries++;
		}
		
		if (readLen <= 0)
			return TRUE;
		content[readLen] = '\0';
	}
	else if (cg_http_packet_getheadervalue(httpPkt, 
					CG_HTTP_CONTENT_LENGTH) == NULL)
	{
		/* header existance must be checked! otherwise packets which
		   rightly report 0 as content length, will jam the http */
#if defined(CG_HTTP_SUPPORT_NO_CONTENTLENGTH_REQUEST)
		contentStr = cg_string_new();
		cg_string_setfreeflag(contentStr, FALSE);
		while (0 < cg_socket_read(sock, readChar, 1))
			cg_string_naddvalue(contentStr, readChar, 1);
		content = cg_string_getvalue(contentStr);
		/**** Thanks for Theo Beisch (08/21/05) ****/
		conLen = cg_string_length(contentStr);
		cg_http_packet_setcontentlength(httpPkt,conLen);
		cg_string_delete(contentStr);
#endif
	}

	cg_http_packet_setcontentpointer(httpPkt, content);
	return TRUE;
}

/****************************************
* cg_http_packet_read
****************************************/

BOOL cg_http_packet_read(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	cg_http_packet_read_headers(httpPkt, sock, lineBuf, lineBufSize);
	
	return cg_http_packet_read_body(httpPkt, sock, lineBuf, lineBufSize);
}

/****************************************
* cg_http_packet_getheadersize
****************************************/

int cg_http_packet_getheadersize(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	int headerSize;
	char *name;
	char *value;
	
	headerSize = 0;
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		value = cg_http_header_getvalue(header);
		headerSize += cg_strlen(name); 
		headerSize += sizeof(CG_HTTP_COLON)-1; 
		headerSize += sizeof(CG_HTTP_SP)-1; 
		headerSize += cg_strlen(value); 
		headerSize += sizeof(CG_HTTP_CRLF)-1; 
	}
	headerSize += sizeof(CG_HTTP_CRLF)-1; 

	return headerSize;
}

/****************************************
* cg_http_packet_copy
****************************************/

void cg_http_packet_copy(CgHttpPacket *destHttpPkt, CgHttpPacket *srcHttpPkt)
{
	CgHttpHeader *srcHeader;
	CgHttpHeader *destHeader;
	
	cg_http_packet_clear(destHttpPkt);
	
	/**** copy headers ****/
	for (srcHeader = cg_http_packet_getheaders(srcHttpPkt); srcHeader != NULL; srcHeader = cg_http_header_next(srcHeader)) {
		destHeader = cg_http_header_new();
		cg_http_header_setname(destHeader, cg_http_header_getname(srcHeader));
		cg_http_header_setvalue(destHeader, cg_http_header_getvalue(srcHeader));
		cg_http_packet_addheader(destHttpPkt, destHeader);
	}
	
	/**** copy content ****/
	cg_http_packet_setcontent(destHttpPkt, cg_http_packet_getcontent(srcHttpPkt));
}

/****************************************
* cg_http_packet_print
****************************************/

void cg_http_packet_print(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	char *content;
	long contentLen;
	
	/**** print headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		printf("%s: %s\n",
			cg_http_header_getname(header),
			cg_http_header_getvalue(header));
	}
	printf("\n");
	
	/**** print content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	if (content != NULL && 0 < contentLen)
		printf("%s\n", content);
}
