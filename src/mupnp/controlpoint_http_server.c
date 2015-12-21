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

#include <mupnp/controlpoint.h>
#include <mupnp/event/notify.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_CONTROLPOINT)

/**
 * The function that calls all HTTP listener callback functions. Do not call
 * this from applications.
 *
 * @param httpReq The received HTTP request
 */
void mupnp_controlpoint_httprequestreceived(mUpnpHttpRequest* httpReq)
{
  mUpnpControlPoint* ctrlPoint = NULL;
  mUpnpNotifyRequest* notifyReq = NULL;
  mUpnpPropertyList* propList = NULL;
  mUpnpProperty* prop = NULL;
  mUpnpEventListenerList* eventListeners = NULL;
  const char* sid = NULL;
  long seq = 0;
  long timeout = 0;
  mUpnpDevice* dev = NULL;
  mUpnpService* service = NULL;
  int notifyListeners = 0;

  mupnp_log_debug_l4("Entering...\n");

  ctrlPoint = (mUpnpControlPoint*)mupnp_http_request_getuserdata(httpReq);

  mupnp_controlpoint_lock(ctrlPoint);

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  if (mupnp_http_request_isnotifyrequest(httpReq) == true) {
    notifyReq = mupnp_event_notify_request_new();
    mupnp_event_notify_request_sethttprequest(notifyReq, httpReq);

    /* Get service according to SID */
    sid = mupnp_event_notify_request_getsid(notifyReq);

    for (dev = mupnp_controlpoint_getdevices(ctrlPoint);
         dev != NULL;
         dev = mupnp_device_next(dev)) {
      service = mupnp_device_getservicebysid(dev, sid);
      if (service != NULL)
        break;
    }

    if (service != NULL) {
      /* We found a service */
      seq = mupnp_event_notify_request_getseq(notifyReq);

      /* Check that event key = previous + 1 */
      if (seq != 0 && seq != mupnp_service_geteventkey(service) + 1) {
        /* The sequence does not match, unsubscribe and subscribe */
        timeout = mupnp_service_getsubscriptiontimeout(service);
        mupnp_controlpoint_unsubscribe(ctrlPoint, service);
        mupnp_controlpoint_subscribe(ctrlPoint, service, timeout);
      }
      else {
        /* Wrap seq, so that assertion is true next time */
        if (seq == MUPNP_EVENT_MAX_SEQ)
          seq = 0;

        /* Set event key */
        mupnp_service_seteventkey(service, seq);

        notifyListeners = 1;
        propList = mupnp_event_notify_request_getpropertylist(notifyReq);
        for (prop = mupnp_propertylist_gets(propList);
             prop != NULL;
             prop = mupnp_property_next(prop)) {
          /* Update the service's state table from the event */
          mupnp_controlpoint_updatestatetablefromproperty(service, prop);
        }
      }
    }
    eventListeners = mupnp_controlpoint_geteventlisteners(ctrlPoint);
    mupnp_controlpoint_unlock(ctrlPoint);

    if (notifyListeners && propList != NULL) {
      /* Notify listeners out of control point lock */
      for (prop = mupnp_propertylist_gets(propList);
           prop != NULL;
           prop = mupnp_property_next(prop)) {
        mupnp_eventlistenerlist_notify(eventListeners, prop);
      }
    }

    mupnp_event_notify_request_delete(notifyReq);
    mupnp_http_request_postokrequest(httpReq);

    return;
  }
#endif

  mupnp_controlpoint_unlock(ctrlPoint);
  mupnp_http_request_postbadrequest(httpReq);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * When an event is received, update also the associated service's
 * state table to include the evented state variable.
 *
 * @param service The service, whose state table to update
 * @param prop The evented property from which to update
 */
void mupnp_controlpoint_updatestatetablefromproperty(mUpnpService* service,
    mUpnpProperty* prop)
{
  mUpnpStateVariable* var = NULL;

  mupnp_log_debug_l4("Entering...\n");

  if (service == NULL || prop == NULL) {
    return;
  }

  var = mupnp_service_getstatevariablebyname(service,
      mupnp_property_getname(prop));

  if (var) {
    mupnp_statevariable_setvalue(var,
        mupnp_property_getvalue(prop));
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_CONTROLPOINT (End)
****************************************/

#endif
