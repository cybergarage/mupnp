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

#include <mupnp/device.h>
#include <mupnp/control/control.h>
#include <mupnp/util/log.h>
#include <mupnp/util/string.h>

#include <ctype.h>
#include <string.h>

static int filter_duplicate_m_search(mUpnpSSDPPacket* ssdpPkt);
static int simple_string_hash(char* str, int table_size);

/****************************************
* mupnp_device_ssdpmessagereceived
****************************************/

void mupnp_device_ssdpmessagereceived(mUpnpDevice* dev, mUpnpSSDPPacket* ssdpPkt, int filter)
{
  bool isRootDev;
  const char* ssdpST;
  const char *devUDN, *devType;
  char ssdpMsg[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
  char deviceUSN[MUPNP_SSDP_HEADER_LINE_MAXSIZE];
#if defined WINCE
  size_t n;
#else
  int n;
#endif
  mUpnpService* service;
  mUpnpDevice* childDev;
  const char* ssdpMXString;
  int ssdpMX;
  const char* ssdpTargetAddr;

  mupnp_log_debug_l4("Entering...\n");

  ssdpMXString = mupnp_http_headerlist_getvalue(ssdpPkt->headerList, MUPNP_HTTP_MX);
  ssdpST = mupnp_ssdp_packet_getst(ssdpPkt);

  /* Check if this ssdp packet has already been checked + filtered */
  if (filter) {

    /****************************************
		 * Request line
		 * Check the request line for errors, this is not ideal as it currently only
		 * checks for the presence of the strings and not the order.
		 ***************************************/
    /**** check for M-SEARCH and return if not found ****/
    if (mupnp_strstr(mupnp_string_getvalue(ssdpPkt->dgmPkt->data), MUPNP_HTTP_MSEARCH) < 0)
      return;
    /**** check for * and return if not found ****/
    if (mupnp_strstr(mupnp_string_getvalue(ssdpPkt->dgmPkt->data), "*") < 0)
      return;
    /**** check HTTP version and return if not found ****/
    if (mupnp_strstr(mupnp_string_getvalue(ssdpPkt->dgmPkt->data), MUPNP_HTTP_VER11) < 0)
      return;

    /****************************************
		 * check HOST header, should always be 239.255.255.250:1900, return if incorrect
		 ***************************************/
    ssdpTargetAddr = mupnp_ssdp_packet_gethost(ssdpPkt);
    if (mupnp_strcmp(ssdpTargetAddr, MUPNP_SSDP_MULTICAST_ADDRESS) != 0 && !mupnp_net_isipv6address(ssdpTargetAddr))
      return;

    /****************************************
		 * check MAN header, return if incorrect
		 ***************************************/
    if (mupnp_ssdp_packet_isdiscover(ssdpPkt) == false)
      return;

    /****************************************
		 * check MX header, return if incorrect
		 ***************************************/
    if (ssdpMXString == NULL || mupnp_strlen(ssdpMXString) == 0)
      /* return if the MX value does not exist or is empty */
      return;
    /* check if MX value is not an integer */
    for (n = 0; n < strlen(ssdpMXString); n++) {
      if (isdigit(ssdpMXString[n]) == 0)
        /* MX value contains a non-digit so is invalid */
        return;
    }

    /****************************************
		 * check ST header and if empty return
		 ***************************************/
    if (mupnp_strlen(ssdpST) <= 0)
      return;

    /* Check if we have received this search recently
		 * and ignore duplicates. */
    if (filter_duplicate_m_search(ssdpPkt))
      return;

    ssdpMX = mupnp_ssdp_packet_getmx(ssdpPkt);
    mupnp_log_debug("Sleeping for a while... (MX:%d)\n", ssdpMX);
    mupnp_waitrandom((ssdpMX * 1000) / 4);
  }

  isRootDev = mupnp_device_isrootdevice(dev);

  if (mupnp_st_isalldevice(ssdpST) == true) {
    /* for root device only */
    if (isRootDev == true) {
      mupnp_device_getnotifydevicent(dev, ssdpMsg, sizeof(ssdpMsg));
      mupnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
      mupnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
    }
    /* for all devices send */
    /* device type : device version */
    mupnp_device_getnotifydevicetypent(dev, ssdpMsg, sizeof(ssdpMsg));
    mupnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
    mupnp_device_postsearchresponse(dev, ssdpPkt, ssdpMsg, deviceUSN);
    /* device UUID */
    mupnp_device_postsearchresponse(dev, ssdpPkt, mupnp_device_getudn(dev), mupnp_device_getudn(dev));
  }
  else if (mupnp_st_isrootdevice(ssdpST) == true) {
    if (isRootDev == true) {
      mupnp_device_getnotifydeviceusn(dev, deviceUSN, sizeof(deviceUSN));
      mupnp_device_postsearchresponse(dev, ssdpPkt, MUPNP_ST_ROOT_DEVICE, deviceUSN);
    }
  }
  else if (mupnp_st_isuuiddevice(ssdpST) == true) {
    devUDN = mupnp_device_getudn(dev);
    if (mupnp_streq(ssdpST, devUDN) == true)
      mupnp_device_postsearchresponse(dev, ssdpPkt, devUDN, devUDN);
  }
  else if (mupnp_st_isurn(ssdpST) == true) {
    devType = mupnp_device_getdevicetype(dev);
    if (mupnp_streq(ssdpST, devType) == true) {
      mupnp_device_getnotifydevicetypeusn(dev, deviceUSN, sizeof(deviceUSN));
      mupnp_device_postsearchresponse(dev, ssdpPkt, devType, deviceUSN);
    }
  }

  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service))
    mupnp_service_ssdpmessagereceived(service, ssdpPkt);

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev))
    mupnp_device_ssdpmessagereceived(childDev, ssdpPkt, false);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_device_ssdplistener
