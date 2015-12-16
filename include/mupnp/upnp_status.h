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

#ifndef _MUPNP_UPNPSTATUS_H_
#define _MUPNP_UPNPSTATUS_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 * Defines
 ********************************************************************************/

#define MUPNP_STATUS_INVALID_ACTION 401
#define MUPNP_STATUS_INVALID_ARGS 402
#define MUPNP_STATUS_OUT_OF_SYNC 403
#define MUPNP_STATUS_INVALID_VAR 404
#define MUPNP_STATUS_ACTION_FAILED 501

/****************************************
 * Data Type
 ****************************************/

/**
 * UPnP status struct used in action and state operations
 */
typedef struct _mUpnpStatus {
  /** Status code */
  int code;

  /** Status description */
  mUpnpString* description;
} mUpnpStatus;

/******************************************************************************
 * Function
 ******************************************************************************/

/**
 * Create a new status struct
 *
 */
mUpnpStatus* mupnp_status_new();

/**
 * Delete a status struct
 *
 * \param upnpStat Status struct to delete
 */
void mupnp_status_delete(mUpnpStatus* upnpStat);

/**
 * Set status code for status struct
 *
 * \param upnpStat Status struct
 * \param value Status code
 */
#define mupnp_status_setcode(upnpStat, value) (upnpStat->code = value)

/**
 * Get Status code from a status struct
 *
 * \param upnpStat Status structure
 */
#define mupnp_status_getcode(upnpStat) (upnpStat->code)

/**
 * Set status description for a status struct
 *
 * \param upnpStat Status struct
 * \param value Status description
 */
#define mupnp_status_setdescription(upnpStat, value) mupnp_string_setvalue(upnpStat->description, value)

/**
 * Get status description from a status struct
 *
 * \param upnpStat Status struct
 */
#define mupnp_status_getdescription(upnpStat) mupnp_string_getvalue(upnpStat->description)

/**
 * Helper function that converts a status code into a status description
 *
 * \param code Code to convert into a description
 * \return Status description
 */
char* mupnp_status_code2string(int code);

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
