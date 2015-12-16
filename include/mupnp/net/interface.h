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

#ifndef _MUPNP_NET_CINTERFACE_H_
#define _MUPNP_NET_CINTERFACE_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>

#if !defined(WIN32)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_NET_IPV4_ADDRSTRING_MAXSIZE ((3 * 4) + (1 * 3) + 1)
#define MUPNP_NET_IPV6_ADDRSTRING_MAXSIZE (1 + (8 * 4) + (1 * 7) + 1 + 1)

#define MUPNP_NET_IPV4_LOOPBACK "127.0.0.1"
#define MUPNP_NET_IPV6_LOOPBACK "fixmelater"
#define MUPNP_NET_MACADDR_SIZE 6

#if defined(BTRON) || defined(TENGINE)
#define MUPNP_NET_DEFAULT_IFNAME "Neta"
#endif

/****************************************
* Data Type
****************************************/

typedef struct _mUpnpNetworkInterface {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* name;
  mUpnpString* ipaddr;
  mUpnpString* netmask;
  mUpnpByte macaddr[MUPNP_NET_MACADDR_SIZE];
  int index;
} mUpnpNetworkInterface, mUpnpNetworkInterfaceList;

/****************************************
* Function (NetworkInterface)
****************************************/

mUpnpNetworkInterface* mupnp_net_interface_new();
void mupnp_net_interface_delete(mUpnpNetworkInterface* netIf);
mUpnpNetworkInterface* mupnp_net_interface_getany();

#define mupnp_net_interface_next(netIf) (mUpnpNetworkInterface*) mupnp_list_next((mUpnpList*)netIf)
#define mupnp_net_interface_remove(netIf) mupnp_list_remove((mUpnpList*)netIf)

void mupnp_net_interface_setname(mUpnpNetworkInterface* netIf, char* name);
char* mupnp_net_interface_getname(mUpnpNetworkInterface* netIf);
void mupnp_net_interface_setaddress(mUpnpNetworkInterface* netIf, char* ipaddr);
char* mupnp_net_interface_getaddress(mUpnpNetworkInterface* netIf);
void mupnp_net_interface_setnetmask(mUpnpNetworkInterface* netIf, char* ipaddr);
char* mupnp_net_interface_getnetmask(mUpnpNetworkInterface* netIf);
char* mupnp_net_selectaddr(struct sockaddr* remoteaddr);

#define mupnp_net_interface_setmacaddress(netIf, value) memcpy(netIf->macaddr, value, MUPNP_NET_MACADDR_SIZE)
#define mupnp_net_interface_getmacaddress(netIf, buf) memcpy(buf, netIf->macaddr, MUPNP_NET_MACADDR_SIZE)

#define mupnp_net_interface_setindex(netIf, value) (netIf->index = value)
#define mupnp_net_interface_getindex(netIf, buf) (netIf->index)

/**
 * Compares two interfaces based on IP-address.
 */
int mupnp_net_interface_cmp(mUpnpNetworkInterface* netIfA,
    mUpnpNetworkInterface* netIfB);

/****************************************
* Function (NetworkInterfaceList)
****************************************/

mUpnpNetworkInterfaceList* mupnp_net_interfacelist_new();
void mupnp_net_interfacelist_delete(mUpnpNetworkInterfaceList* netIfList);

#define mupnp_net_interfacelist_clear(netIfList) mupnp_list_clear((mUpnpList*)netIfList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_net_interface_delete)
#define mupnp_net_interfacelist_size(netIfList) mupnp_list_size((mUpnpList*)netIfList)
#define mupnp_net_interfacelist_gets(netIfList) (mUpnpNetworkInterface*) mupnp_list_next((mUpnpList*)netIfList)
#define mupnp_net_interfacelist_add(netIfList, netIf) mupnp_list_add((mUpnpList*)netIfList, (mUpnpList*)netIf)

mUpnpNetworkInterface* mupnp_net_interfacelist_get(mUpnpNetworkInterfaceList* netIfList, char* name);

/**
 * Gets changes in the two given (aka old and new) interface lists. Changes
 * are resolved based on IP-addresses.
 * 
 * @param netIfListOld List of the old interfaces. It is changed!
 * @param netIfListNew List of the new interfaces. It is changed!
 * @param netIfListAdded List to store interfaces, which were in netIfListNew,
 *			 but were not in netIfListOld. May be NULL.
 * @param netIfListRemoved List to store interfaces, which were in 
 *			   netIfListOld, but were not in netIfListNew.
 */
void mupnp_net_interfacelist_getchanges(mUpnpNetworkInterfaceList* netIfListOld,
    mUpnpNetworkInterfaceList* netIfListNew,
    mUpnpNetworkInterfaceList* netIfListAdded,
    mUpnpNetworkInterfaceList* netIfListRemoved);

/****************************************
* Function
****************************************/

#if defined(ITRON)
void mupnp_net_setinterface(const char* ifaddr);
#endif

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList);

bool mupnp_net_isipv6address(const char* addr);
int mupnp_net_getipv6scopeid(const char* addr);

#ifdef __cplusplus
}
#endif

#endif
