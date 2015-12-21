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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/typedef.h>

#if defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
#define HAVE_UUID_UUID_H 1
#define HAVE_LIBUUID 1
#define HAVE_UUID_UNPARSE_LOWER 1
#endif

#if defined(HAVE_UUID_UUID_H) || defined(HAVE_LIBUUID)
#include <uuid/uuid.h>
#endif

#include <mupnp/upnp.h>
#include <mupnp/util/log.h>

#include <time.h>
#include <stdlib.h>
#include <limits.h>

// Some systems (Solaris, CentOS?) come with libuuid, but does not feature
// the uuid_unparse_lower() call.
#if defined(HAVE_LIBUUID)
#if !defined(HAVE_UUID_UNPARSE_LOWER)
// Older versions of libuuid don't have uuid_unparse_lower(),
// only uuid_unparse()
void uuid_unparse_lower(uuid_t uu, char* out)
{
  int i;
  uuid_unparse(uu, out);
  // Characters in out are either 0-9, a-z, '-', or A-Z.  A-Z is mapped to
  // a-z by bitwise or with 0x20, and the others already have this bit set
  for (i = 0; i < sizeof(uu); ++i)
    out[i] |= 0x20;
}
#endif
#endif

/****************************************
* Static
****************************************/

static bool isUpnpNMPRMode = false;

/****************************************
* mupnp_createuuid
****************************************/

const char* mupnp_createuuid(char* uuidBuf, size_t uuidBufSize)
{
#if defined(HAVE_LIBUUID)
  uuid_t uuid;
  char uuidStr[MUPNP_UUID_MAX_LEN];
#elif defined(WIN32)
#pragma comment(lib, "Rpcrt4.lib")
  UUID uuid;
  unsigned char* szUuid = NULL;
#else
  time_t time1;
  time_t time2;
#endif

  mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_LIBUUID)
  uuid_generate(uuid);
  uuid_unparse_lower(uuid, uuidStr);
  snprintf(uuidBuf, uuidBufSize, "uuid:%s", uuidStr);
#elif defined(WIN32)
  UuidCreate(&uuid);
  UuidToString(&uuid, &szUuid);
  sprintf(uuidBuf, "%s:%s", MUPNP_UUID_NAME, szUuid);
  RpcStringFree(&szUuid);
#else
  /**** Thanks for Makela Aapo (10/30/05) ****/
  time1 = mupnp_getcurrentsystemtime();
  time2 = (time_t)((double)mupnp_getcurrentsystemtime(NULL) * ((double)rand() / (double)RAND_MAX));
  snprintf(uuidBuf, uuidBufSize, "%s:%04x-%04x-%04x-%04x", MUPNP_UUID_NAME, (int)(time1 & 0xFFFF), (int)(((time1 >> 31) | 0xA000) & 0xFFFF), (int)(time2 & 0xFFFF), (int)(((time2 >> 31) | 0xE000) & 0xFFFF));
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return uuidBuf;
}

/****************************************
* uuid_unparse_lower
****************************************/

/* Some systems (Solaris, CentOS?) come with libuuid, but does not feature
    the uuid_unparse_lower() call. */

#if defined(HAVE_UUID_UUID_H) && !defined(HAVE_UUID_UNPARSE_LOWER)
/* Older versions of libuuid don't have uuid_unparse_lower(),
   only uuid_unparse() */
void uuid_unparse_lower(uuid_t uu, char* out)
{
  int i;
  uuid_unparse(uu, out);
  /*Characters in out are either 0-9, a-z, '-', or A-Z.  A-Z is mapped to
  a-z by bitwise or with 0x20, and the others already have this bit set */
  for (i = 0; i < sizeof(uu); ++i)
    out[i] |= 0x20;
}
#endif

/****************************************
* mupnp_getservername
****************************************/

const char* mupnp_getservername(char* buf, size_t bufSize)
{
  size_t nameLen;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_http_getservername(buf, bufSize);
  nameLen = mupnp_strlen(buf);
  bufSize -= nameLen;
  if (bufSize <= 0)
    return buf;
#if defined(HAVE_SNPRINTF)
  snprintf((buf + nameLen), bufSize, " %s/%s UPnP/%s DLNADOC/%s", MUPNP_NAME, MUPNP_VER, MUPNP_VER, MUPNP_DLNA_VER);
#else
  sprintf((buf + nameLen), " %s/%s UPnP/%s DLNADOC/%s", MUPNP_NAME, MUPNP_VER, MUPNP_VER, MUPNP_DLNA_VER);
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_setnmprmode
****************************************/

void mupnp_setnmprmode(bool onFlag)
{
  mupnp_log_debug_l4("Entering...\n");

  isUpnpNMPRMode = onFlag;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_isnmprmode
****************************************/

bool mupnp_isnmprmode()
{
  mupnp_log_debug_l4("Entering...\n");

  return isUpnpNMPRMode;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_createbootid
****************************************/

int mupnp_createbootid()
{
  mUpnpTime currentTime;

  mupnp_log_debug_l4("Entering...\n");

  currentTime = mupnp_getcurrentsystemtime();

  mupnp_log_debug_l4("Leaving...\n");

  return (int)(currentTime % INT_MAX);
}

/****************************************
 * mupnp_generateconfigid
 ****************************************/

int mupnp_generateconfigid(const char* string)
{
  int configId = 0;
  size_t n;

  for (n = 0; n < mupnp_strlen(string); n++) {
    configId += string[n];
    if (configId < MUPNP_CONFIGID_UPNP_ORG_MAX)
      continue;
    configId = configId % MUPNP_CONFIGID_UPNP_ORG_MAX;
  }
  return configId;
}
