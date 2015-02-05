/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_response.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*
******************************************************************/

#include <mupnp/util/list.h>
#include <mupnp/http/http.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_http_response_new
****************************************/

CgHttpResponse *mupnp_http_response_new()
{
	CgHttpResponse *httpRes;

	mupnp_log_debug_l4("Entering...\n");

	httpRes = (CgHttpResponse *)malloc(sizeof(CgHttpResponse));
	
	if ( NULL != httpRes )
	{
		mupnp_http_packet_init((CgHttpPacket *)httpRes);
		httpRes->version = mupnp_string_new();
		httpRes->reasonPhrase = mupnp_string_new();
		
		mupnp_http_response_setversion(httpRes, CG_HTTP_VER11);
		mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_BAD_REQUEST);
		mupnp_http_response_setuserdata(httpRes, NULL);

		mupnp_http_response_settimeout(httpRes, CG_HTTP_CONN_TIMEOUT);
	}
	
	return httpRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_delete
****************************************/

void mupnp_http_response_delete(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_packet_clean((CgHttpPacket *)httpRes);
	mupnp_string_delete(httpRes->version);
	mupnp_string_delete(httpRes->reasonPhrase);
	free(httpRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_clear
****************************************/

void mupnp_http_response_clear(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_packet_clear((CgHttpPacket *)httpRes);
	mupnp_http_response_setversion(httpRes, NULL);
	mupnp_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_INTERNAL_SERVER_ERROR);
	mupnp_http_response_setreasonphrase(httpRes, NULL);
	mupnp_http_response_setuserdata(httpRes, NULL);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setversion
****************************************/

void mupnp_http_response_setversion(CgHttpResponse *httpRes, const char *value)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(httpRes->version, value);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getversion
****************************************/

char *mupnp_http_response_getversion(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(httpRes->version);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setstatuscode
****************************************/

void mupnp_http_response_setstatuscode(CgHttpResponse *httpRes, int value)
{
	mupnp_log_debug_l4("Entering...\n");

	httpRes->statusCode = value;
	mupnp_http_response_setreasonphrase(httpRes,
			(char*) mupnp_http_statuscode2reasonphrase(value));

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getstatuscode
****************************************/

int mupnp_http_response_getstatuscode(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	return httpRes->statusCode;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_setreasonphrase
****************************************/

void mupnp_http_response_setreasonphrase(CgHttpResponse *httpRes, const char *value)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_setvalue(httpRes->reasonPhrase, value);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_getreasonphrase
****************************************/

char *mupnp_http_response_getreasonphrase(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	return mupnp_string_getvalue(httpRes->reasonPhrase);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_read
****************************************/

BOOL mupnp_http_response_read(CgHttpResponse *httpRes, CgSocket *sock, BOOL onlyHeader)
{
	char lineBuf[CG_HTTP_READLINE_BUFSIZE];
	CgStringTokenizer *strTok;
	char *token;
	ssize_t readLen;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_response_clear(httpRes);

	readLen = mupnp_socket_readline(sock, lineBuf, sizeof(lineBuf));
	if (readLen <= 0)
		return FALSE;

	strTok = mupnp_string_tokenizer_new(lineBuf, CG_HTTP_STATUSLINE_DELIM);
	if (mupnp_string_tokenizer_hasmoretoken(strTok) == TRUE)
		mupnp_http_response_setversion(httpRes, mupnp_string_tokenizer_nexttoken(strTok));
	if (mupnp_string_tokenizer_hasmoretoken(strTok) == TRUE)
		mupnp_http_response_setstatuscode(httpRes, atoi(mupnp_string_tokenizer_nexttoken(strTok)));
	if (mupnp_string_tokenizer_hasmoretoken(strTok) == TRUE) {
		token = mupnp_string_tokenizer_nextalltoken(strTok);
		mupnp_strrtrim(token, CG_HTTP_STATUSLINE_DELIM, mupnp_strlen(CG_HTTP_STATUSLINE_DELIM));
		mupnp_http_response_setreasonphrase(httpRes, token);
	}
	mupnp_string_tokenizer_delete(strTok);

	mupnp_http_packet_read((CgHttpPacket *)httpRes, sock, onlyHeader, lineBuf, sizeof(lineBuf));
	
	mupnp_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* mupnp_http_response_copy
****************************************/

void mupnp_http_response_copy(CgHttpResponse *destHttpRes, CgHttpResponse *srcHttpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_response_setversion(destHttpRes, mupnp_http_response_getversion(srcHttpRes));
	mupnp_http_response_setstatuscode(destHttpRes, mupnp_http_response_getstatuscode(srcHttpRes));
	mupnp_http_response_setreasonphrase(destHttpRes, mupnp_http_response_getreasonphrase(srcHttpRes));

	mupnp_http_packet_copy((CgHttpPacket *)destHttpRes, (CgHttpPacket *)srcHttpRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_response_print
****************************************/

void mupnp_http_response_print(CgHttpResponse *httpRes)
{
	mupnp_log_debug_l4("Entering...\n");
	
	mupnp_log_debug_s("%s %d %s\n",
		mupnp_http_response_getversion(httpRes),
		mupnp_http_response_getstatuscode(httpRes),
		mupnp_http_response_getreasonphrase(httpRes));

	mupnp_http_packet_print((CgHttpPacket *)httpRes);
	
	mupnp_log_debug_l4("Leaving...\n");
}