****************************************/

void mupnp_device_ssdplistener(mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpDevice* dev;

  mupnp_log_debug_l4("Entering...\n");

  dev = (mUpnpDevice*)mupnp_ssdp_packet_getuserdata(ssdpPkt);
  mupnp_device_ssdpmessagereceived(dev, ssdpPkt, true);

  mupnp_log_debug_l4("Leaving...\n");
}

/* private methods */

static int filter_duplicate_m_search(mUpnpSSDPPacket* ssdpPkt)
{
  mUpnpTime* timestamps = ssdpPkt->timestamps;
  size_t s_length;
  int loc;
  const char* st;
  char *id_string, *r_address, port[6];
  mUpnpTime curr_time;

  mupnp_log_debug_l4("Entering...\n");

  /* Initializing hash table to zero */
  if (!ssdpPkt->initialized) {
    ssdpPkt->initialized = 1;
    memset(timestamps, '\0', MUPNP_SSDP_FILTER_TABLE_SIZE * sizeof(mUpnpTime));
  }

  r_address = mupnp_string_getvalue(ssdpPkt->dgmPkt->remoteAddress);
  st = mupnp_ssdp_packet_getst(ssdpPkt);
  sprintf(port, "%d", ssdpPkt->dgmPkt->remotePort);

  /* Catenating remote address string with ssdp ST header field. */
  s_length = strlen(r_address) + strlen(st) + strlen(port);
  id_string = (char*)malloc(s_length + 1);

  if (NULL == id_string) {
    mupnp_log_debug_s("Memory allocation problem!\n");
    return false;
  }

  memset(id_string, '\0', s_length + 1);

  mupnp_strcat(id_string, r_address);
  mupnp_strcat(id_string, port);
  mupnp_strcat(id_string, st);

  loc = simple_string_hash(id_string, MUPNP_SSDP_FILTER_TABLE_SIZE);

  mupnp_log_debug("Calculated hash: %d\n", loc);

  free(id_string);

  curr_time = mupnp_getcurrentsystemtime();

  if (0 == timestamps[loc]) {
    timestamps[loc] = curr_time;
    mupnp_log_debug("First packet... Updating hash table.\n");
    return false;
  }
  else if ((curr_time - timestamps[loc]) < MUPNP_DEVICE_M_SEARCH_FILTER_INTERVAL) {
    mupnp_log_debug("Filtering packet!\n");
    timestamps[loc] = curr_time;
    return true;
  }
  else {
    timestamps[loc] = curr_time;
    mupnp_log_debug("Old timestamp found, just updating it.\n");
    return false;
  }

  mupnp_log_debug_l4("Leaving...\n");
}

static int simple_string_hash(char* str, int table_size)
{
  int sum = 0;

  mupnp_log_debug_l4("Entering...\n");

  if (str == NULL)
    return -1;

  mupnp_log_debug("Calculating hash from string |%s|, table size: %d\n", str, table_size);

  /* Sum up all the characters in the string */
  for (; *str; str++)
    sum += *str;

  mupnp_log_debug_l4("Leaving...\n");

  return sum % table_size;
}
