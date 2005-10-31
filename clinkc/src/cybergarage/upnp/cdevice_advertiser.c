/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdevice_advertiser.c
*
*	Revision:
*
*	06/14/05
*		- first revision
*
*	10/31/05
*		- Added comments
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/ctime.h>

/**
 * cg_upnp_device_advertiser_action
 * 
 * Function run as a UPnP device advertisement
 * thread.
 *
 * \param thread The thread context that this function is running in
 */
static void cg_upnp_device_advertiser_action(CgThread *thread) 
{
  CgUpnpDevice *dev;
  long leaseTime;
  long notifyInterval;
  
  /**
   * Get the device struct that this function is advertising.
   */
  dev = (CgUpnpDevice *)cg_thread_getuserdata(thread);

  /**
   * Lease time
   */
  leaseTime = cg_upnp_device_getleasetime(dev);
  
  /* Run this thread until it is stopped from outside */
  while (cg_thread_isrunnable(thread) == TRUE)
    {
      notifyInterval = (leaseTime/4) + (long)((float)leaseTime * (cg_random() * 0.25f));
      notifyInterval *= 1000;

      /* Sleep *notifyInterval* msecs */
      cg_wait(notifyInterval); 

      /* Initiate advertise routine after sleeping */
      cg_upnp_device_announce(dev);
    }
}

/**
 * cg_upnp_device_advertiser_start
 *
 * Start the advertiser thread for the given device
 *
 * \param dev The device that is to be advertised
 */
BOOL cg_upnp_device_advertiser_start(CgUpnpDevice *dev)
{
  CgThread *advertiser;
  
  /* Get the already existing (created in device_new) advertiser thread */
  advertiser = cg_upnp_device_getadvertiserthead(dev);

  /* Store the device into the advertiser thread struct's user data */
  cg_thread_setuserdata(advertiser, dev);

  /* Set the function that will run the thread */
  cg_thread_setaction(advertiser, cg_upnp_device_advertiser_action);

  /* Start the thread */
  return cg_thread_start(advertiser);
}

/**
 * cg_upnp_device_advertiser_stop
 *
 * Stop the advertiser thread for the given device
 *
 * \param dev Device that is being advertised
 */
BOOL cg_upnp_device_advertiser_stop(CgUpnpDevice *dev)
{
  CgThread *advertiser;
  
  /* Get the advertiser thread from the device struct */
  advertiser = cg_upnp_device_getadvertiserthead(dev);
  
  /* Stop the thread */
  return cg_thread_stop(advertiser);
}
