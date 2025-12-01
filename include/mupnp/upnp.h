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

/**
 * @file upnp.h
 * @brief Main header file for the mUPnP for C library
 *
 * @details
 * This is the primary include file for applications using mUPnP for C.
 * It provides access to all major UPnP functionality through a single include.
 *
 * mUPnP for C is a development package that simplifies the creation of
 * UPnP™ devices and control points by abstracting the underlying protocols
 * (SSDP, SOAP, GENA, HTTP, HTTPU) and providing a clean, easy-to-use API.
 *
 * # Key Components
 *
 * ## Control Point (Client) API
 * Control points discover and interact with UPnP devices on the network:
 * - mUpnpControlPoint - Main control point interface
 * - Device discovery via M-SEARCH (SSDP)
 * - Action invocation via SOAP
 * - Event subscription and notification (GENA)
 *
 * ## Device (Server) API
 * Devices advertise services and respond to control point requests:
 * - mUpnpDevice - Device implementation interface
 * - Service advertisement via SSDP
 * - Action handling via SOAP
 * - Event notification to subscribers
 *
 * ## Service and Action Model
 * Services contain actions and state variables:
 * - mUpnpService - Service container
 * - mUpnpAction - Remote procedure calls
 * - mUpnpStateVariable - Service state with optional eventing
 *
 * # Basic Usage
 *
 * ## Control Point Example
 * @code
 * #include <mupnp/upnp.h>
 *
 * void device_listener(mUpnpControlPoint* cp, const char* udn, 
 *                      mUpnpDeviceStatus status) {
 *     if (status == mUpnpDeviceStatusAdded) {
 *         printf("Device added: %s\n", udn);
 *     }
 * }
 *
 * int main() {
 *     // Create and start control point
 *     mUpnpControlPoint* cp = mupnp_controlpoint_new();
 *     mupnp_controlpoint_setdevicelistener(cp, device_listener);
 *     mupnp_controlpoint_start(cp);
 *     
 *     // Discover devices
 *     mupnp_controlpoint_search(cp, "ssdp:all");
 *     
 *     // ... use devices ...
 *     
 *     // Cleanup
 *     mupnp_controlpoint_stop(cp);
 *     mupnp_controlpoint_delete(cp);
 *     return 0;
 * }
 * @endcode
 *
 * ## Device Example
 * @code
 * #include <mupnp/upnp.h>
 *
 * bool action_listener(mUpnpAction* action) {
 *     const char* name = mupnp_action_getname(action);
 *     if (strcmp(name, "GetStatus") == 0) {
 *         mUpnpArgument* arg = mupnp_action_getargumentbyname(action, "Status");
 *         mupnp_argument_setvalue(arg, "OK");
 *         return true;
 *     }
 *     return false;
 * }
 *
 * int main() {
 *     // Create device and load description
 *     mUpnpDevice* dev = mupnp_device_new();
 *     mupnp_device_loaddescriptionfile(dev, "description.xml");
 *     
 *     // Set action listener
 *     mupnp_device_setactionlistener(dev, action_listener);
 *     
 *     // Start device
 *     mupnp_device_start(dev);
 *     
 *     // ... run device ...
 *     
 *     // Cleanup
 *     mupnp_device_stop(dev);
 *     mupnp_device_delete(dev);
 *     return 0;
 * }
 * @endcode
 *
 * # Thread Safety
 * The mUPnP library is designed for multi-threaded environments:
 * - Most objects have internal mutexes for thread safety
 * - Control points should be locked when accessing device lists
 * - Listener callbacks may be invoked from multiple threads
 * - See individual function documentation for specific thread-safety notes
 *
 * # Platform Support
 * mUPnP for C supports multiple platforms:
 * - Linux/Unix (POSIX)
 * - Windows (Win32)
 * - macOS
 * - Embedded systems (ITRON, T-Engine, BTRON)
 *
 * @note UPnP™ is a certification mark of the UPnP™ Implementers Corporation.
 *
 * @see http://cybergarage.github.io/mupnp/ for documentation
 * @see https://github.com/cybergarage/mupnp for source code
 *
 * @version 3.0.2
 */

#ifndef _MUPNP_UPNP_H_
#define _MUPNP_UPNP_H_

#include <mupnp/typedef.h>

#include <mupnp/upnp_function.h>
#include <mupnp/upnp_status.h>
#include <mupnp/upnp_limit.h>

#include <mupnp/device.h>
#include <mupnp/controlpoint.h>
#include <mupnp/control/control.h>
#include <mupnp/event/event.h>

/****************************************
* Define
****************************************/

#define MUPNP_NAME "mUPnP"
#define MUPNP_VER "3.0.2"

#define MUPNP_DLNA_VER "1.50"

#define MUPNP_XML_DECLARATION "<?xml version=\"1.0\"?>"

#endif
