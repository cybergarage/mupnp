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

#include <mupnp/ssdp/ssdp_server.h>
#include <mupnp/control/control.h>
#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_ssdp_packet_new
****************************************/

mUpnpSSDPPacket* mupnp_ssdp_packet_new()
{
  mUpnpSSDPPacket* ssdpPkt;

  mupnp_log_debug_l4("Entering...\n");

  ssdpPkt = (mUpnpSSDPPacket*)malloc(sizeof(mUpnpSSDPPacket));

  if (NULL != ssdpPkt) {
    ssdpPkt->dgmPkt = mupnp_socket_datagram_packet_new();
    ssdpPkt->headerList = mupnp_http_headerlist_new();
    ssdpPkt->initialized = 0;

    mupnp_ssdp_packet_setuserdata(ssdpPkt, NULL);
    mupnp_ssdp_packet_settimestamp(ssdpPkt, mupnp_getcurrentsystemtime());

    ssdpPkt->timestamps = (mUpnpTime*)malloc(MUPNP_SSDP_FILTER_TABLE_SIZE * sizeof(mUpnpTime));
  }

  return ssdpPkt;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_delete
****************************************/

void mupnp_ssdp_packet_delete(mUpnpSSDPPacket* ssdpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_ssdp_packet_clear(ssdpPkt);

  mupnp_socket_datagram_packet_delete(ssdpPkt->dgmPkt);
  mupnp_http_headerlist_delete(ssdpPkt->headerList);

  free(ssdpPkt->timestamps);

  free(ssdpPkt);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_clear
****************************************/

void mupnp_ssdp_packet_clear(mUpnpSSDPPacket* ssdpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_socket_datagram_packet_setdata(ssdpPkt->dgmPkt, NULL);
  mupnp_http_headerlist_clear(ssdpPkt->headerList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_isrootdevice
****************************************/

bool mupnp_ssdp_packet_isrootdevice(mUpnpSSDPPacket* ssdpPkt)
{
  mupnp_log_debug_l4("Entering...\n");

  if (mupnp_nt_isrootdevice(mupnp_ssdp_packet_getnt(ssdpPkt)) == true)
    return true;
  if (mupnp_st_isrootdevice(mupnp_ssdp_packet_getst(ssdpPkt)) == true)
    return true;
  return mupnp_usn_isrootdevice(mupnp_ssdp_packet_getusn(ssdpPkt));

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_setheader
****************************************/

void mupnp_ssdp_packet_setheader(mUpnpSSDPPacket* ssdpPkt, char* ssdpMsg)
{
  mUpnpStringTokenizer* ssdpTok;
  mUpnpStringTokenizer* ssdpLineTok;
  mUpnpHttpHeader* header;
  char* lineMsg;
  char* name;
  char* value;

  mupnp_log_debug_l4("Entering...\n");

  ssdpTok = mupnp_string_tokenizer_new(ssdpMsg, MUPNP_HTTP_CRLF);

  /**** skip the first line ****/
  if (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == false)
    return;
  lineMsg = mupnp_string_tokenizer_nexttoken(ssdpTok);

  while (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == true) {
    lineMsg = mupnp_string_tokenizer_nexttoken(ssdpTok);
    name = NULL;
    value = NULL;
    ssdpLineTok = mupnp_string_tokenizer_new(lineMsg, MUPNP_HTTP_HEADERLINE_DELIM);
    if (mupnp_string_tokenizer_hasmoretoken(ssdpLineTok) == true)
      name = mupnp_string_tokenizer_nexttoken(ssdpLineTok);
    if (mupnp_string_tokenizer_hasmoretoken(ssdpLineTok) == true) {
      value = mupnp_string_tokenizer_nextalltoken(ssdpLineTok);
      mupnp_strrtrim(value, MUPNP_HTTP_HEADERLINE_DELIM, mupnp_strlen(MUPNP_HTTP_HEADERLINE_DELIM));
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
 * mupnp_ssdp_packet_getmaxage
 ****************************************/

long mupnp_ssdp_packet_getmaxage(mUpnpSSDPPacket* ssdpPkt)
{
  const char* cachecontrol = NULL;
  ssize_t maxageIdx = 0;

  mupnp_log_debug_l4("Entering...\n");

  cachecontrol = mupnp_ssdp_packet_getcachecontrol(ssdpPkt);
  if (cachecontrol == NULL)
    return 0;

  maxageIdx = mupnp_strstr(cachecontrol, MUPNP_HTTP_MAX_AGE);
  if (maxageIdx < 0)
    return 0;

  maxageIdx = mupnp_strstr(cachecontrol + maxageIdx, "=");
  if (maxageIdx <= 0)
    return 0;

  maxageIdx++;

  return mupnp_str2long(cachecontrol + maxageIdx);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_copy
****************************************/

void mupnp_ssdp_packet_copy(mUpnpSSDPPacket* dstSsdpPkt, mUpnpSSDPPacket* srcSsdpPkt)
{
  mUpnpHttpHeader* srcHeader;
  mUpnpHttpHeader* destHeader;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_socket_datagram_packet_copy(dstSsdpPkt->dgmPkt, srcSsdpPkt->dgmPkt);

  /**** copy headers ****/
  mupnp_ssdp_packet_clear(dstSsdpPkt);
  for (srcHeader = mupnp_ssdp_packet_getheaders(srcSsdpPkt); srcHeader != NULL; srcHeader = mupnp_http_header_next(srcHeader)) {
    destHeader = mupnp_http_header_new();
    mupnp_http_header_setname(destHeader, mupnp_http_header_getname(srcHeader));
    mupnp_http_header_setvalue(destHeader, mupnp_http_header_getvalue(srcHeader));
    mupnp_ssdp_packet_addheader(dstSsdpPkt, destHeader);
  }

  /* Set timestamp */
  mupnp_ssdp_packet_settimestamp(dstSsdpPkt, mupnp_getcurrentsystemtime());

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_ssdp_packet_print
****************************************/

void mupnp_ssdp_packet_print(mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpHttpHeader* header;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_s("ssdp from %s %d\n",
      mupnp_ssdp_packet_getremoteaddress(ssdpPkt),
      mupnp_ssdp_packet_getremoteport(ssdpPkt));

  /**** print headers ****/
  for (header = mupnp_http_headerlist_gets(ssdpPkt->headerList); header != NULL; header = mupnp_http_header_next(header)) {
    mupnp_log_debug_s("%s: %s\n",
        mupnp_http_header_getname(header),
        mupnp_http_header_getvalue(header));
  }

  mupnp_log_debug_l4("Leaving...\n");
}
