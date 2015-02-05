/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cssdp_request.c
*
*	Revision:
*
*	03/05/05
*		- first revision
*
******************************************************************/

#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

#define CG_UPNP_SSDP_MAXAGE_LEN 7

/****************************************
* mupnp_upnp_ssdpresponse_new
****************************************/

mUpnpUpnpSSDPResponse *mupnp_upnp_ssdpresponse_new()
{
	mUpnpUpnpSSDPResponse*ssdpRes;

	mupnp_log_debug_l4("Entering...\n");

	ssdpRes = mupnp_http_response_new();
	
	mupnp_http_response_setversion(ssdpRes, CG_HTTP_VER11);
	mupnp_http_response_setstatuscode(ssdpRes, CG_HTTP_STATUS_OK);

	return ssdpRes;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdpresponse_delete
****************************************/

void mupnp_upnp_ssdpresponse_delete(mUpnpUpnpSSDPResponse *ssdpRes)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_response_delete(ssdpRes);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdpresponse_setleasetime
****************************************/

void mupnp_upnp_ssdpresponse_setleasetime(mUpnpUpnpSSDPResponse *ssdpRes, mUpnpTime value)
{
	char buf[CG_UPNP_SSDP_MAXAGE_LEN + 1 + CG_STRING_INTEGER_BUFLEN];

	mupnp_log_debug_l4("Entering...\n");

	sprintf(buf, "%s=%d", CG_HTTP_MAX_AGE, (int)value);
	mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpRes, CG_HTTP_CACHE_CONTROL, buf);

	mupnp_log_debug_l4("Leaving...\n");
}
		
/****************************************
* mupnp_upnp_ssdpresponse_getleasetime
****************************************/

int mupnp_upnp_ssdpresponse_getleasetime(mUpnpUpnpSSDPResponse *ssdpRes)
{
	const char *cacheCtrl;

	mupnp_log_debug_l4("Entering...\n");

	cacheCtrl = mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpRes, CG_HTTP_CACHE_CONTROL);

	mupnp_log_debug_l4("Leaving...\n");
  
	return mupnp_upnp_ssdp_getleasetime(cacheCtrl);
}

/****************************************
* mupnp_upnp_ssdpresponse_tostring
****************************************/

char *mupnp_upnp_ssdpresponse_tostring(mUpnpUpnpSSDPResponse *ssdpRes, mUpnpString *ssdpMsg)
{
	mUpnpHttpHeader *header;
	char statusCodeBuf[CG_STRING_INTEGER_BUFLEN];
	const char *name;
	const char *value;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_addvalue(ssdpMsg, mupnp_http_response_getversion(ssdpRes));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_SP);
	mupnp_string_addvalue(ssdpMsg, mupnp_int2str(mupnp_http_response_getstatuscode(ssdpRes), statusCodeBuf, sizeof(statusCodeBuf)));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_SP);
	mupnp_string_addvalue(ssdpMsg, mupnp_http_response_getreasonphrase(ssdpRes));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	for (header = mupnp_http_packet_getheaders((mUpnpHttpPacket *)ssdpRes); header != NULL; header = mupnp_http_header_next(header)) {
		name = mupnp_http_header_getname(header);
		value = mupnp_http_header_getvalue(header);
		mupnp_string_addvalue(ssdpMsg, name);
		mupnp_string_addvalue(ssdpMsg, CG_HTTP_COLON);
		mupnp_string_addvalue(ssdpMsg, CG_HTTP_SP);
		mupnp_string_addvalue(ssdpMsg, value);
		mupnp_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	}
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	return mupnp_string_getvalue(ssdpMsg);

	mupnp_log_debug_l4("Leaving...\n");
}

