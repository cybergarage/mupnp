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
#include <mupnp/util/time.h>
#include <mupnp/util/log.h>

/**
 * mupnp_device_advertiser_action
 * 
 * Function run as a UPnP device advertisement
 * thread.
 *
 * \param thread The thread context that this function is running in
 */
static void mupnp_device_advertiser_action(mUpnpThread* thread)
{
  mUpnpDevice* dev;
  mUpnpTime leaseTime;
  mUpnpTime notifyInterval;

  mupnp_log_debug_l4("Entering...\n");

  /**
   * Get the device struct that this function is advertising.
   */
  dev = (mUpnpDevice*)mupnp_thread_getuserdata(thread);

  /**
   * Lease time
   */
  leaseTime = mupnp_device_getleasetime(dev);

  /* Run this thread until it is stopped from outside */
  while (mupnp_thread_isrunnable(thread) == true) {
    notifyInterval = (leaseTime / 4) + (long)((float)leaseTime * (mupnp_random() * 0.25f));
    notifyInterval *= 1000;

    /* Sleep *notifyInterval* msecs */
    mupnp_wait(notifyInterval);

    /* Check if we must exit before sending new announce */
    if (mupnp_thread_isrunnable(thread) == false)
      break;

    /* Initiate advertise routine after sleeping */
    mupnp_device_announce(dev);
  }

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * mupnp_device_advertiser_start
 *
 * Start the advertiser thread for the given device
 *
 * \param dev The device that is to be advertised
 */
bool mupnp_device_advertiser_start(mUpnpDevice* dev)
{
  mUpnpThread* advertiser;

  mupnp_log_debug_l4("Entering...\n");

  /* Get the already existing (created in device_new) advertiser thread */
  advertiser = mupnp_device_getadvertiserthead(dev);

  /* Store the device into the advertiser thread struct's user data */
  mupnp_thread_setuserdata(advertiser, dev);

  /* Set the function that will run the thread */
  mupnp_thread_setaction(advertiser, mupnp_device_advertiser_action);

  /* Start the thread */
  return mupnp_thread_start(advertiser);

  mupnp_log_debug_l4("Leaving...\n");
}

/**
 * mupnp_device_advertiser_stop
 *
 * Stop the advertiser thread for the given device
 *
 * \param dev Device that is being advertised
 */
bool mupnp_device_advertiser_stop(mUpnpDevice* dev)
{
  mUpnpThread* advertiser;

  mupnp_log_debug_l4("Entering...\n");

  /* Get the advertiser thread from the device struct */
  advertiser = mupnp_device_getadvertiserthead(dev);

  //Theo Beisch
  if (advertiser != NULL)
    return mupnp_thread_stop(advertiser);
  return false;
}

/**
 * mupnp_device_advertiser_isrunning
 *
 * Check if the advertiser has been started
 *
 * \param dev Device that is being advertised
 */
bool mupnp_device_advertiser_isrunning(mUpnpDevice* dev)
{
  mUpnpThread* advertiser;

  mupnp_log_debug_l4("Entering...\n");

  advertiser = mupnp_device_getadvertiserthead(dev);
  if (advertiser != NULL)
    return mupnp_thread_isrunnable(advertiser);

  return false;
}
