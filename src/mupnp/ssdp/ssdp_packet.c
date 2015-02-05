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
*	File: cssdp_packet.c
*
*	Revision:
*
*	06/01/05
*		- first revision
*
*	10/31/05
*		- Delete also ssdpTok in mupnp_upnp_ssdp_packet_set_header()

******************************************************************/

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/control/control.h>
#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_upnp_ssdp_packet_new
****************************************/

mUpnpUpnpSSDPPacket *mupnp_upnp_ssdp_packet_new()
{
	mUpnpUpnpSSDPPacket *ssdpPkt;

	mupnp_log_debug_l4("Entering...\n");

	ssdpPkt = (mUpnpUpnpSSDPPacket *)malloc(sizeof(mUpnpUpnpSSDPPacket));
	
	if ( NULL != ssdpPkt )
	{
		ssdpPkt->dgmPkt = mupnp_socket_datagram_packet_new();
		ssdpPkt->headerList = mupnp_http_headerlist_new();
		ssdpPkt->initialized = 0;

		mupnp_upnp_ssdp_packet_setuserdata(ssdpPkt, NULL);
		mupnp_upnp_ssdp_packet_settimestamp(ssdpPkt, mupnp_getcurrentsystemtime());

		ssdpPkt->timestamps = (mUpnpTime *)malloc(CG_UPNP_SSDP_FILTER_TABLE_SIZE * sizeof(mUpnpTime));
	}
	
	return ssdpPkt;

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_delete
****************************************/

void mupnp_upnp_ssdp_packet_delete(mUpnpUpnpSSDPPacket *ssdpPkt)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_upnp_ssdp_packet_clear(ssdpPkt);
	
	mupnp_socket_datagram_packet_delete(ssdpPkt->dgmPkt);
	mupnp_http_headerlist_delete(ssdpPkt->headerList);

	free(ssdpPkt->timestamps);
	
	free(ssdpPkt);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_clear
****************************************/

void mupnp_upnp_ssdp_packet_clear(mUpnpUpnpSSDPPacket *ssdpPkt)
{
	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_datagram_packet_setdata(ssdpPkt->dgmPkt, NULL);
	mupnp_http_headerlist_clear(ssdpPkt->headerList);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_isrootdevice
****************************************/

BOOL mupnp_upnp_ssdp_packet_isrootdevice(mUpnpUpnpSSDPPacket *ssdpPkt)
{
	mupnp_log_debug_l4("Entering...\n");

	if (mupnp_upnp_nt_isrootdevice(mupnp_upnp_ssdp_packet_getnt(ssdpPkt)) == TRUE)
		return TRUE;
	if (mupnp_upnp_st_isrootdevice(mupnp_upnp_ssdp_packet_getst(ssdpPkt)) == TRUE)
		return TRUE;
	return mupnp_upnp_usn_isrootdevice(mupnp_upnp_ssdp_packet_getusn(ssdpPkt));

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_setheader
****************************************/

void mupnp_upnp_ssdp_packet_setheader(mUpnpUpnpSSDPPacket *ssdpPkt, char *ssdpMsg)
{
	mUpnpStringTokenizer *ssdpTok;
	mUpnpStringTokenizer *ssdpLineTok;
	mUpnpHttpHeader *header;
	char *lineMsg;
	char *name;
	char *value;
		
	mupnp_log_debug_l4("Entering...\n");

	ssdpTok = mupnp_string_tokenizer_new(ssdpMsg, CG_HTTP_CRLF);

	/**** skip the first line ****/	
	if (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == FALSE)
		return;
	lineMsg = mupnp_string_tokenizer_nexttoken(ssdpTok);
	
	while (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == TRUE) {
		lineMsg = mupnp_string_tokenizer_nexttoken(ssdpTok);
		name = NULL;
		value = NULL;
		ssdpLineTok = mupnp_string_tokenizer_new(lineMsg, CG_HTTP_HEADERLINE_DELIM);
		if (mupnp_string_tokenizer_hasmoretoken(ssdpLineTok) == TRUE)
			name = mupnp_string_tokenizer_nexttoken(ssdpLineTok);
		if (mupnp_string_tokenizer_hasmoretoken(ssdpLineTok) == TRUE) {
			value = mupnp_string_tokenizer_nextalltoken(ssdpLineTok);
			mupnp_strrtrim(value, CG_HTTP_HEADERLINE_DELIM, mupnp_strlen(CG_HTTP_HEADERLINE_DELIM));
		}
		if (name != NULL) {
			if (value == NULL)
				value = "";
			header = mupnp_http_header_new();
			mupnp_http_header_setname(header, name);
			mupnp_http_header_setvalue(header, value);
			mupnp_http_headerlist_add(ssdpPkt->headerList, header);
		}
		mupnp_string_tokenizer_delete(ssdpLineTok);
	}

	mupnp_string_tokenizer_delete(ssdpTok);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_upnp_ssdp_packet_getmaxage
 ****************************************/

long mupnp_upnp_ssdp_packet_getmaxage(mUpnpUpnpSSDPPacket *ssdpPkt)
{
	const char *cachecontrol = NULL;
	ssize_t maxageIdx = 0;
	
	mupnp_log_debug_l4("Entering...\n");

	cachecontrol = mupnp_upnp_ssdp_packet_getcachecontrol(ssdpPkt);
	if (cachecontrol == NULL) return 0;
	
	maxageIdx = mupnp_strstr(cachecontrol, CG_HTTP_MAX_AGE);
	if (maxageIdx < 0) return 0;
		
	maxageIdx = mupnp_strstr(cachecontrol+maxageIdx, "=");
	if (maxageIdx <= 0) return 0;
		
	maxageIdx++;
	
	return mupnp_str2long(cachecontrol+maxageIdx);

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_copy
****************************************/

void mupnp_upnp_ssdp_packet_copy(mUpnpUpnpSSDPPacket *dstSsdpPkt, mUpnpUpnpSSDPPacket *srcSsdpPkt)
{
	mUpnpHttpHeader *srcHeader;
	mUpnpHttpHeader *destHeader;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_socket_datagram_packet_copy(dstSsdpPkt->dgmPkt, srcSsdpPkt->dgmPkt);

	/**** copy headers ****/
	mupnp_upnp_ssdp_packet_clear(dstSsdpPkt);
	for (srcHeader = mupnp_upnp_ssdp_packet_getheaders(srcSsdpPkt); srcHeader != NULL; srcHeader = mupnp_http_header_next(srcHeader)) {
		destHeader = mupnp_http_header_new();
		mupnp_http_header_setname(destHeader, mupnp_http_header_getname(srcHeader));
		mupnp_http_header_setvalue(destHeader, mupnp_http_header_getvalue(srcHeader));
		mupnp_upnp_ssdp_packet_addheader(dstSsdpPkt, destHeader);
	}
	
	/* Set timestamp */
	mupnp_upnp_ssdp_packet_settimestamp(dstSsdpPkt, mupnp_getcurrentsystemtime());

	mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_upnp_ssdp_packet_print
****************************************/

void mupnp_upnp_ssdp_packet_print(mUpnpUpnpSSDPPacket *ssdpPkt)
{
	mUpnpHttpHeader *header;
	
	mupnp_log_debug_l4("Entering...\n");

	mupnp_log_debug_s("ssdp from %s %d\n",
		mupnp_upnp_ssdp_packet_getremoteaddress(ssdpPkt),
		mupnp_upnp_ssdp_packet_getremoteport(ssdpPkt));
		
	/**** print headers ****/
	for (header = mupnp_http_headerlist_gets(ssdpPkt->headerList); header != NULL; header = mupnp_http_header_next(header)) {
	mupnp_log_debug_s("%s: %s\n",
			mupnp_http_header_getname(header),
			mupnp_http_header_getvalue(header));
	}


	mupnp_log_debug_l4("Leaving...\n");
}
