/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
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
******************************************************************/

#include <cybergarage/net/curi.h>
#include <cybergarage/util/cstring.h>

/****************************************
* cg_net_uri_new
****************************************/

CgNetURI *cg_net_uri_new()
{
	CgNetURI *uri = (CgNetURI *)malloc(sizeof(CgNetURI));
	
	uri->uri = cg_string_new();
	uri->protocol = cg_string_new();
	uri->user = cg_string_new();
	uri->password = cg_string_new();
	uri->host = cg_string_new();
	uri->port = 0;
	uri->path = cg_string_new();
	uri->query = cg_string_new();
	uri->fragment = cg_string_new();

	/**** Thanks for Theo Beisch (2005/08/25) ****/
	cg_string_setvalue(uri->path, CG_NET_URI_DEFAULT_PATH);
		
	return uri;
}

/****************************************
* cg_net_uri_delete
****************************************/

void cg_net_uri_delete(CgNetURI *uri)
{
	cg_string_delete(uri->uri);
	cg_string_delete(uri->protocol);
	cg_string_delete(uri->user);
	cg_string_delete(uri->password);
	cg_string_delete(uri->host);
	cg_string_delete(uri->path);
	cg_string_delete(uri->query);
	cg_string_delete(uri->fragment);
	
	free(uri);
}

/****************************************
* cg_net_uri_clear
****************************************/

void cg_net_uri_clear(CgNetURI *uri)
{
	cg_string_clear(uri->uri);
	cg_string_clear(uri->protocol);
	cg_string_clear(uri->user);
	cg_string_clear(uri->password);
	cg_string_clear(uri->host);
	uri->port = 0;
	cg_string_clear(uri->path);
	cg_string_clear(uri->query);
	cg_string_clear(uri->fragment);
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
	
	if (shashIdx < 0)
	{
		/**** Thanks for Visa Smolander (10/30/2005) ****/
		cg_string_setvalue(uri->path, CG_NET_URI_SLASH_DELIM);
		cg_string_addvalue(uri->path, value);
 		return;
	}

	currIdx += shashIdx;
	
	/**** Path (Query/Fragment) ****/
	cg_string_setnvalue(uri->path, value+currIdx,  uriLen-currIdx);
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
			queryLen -= uriLen - (currIdx+sharpIdx);
		cg_string_setnvalue(uri->query, value+currIdx+questionIdx+1,  queryLen);
	}
}

