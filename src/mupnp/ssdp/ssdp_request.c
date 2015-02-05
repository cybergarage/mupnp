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

#include <mupnp/ssdp/ssdp.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_ssdprequest_new
****************************************/

mUpnpUpnpSSDPRequest *mupnp_upnp_ssdprequest_new()
{
	mUpnpUpnpSSDPRequest *ssdpReq;

	mupnp_log_debug_l4("Entering...\n");

	ssdpReq = mupnp_http_request_new();
	
	mupnp_http_request_seturi(ssdpReq, "*");
	mupnp_http_request_setversion(ssdpReq, CG_HTTP_VER11);
	mupnp_http_request_setcontentlength(ssdpReq, 0);

	mupnp_log_debug_l4("Leaving...\n");
	
	return ssdpReq;
}

/****************************************
* mupnp_upnp_ssdprequest_delete
****************************************/

void mupnp_upnp_ssdprequest_delete(mUpnpUpnpSSDPRequest *ssdpReq)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_http_request_delete(ssdpReq);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdprequest_setleasetime
****************************************/

void mupnp_upnp_ssdprequest_setleasetime(mUpnpUpnpSSDPRequest *ssdpReq, mUpnpTime value)
{
	char buf[sizeof(CG_HTTP_MAX_AGE) + 1 + CG_STRING_INTEGER_BUFLEN];
	
	mupnp_log_debug_l4("Entering...\n");

	sprintf(buf, "%s=%ld", CG_HTTP_MAX_AGE, (long)value);
	mupnp_http_packet_setheadervalue((mUpnpHttpPacket*)ssdpReq, CG_HTTP_CACHE_CONTROL, buf);

	mupnp_log_debug_l4("Leaving...\n");
}
		
/****************************************
* mupnp_upnp_ssdprequest_getleasetime
****************************************/

mUpnpTime mupnp_upnp_ssdprequest_getleasetime(mUpnpUpnpSSDPRequest *ssdpReq)
{
	const char *cacheCtrl;

	mupnp_log_debug_l4("Entering...\n");

	cacheCtrl = mupnp_http_packet_getheadervalue((mUpnpHttpPacket*)ssdpReq, CG_HTTP_CACHE_CONTROL);
	
	return mupnp_upnp_ssdp_getleasetime(cacheCtrl);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdprequest_tostring
****************************************/

char *mupnp_upnp_ssdprequest_tostring(mUpnpUpnpSSDPRequest *ssdpReq, mUpnpString *ssdpMsg)
{
	mUpnpHttpHeader *header;
	const char *name;
	const char *value;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_string_addvalue(ssdpMsg, mupnp_http_request_getmethod(ssdpReq));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_SP);
	mupnp_string_addvalue(ssdpMsg, mupnp_http_request_geturi(ssdpReq));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_SP);
	mupnp_string_addvalue(ssdpMsg, mupnp_http_request_getversion(ssdpReq));
	mupnp_string_addvalue(ssdpMsg, CG_HTTP_CRLF);
	
	for (header = mupnp_http_packet_getheaders((mUpnpHttpPacket *)ssdpReq); header != NULL; header = mupnp_http_header_next(header)) {
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
