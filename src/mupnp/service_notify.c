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

#include <mupnp/service.h>
#include <mupnp/util/log.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_service_notifymain
****************************************/

static bool mupnp_service_notifymain(mUpnpService* service, mUpnpStateVariable* statVar)
{
  mUpnpSubscriber* sub;
  mUpnpSubscriber** subArray;
  int subArrayCnt;
  int n;

  mupnp_log_debug_l4("Entering...\n");

  mupnp_service_lock(service);

  /**** Remove expired subscribers ****/
  subArrayCnt = mupnp_service_getnsubscribers(service);
  subArray = (mUpnpSubscriber**)malloc(sizeof(mUpnpSubscriber*) * subArrayCnt);

  if (NULL == subArray) {
    mupnp_log_debug_s("Memory allocation problem!\n");
    mupnp_service_unlock(service);
    return false;
  }

  sub = mupnp_service_getsubscribers(service);
  for (n = 0; n < subArrayCnt; n++) {
    subArray[n] = sub;
    sub = mupnp_subscriber_next(sub);
  }
  for (n = 0; n < subArrayCnt; n++) {
    sub = subArray[n];
    if (sub == NULL)
      continue;
    if (mupnp_subscriber_isexpired(sub) == true)
      mupnp_service_removesubscriber(service, sub);
  }
  free(subArray);

  /**** Notify to subscribers ****/
  subArrayCnt = mupnp_service_getnsubscribers(service);
  subArray = (mUpnpSubscriber**)malloc(sizeof(mUpnpSubscriber*) * subArrayCnt);

  if (NULL == subArray) {
    mupnp_log_debug_s("Memory allocation problem!\n");
    mupnp_service_unlock(service);
    return false;
  }

  sub = mupnp_service_getsubscribers(service);
  for (n = 0; n < subArrayCnt; n++) {
    subArray[n] = sub;
    sub = mupnp_subscriber_next(sub);
  }
  for (n = 0; n < subArrayCnt; n++) {
    sub = subArray[n];
    if (sub == NULL)
      continue;
    if (statVar) {
      if (mupnp_subscriber_notify(sub, statVar) == false) {
        /**** remove invalid the subscriber but don't remove in NMPR specification ****/
        mupnp_service_removesubscriber(service, sub);
      }
    }
    else {
      if (mupnp_subscriber_notifyall(sub, service) == false) {
        /**** remove invalid the subscriber but don't remove in NMPR specification ****/
        mupnp_service_removesubscriber(service, sub);
      }
    }
  }
  free(subArray);

  mupnp_service_unlock(service);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
* mupnp_service_notify
****************************************/

bool mupnp_service_notify(mUpnpService* service, mUpnpStateVariable* statVar)
{
  return mupnp_service_notifymain(service, statVar);
}

/****************************************
* mupnp_service_notifyall
****************************************/

bool mupnp_service_notifyallbracket(mUpnpService* service)
{
  return mupnp_service_notifymain(service, NULL);
}

/****************************************
* mupnp_service_notifyall
****************************************/

bool mupnp_service_notifyall(mUpnpService* service, bool doBracket)
{
  mUpnpStateVariable* statVar;

  mupnp_log_debug_l4("Entering...\n");

  if (doBracket) {
    mupnp_service_notifyallbracket(service);
  }
  else {
    for (statVar = mupnp_service_getstatevariables(service); statVar != NULL; statVar = mupnp_statevariable_next(statVar)) {
      if (mupnp_statevariable_issendevents(statVar) == true)
        mupnp_service_notify(service, statVar);
    }
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}

/****************************************
 * mupnp_service_notifyall
 ****************************************/

static void mupnp_service_notifyall_thread(mUpnpThread* thread)
{
  mUpnpService* service;

  service = (mUpnpService*)mupnp_thread_getuserdata(thread);
  mupnp_service_notifyall(service, true);
  mupnp_thread_delete(thread);
}

void mupnp_service_createnotifyallthread(mUpnpService* service, mUpnpTime waitTime)
{
  mUpnpThread* thread;

  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, mupnp_service_notifyall_thread);
  mupnp_thread_setuserdata(thread, service);

  mupnp_wait(waitTime);
  mupnp_thread_start(thread);
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
