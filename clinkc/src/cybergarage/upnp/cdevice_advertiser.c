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
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/ctime.h>

static void cg_upnp_device_advertiser_action(CgThread *thread) 
{
	CgUpnpDevice *dev;
	long leaseTime;
	long notifyInterval;
	 
	dev = (CgUpnpDevice *)cg_thread_getuserdata(thread);
	leaseTime = cg_upnp_device_getleasetime(dev);
		
	while (cg_thread_isrunnable(thread) == TRUE) {
		notifyInterval = (leaseTime/4) + (long)((float)leaseTime * (cg_random() * 0.25f));
		notifyInterval *= 1000;
		cg_wait(notifyInterval);
		cg_upnp_device_announce(dev);
	}
}

/****************************************
* cg_upnp_device_advertiser_start
****************************************/

BOOL cg_upnp_device_advertiser_start(CgUpnpDevice *dev)
{
	CgThread *advertiser;
	
	advertiser = cg_upnp_device_getadvertiserthead(dev);
	cg_thread_setuserdata(advertiser, dev);
	cg_thread_setaction(advertiser, cg_upnp_device_advertiser_action);
	return cg_thread_start(advertiser);
}

/****************************************
* cg_upnp_device_advertiser_stop
****************************************/

BOOL cg_upnp_device_advertiser_stop(CgUpnpDevice *dev)
{
	CgThread *advertiser;
	
	advertiser = cg_upnp_device_getadvertiserthead(dev);
	
	return cg_thread_stop(advertiser);
}

