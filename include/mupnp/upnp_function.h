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

#ifndef _MUPNP_FUNCTION_H_
#define _MUPNP_FUNCTION_H_

#include <mupnp/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_UUID_NAME "uuid"
#define MUPNP_UUID_MAX_LEN 128 /*(5 + ((4+1)*4) + 1)*/
#define MUPNP_SEVERNAME_MAXLEN 64
#define MUPNP_CONFIGID_UPNP_ORG_MAX 16777215

/****************************************
* Data Type
****************************************/

/****************************************
* Function
****************************************/

/**
 * Create a UUID (Universally Unique IDentifier) based on a semi-random value using current time.
 *
 * \param uuidBuf The UUID created by this function (must not be NULL)
 * \return The UUID created by this function (== \e uuidBuf)
 */
const char* mupnp_createuuid(char* uuidBuf, size_t uuidBufSize);

/**
 * Get the server name ("OSname/OSversion/UPnP/<version> mUPnP/<version>")
 *
 * \param buf Buffer to store the server name
 * \param bufSize Buffer length
 * \param The server name buffer (== \e buf)
 */
const char* mupnp_getservername(char* buf, size_t bufSize);

/**
 * Enable/disable NMPR (Intel Networked Media Product Requirements) mode.
 *
 * \param onFlag true: enable; false: disable
 */
void mupnp_setnmprmode(bool onFlag);

/**
 * Check whether NMPR (Intel Networked Media Product Requirements) mode is on.
 * 
 *
 * \return true if on; otherwise false
 */
bool mupnp_isnmprmode();

/**
 * Generate a integer id of BOOTID.UPNP.ORG.
 * 
 *
 * \return BOOTID
 */
int mupnp_createbootid();

/**
 * Generate a integer id of CONFIGID.UPNP.ORG.
 *
 *
 * \return CONFIGID
 */
int mupnp_generateconfigid(const char* string);

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
