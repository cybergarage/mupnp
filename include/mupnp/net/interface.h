/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cinterface.h
*
*	Revision:
*
*	02/09/05
*		- first revision
*	03/23/06 Theo Beisch
*		- added localhost constants
*	09/12/07
*		- Added the following functions to get MAC address.
*		  mupnp_net_interface_setmacaddress(), mupnp_net_interface_getmacaddress()
*		- Changed mupnp_net_gethostinterfaces() to get the MAC address using GetAdaptersInfo() as default on Windows platform.
*		- Changed mupnp_net_gethostinterfaces() to get the MAC address using getifaddrs() on UNIX platform.
*		   Note : Other platforms might not support to get this functions yet. 
*
******************************************************************/

#ifndef _CG_NET_CINTERFACE_H_
#define _CG_NET_CINTERFACE_H_

#include <mupnp/typedef.h>
#include <mupnp/util/string.h>
#include <mupnp/util/list.h>

#if !defined(WIN32)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_NET_IPV4_ADDRSTRING_MAXSIZE ((3*4)+(1*3)+1)
#define CG_NET_IPV6_ADDRSTRING_MAXSIZE (1+(8*4)+(1*7)+1+1)

#define CG_NET_IPV4_LOOPBACK "127.0.0.1"
#define CG_NET_IPV6_LOOPBACK "fixmelater"
#define CG_NET_MACADDR_SIZE 6

#if defined(BTRON) || defined(TENGINE)
#define CG_NET_DEFAULT_IFNAME "Neta"
#endif

/****************************************
* Data Type
****************************************/

typedef struct _CgNetworkInterface {
	BOOL headFlag;
	struct _CgNetworkInterface *prev;
	struct _CgNetworkInterface *next;
	CgString *name;
	CgString *ipaddr;
	CgString *netmask;
	CgByte macaddr[CG_NET_MACADDR_SIZE];
	int index;
} CgNetworkInterface, CgNetworkInterfaceList;

/****************************************
* Function (NetworkInterface)
****************************************/

CgNetworkInterface *mupnp_net_interface_new();
void mupnp_net_interface_delete(CgNetworkInterface *netIf);
CgNetworkInterface* mupnp_net_interface_getany();

#define mupnp_net_interface_next(netIf) (CgNetworkInterface *)mupnp_list_next((CgList *)netIf)
#define mupnp_net_interface_remove(netIf) mupnp_list_remove((CgList *)netIf)

void mupnp_net_interface_setname(CgNetworkInterface *netIf, char *name);
char *mupnp_net_interface_getname(CgNetworkInterface *netIf);
void mupnp_net_interface_setaddress(CgNetworkInterface *netIf, char *ipaddr);
char *mupnp_net_interface_getaddress(CgNetworkInterface *netIf);
void mupnp_net_interface_setnetmask(CgNetworkInterface *netIf, char *ipaddr);
char *mupnp_net_interface_getnetmask(CgNetworkInterface *netIf);
char *mupnp_net_selectaddr(struct sockaddr *remoteaddr);

#define mupnp_net_interface_setmacaddress(netIf, value) memcpy(netIf->macaddr, value, CG_NET_MACADDR_SIZE)
#define mupnp_net_interface_getmacaddress(netIf, buf) memcpy(buf, netIf->macaddr, CG_NET_MACADDR_SIZE)

#define mupnp_net_interface_setindex(netIf, value) (netIf->index = value)
#define mupnp_net_interface_getindex(netIf, buf) (netIf->index)

/**
 * Compares two interfaces based on IP-address.
 */
int mupnp_net_interface_cmp(CgNetworkInterface *netIfA, 
			 CgNetworkInterface *netIfB);

/****************************************
* Function (NetworkInterfaceList)
****************************************/

CgNetworkInterfaceList *mupnp_net_interfacelist_new();
void mupnp_net_interfacelist_delete(CgNetworkInterfaceList *netIfList);

#define mupnp_net_interfacelist_clear(netIfList) mupnp_list_clear((CgList *)netIfList, (CG_LIST_DESTRUCTORFUNC)mupnp_net_interface_delete)
#define mupnp_net_interfacelist_size(netIfList) mupnp_list_size((CgList *)netIfList)
#define mupnp_net_interfacelist_gets(netIfList) (CgNetworkInterface *)mupnp_list_next((CgList *)netIfList)
#define mupnp_net_interfacelist_add(netIfList,netIf) mupnp_list_add((CgList *)netIfList, (CgList *)netIf)

CgNetworkInterface *mupnp_net_interfacelist_get(CgNetworkInterfaceList *netIfList, char *name);

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
void mupnp_net_interfacelist_getchanges(CgNetworkInterfaceList *netIfListOld,
				     CgNetworkInterfaceList *netIfListNew,
				     CgNetworkInterfaceList *netIfListAdded,
				     CgNetworkInterfaceList *netIfListRemoved);

/****************************************
* Function
****************************************/

#if defined(ITRON)
void mupnp_net_setinterface(const char *ifaddr);
#endif

int mupnp_net_gethostinterfaces(CgNetworkInterfaceList *netIfList);

BOOL mupnp_net_isipv6address(const char *addr);
int mupnp_net_getipv6scopeid(const char *addr);

#ifdef  __cplusplus
}
#endif

#endif

