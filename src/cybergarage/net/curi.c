/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license with patent exclusion,
*       see file COPYING.
*
*	File: cdevice.c
*
*	Revision:
*
*	03/09/05
*		- first revision
*	08/16/25
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Change cg_net_uri_new() to set "/" as the default path.
*		- Changed cg_net_uri_set() to check a ":" and "@" in the password.
*	10/30/05
*		- Thanks for Smolander Visa <visa.smolander@nokia.com>
*		- Changed cg_net_uri_set(). Even a slash wasn't found in URL, 
*		  so it represents only a  part of path. Put slash infront and 
*		  append rest of the string.
*	10/31/05
*		- cg_net_uri_set: Accepts URI (as a path), which doesn't have
*		  a slash.
*	16-Jan-06 Aapo Makela
*		- Added URI escaping and unescaping functions
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clog.h>
#if defined(CG_HTTP_CURL)
#include <curl/curl.h>
#endif

/****************************************
* cg_net_uri_new
****************************************/

CgNetURI *cg_net_uri_new()
{
	CgNetURI *uri;
	
	cg_log_debug_l4("Entering...\n");

	uri = (CgNetURI *)malloc(sizeof(CgNetURI));

	if ( NULL != uri )
	{
		uri->uri = cg_string_new();
		uri->protocol = cg_string_new();
		uri->user = cg_string_new();
		uri->password = cg_string_new();
		uri->host = cg_string_new();
		uri->port = 0;
		uri->path = cg_string_new();
		uri->query = cg_string_new();
		uri->fragment = cg_string_new();
		uri->request = NULL;
		
		/**** Thanks for Theo Beisch (2005/08/25) ****/
		cg_string_setvalue(uri->path, CG_NET_URI_DEFAULT_PATH);
	}
		
	return uri;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_uri_delete
****************************************/

void cg_net_uri_delete(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_delete(uri->uri);
	cg_string_delete(uri->protocol);
	cg_string_delete(uri->user);
	cg_string_delete(uri->password);
	cg_string_delete(uri->host);
	cg_string_delete(uri->path);
	cg_string_delete(uri->query);
	cg_string_delete(uri->fragment);
	if (uri->request != NULL) cg_string_delete(uri->request);
	
	free(uri);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_uri_clear
****************************************/

void cg_net_uri_clear(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_clear(uri->uri);
	cg_string_clear(uri->protocol);
	cg_string_clear(uri->user);
	cg_string_clear(uri->password);
	cg_string_clear(uri->host);
	uri->port = 0;
	cg_string_clear(uri->path);
	cg_string_clear(uri->query);
	cg_string_clear(uri->fragment);
	if (uri->request != NULL) 
	{
		cg_string_delete(uri->request);
		uri->request = NULL;
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_net_uri_set
****************************************/

void cg_net_uri_set(CgNetURI *uri, char *value)
{
	char *protocol;
	int uriLen;
	int currIdx;
	int protoIdx;
	int atIdx;
	int colonIdx;
	int shashIdx;
	char *host;
	int eblacketIdx;
	CgString *hostStr;
	CgString *portStr;
	int hostLen;
	int sharpIdx;
	int questionIdx;
	int queryLen;
	
	cg_log_debug_l4("Entering...\n");

	uriLen = cg_strlen(value);
	cg_net_uri_clear(uri);
	cg_net_uri_seturi(uri, value);
		
	currIdx = 0;
	
	/*** Protocol ****/
	protoIdx = cg_strstr(value, CG_NET_URI_PROTOCOL_DELIM);
	if (0 < protoIdx) {
		cg_string_setnvalue(uri->protocol, value,  protoIdx);
		currIdx += protoIdx + cg_strlen(CG_NET_URI_PROTOCOL_DELIM);
	}

	/*** User (Password) ****/
	atIdx = cg_strstr(value+currIdx, CG_NET_URI_USER_DELIM);
	if (0 < atIdx) {
		colonIdx = cg_strstr(value+currIdx, CG_NET_URI_COLON_DELIM);
		/**** Thanks for Theo Beisch (2005/08/25) ****/
		if (0 < colonIdx && colonIdx<atIdx) {
			cg_string_setnvalue(uri->user, value+currIdx,  colonIdx);
			cg_string_setnvalue(uri->password, value+currIdx+colonIdx+1,  atIdx-(colonIdx+1));
		}
		else 
			cg_string_setnvalue(uri->user, value+currIdx,  atIdx - currIdx);
		currIdx += atIdx + 1;
	}

	/*** Host (Port) ****/
	shashIdx = cg_strstr(value+currIdx, CG_NET_URI_SLASH_DELIM);
	if (0 < shashIdx)
		cg_string_setnvalue(uri->host, value+currIdx,  shashIdx);
	else if (cg_net_uri_isabsolute(uri) == TRUE)
		cg_string_setnvalue(uri->host, value+currIdx, cg_strlen(value) - currIdx);
	host = cg_net_uri_gethost(uri);
	colonIdx = cg_strrchr(host, CG_NET_URI_COLON_DELIM, 1);
	eblacketIdx = cg_strrchr(host, CG_NET_URI_EBLACET_DELIM, 1);
	if (0 < colonIdx && eblacketIdx < colonIdx) {
		hostStr = cg_string_new();
		cg_string_setvalue(hostStr, host);
		hostLen = cg_string_length(hostStr);
		/**** host ****/
		cg_string_setnvalue(uri->host, cg_string_getvalue(hostStr),  colonIdx);
		host = cg_net_uri_gethost(uri);
		if (0 < hostLen) {
			if (host[0] == '[' && host[hostLen-1] == ']')
				cg_string_setnvalue(uri->host, cg_string_getvalue(hostStr)+1,  colonIdx-2);
		}
		/**** port ****/
		portStr = cg_string_new();
		cg_string_setnvalue(portStr, cg_string_getvalue(hostStr)+colonIdx+1,  hostLen- colonIdx-1);
		uri->port = atoi(cg_string_getvalue(portStr));
		cg_string_delete(portStr);
		cg_string_delete(hostStr);
	}
	else {
		uri->port = CG_NET_URI_KNKOWN_PORT;
		protocol = cg_net_uri_getprotocol(uri);
		if (cg_strcmp(protocol, CG_NET_URI_PROTOCOL_HTTP) == 0)
			uri->port = CG_NET_URI_DEFAULT_HTTP_PORT;
		if (cg_strcmp(protocol, CG_NET_URI_PROTOCOL_FTP) == 0)
			uri->port = CG_NET_URI_DEFAULT_FTP_PORT;
	}
	
	if (shashIdx > 0) currIdx += shashIdx;
	
	/*
		Handle relative URL
	*/
	if (cg_net_uri_isabsolute(uri) == FALSE)
	{
		cg_string_addvalue(uri->path, value);
		
	} else {
		/* First set path simply to the rest of URI */
		cg_string_setnvalue(uri->path, value+currIdx,  uriLen-currIdx);
	}
		
	/**** Path (Query/Fragment) ****/
	sharpIdx = cg_strstr(value+currIdx, CG_NET_URI_SHARP_DELIM);
	if (0 < sharpIdx) {
		cg_string_setnvalue(uri->path, value+currIdx,  sharpIdx);
		cg_string_setnvalue(uri->fragment, value+currIdx+sharpIdx+1,  uriLen-(currIdx+sharpIdx+1));
	}
	questionIdx = cg_strstr(value+currIdx, CG_NET_URI_QUESTION_DELIM);
	if (0 < questionIdx) {
		cg_string_setnvalue(uri->path, value+currIdx,  questionIdx);
		queryLen = uriLen-(currIdx+questionIdx+1);
		if (0 < sharpIdx)
			queryLen -= uriLen - (currIdx+sharpIdx+1);
		cg_string_setnvalue(uri->query, value+currIdx+questionIdx+1,  queryLen);
	}

	cg_log_debug_l4("Leaving...\n");
}


char *cg_net_uri_getrequest(CgNetURI *uri)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_net_uri_hasquery(uri) == FALSE)
	{
		return cg_net_uri_getpath(uri);
	}
	
	if (uri->request == NULL) uri->request = cg_string_new();

	cg_string_setvalue(uri->request, cg_net_uri_getpath(uri));
	cg_string_addvalue(uri->request, CG_NET_URI_QUESTION_DELIM);
	cg_string_addvalue(uri->request, cg_net_uri_getquery(uri));
	
	return cg_string_getvalue(uri->request);

	cg_log_debug_l4("Leaving...\n");
}


BOOL cg_net_uri_isequivalent(char *url, char *relative_url)
{
	CgNetURI *u;
	char *path;
	BOOL ret;

	u = cg_net_uri_new();
	cg_net_uri_set(u, url);

	path = cg_net_uri_getpath(u);
	ret = cg_strcmp(path, relative_url) == 0;

	cg_net_uri_delete(u);
	return ret;
}

BOOL cg_net_uri_isescapedstring(char *buf, int bufSize)
{
	int idx;

	cg_log_debug_l4("Entering...\n");

	idx = 0;
	
	if (bufSize < 1) bufSize = cg_strlen(buf) + 1;
	
	/* We assume that the URI string is escaped, if it has at least one
	   escaped character */
	
	/* First check that there is escaping character */
	idx = cg_strstr(buf, CG_NET_URI_ESCAPING_CHAR);
	if (idx < 0 || idx > bufSize - 4) return FALSE;
	
	/* Check that the next two characters are HEX */
	if (((buf[idx+1] >= '0' && buf[idx+1] <= '9') ||
	     (buf[idx+1] >= 'a' && buf[idx+1] <= 'f') ||
	     (buf[idx+1] >= 'A' && buf[idx+1] <= 'F')) &&
	    ((buf[idx+2] >= '0' && buf[idx+2] <= '9') ||
	     (buf[idx+2] >= 'a' && buf[idx+2] <= 'f') ||
	     (buf[idx+2] >= 'A' && buf[idx+2] <= 'F')))
		return TRUE;
	
	return FALSE;

	cg_log_debug_l4("Leaving...\n");
}


char *cg_net_uri_unescapestring(char *buf, int bufSize)
{
	char *tmp = NULL;
	
	cg_log_debug_l4("Entering...\n");

#if defined(CG_HTTP_CURL)
	tmp = (bufSize < 1)?curl_unescape(buf, 0):curl_unescape(buf, bufSize);
	if (tmp == NULL)
	{
		cg_log_debug_s("Memory allocation problem!\n");
		return NULL;
	}
#else
    /* #warning This code is not ready / fully implemented! */
	/* TODO: non-CURL implementation */

	/* Check if URI is already escaped */
	if (cg_net_uri_isescapedstring(buf, bufSize) == FALSE) 
		return buf;
	
	if (bufSize < 1) bufSize = cg_strlen(buf) + 1;

	tmp = cg_strdup(buf);
#endif
	
	/* Print unescaped string back to buffer */
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize,
#else
	sprintf(buf,
#endif
		"%s",
		tmp);
	
	/* Free tmp string */
	if (tmp != NULL) 
	{
#if defined(CG_HTTP_CURL)
		curl_free(tmp);
#else
		free(tmp);
#endif
	}
	
	return buf;

	cg_log_debug_l4("Leaving...\n");
}


char *cg_net_uri_escapestring(char *buf, int bufSize)
{
	char *tmp = NULL;
	int idx = 0, tmpIdx = 0;
	
	cg_log_debug_l4("Entering...\n");

	/* Check if URI is already escaped */
	if (cg_net_uri_isescapedstring(buf + idx, bufSize) == TRUE) 
		return buf;
	
	/* We can safely assume that the non-path part is already escaped */
	idx = cg_strstr(buf, CG_NET_URI_PROTOCOL_DELIM);
	if (idx > 0)
	{
		idx = idx + cg_strlen(CG_NET_URI_PROTOCOL_DELIM);
		tmpIdx = cg_strstr(buf + idx, CG_NET_URI_SLASH_DELIM);
		if (tmpIdx > 0) idx += tmpIdx + cg_strlen(CG_NET_URI_SLASH_DELIM);
	} else {
		idx = 0;
	}
		
	if (bufSize < 1) bufSize = cg_strlen(buf) + 1;	
	
#if defined(CG_HTTP_CURL)
	tmp = curl_escape(buf + idx, 0);
	if (tmp == NULL) return buf;
#else
	/* TODO: non-CURL implementation */
	tmp = cg_strdup(buf + idx);
#endif
	
cg_log_debug_s("%s ==> %s\n", buf + idx, tmp);
	
	/* Print escaped string back to buffer */
#if defined(HAVE_SNPRINTF)
	snprintf(buf + idx, bufSize - idx,
#else
	sprintf(buf + idx,
#endif
		"%s",
		tmp);
	
	/* Free tmp string */
	if (tmp != NULL) 
	{
#if defined(CG_HTTP_CURL)
		curl_free(tmp);
#else
		free(tmp);
#endif
	}
	
	return buf;

	cg_log_debug_l4("Leaving...\n");
}

char *cg_net_uri_getupnpbasepath(CgNetURI *locationURL)
{
        char *path, *c;
        int i;

        path = cg_strdup(cg_net_uri_getpath(locationURL));

	cg_log_debug_s("Mangling url string: %s\n", path);
        
	i = cg_strlen(path);

        if ( 0 >= i )
        {
                cg_log_debug("No base path, doing nothing.\n");
                return NULL;
        }

        /* Truncating out the "file name" from path */
        for ( c=( path + --i); 0<=i; c=( path + --i ))
                if ( '/' == *c )
                {
                        *( path + i + 1 ) = '\0';
			cg_log_debug_s("Truncating string from place %d\n", i);
                        break;
                }

	cg_log_debug_s("url string after mangling: %s\n", path);

        return path;
}
