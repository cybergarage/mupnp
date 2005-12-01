/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cinterface.h
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#ifndef _CG_NET_CINTERFACE_H_
#define _CG_NET_CINTERFACE_H_

#include <cybergarage/typedef.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/clist.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_NET_IPV4_ADDRSTRING_MAXSIZE ((3*4)+(1*3)+1)
#define CG_NET_IPV6_ADDRSTRING_MAXSIZE (1+(8*4)+(1*7)+1+1)

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
	int index;
} CgNetworkInterface, CgNetworkInterfaceList;

/****************************************
* Function (NetworkInterface)
****************************************/

CgNetworkInterface *cg_net_interface_new();
void cg_net_interface_delete(CgNetworkInterface *netIf);
CgNetworkInterface* cg_net_interface_getany();

#define cg_net_interface_next(netIf) (CgNetworkInterface *)cg_list_next((CgList *)netIf)

void cg_net_interface_setname(CgNetworkInterface *netIf, char *name);
char *cg_net_interface_getname(CgNetworkInterface *netIf);
void cg_net_interface_setaddress(CgNetworkInterface *netIf, char *ipaddr);
char *cg_net_interface_getaddress(CgNetworkInterface *netIf);
void cg_net_interface_setindex(CgNetworkInterface *netIf, int index);
int cg_net_interface_getindex(CgNetworkInterface *netIf);

/****************************************
* Function (NetworkInterfaceList)
****************************************/

CgNetworkInterfaceList *cg_net_interfacelist_new();
void cg_net_interfacelist_delete(CgNetworkInterfaceList *netIfList);

#define cg_net_interfacelist_clear(netIfList) cg_list_clear((CgList *)netIfList, (CG_LIST_DESTRUCTORFUNC)cg_net_interface_delete)
#define cg_net_interfacelist_size(netIfList) cg_list_size((CgList *)netIfList)
#define cg_net_interfacelist_gets(netIfList) (CgNetworkInterface *)cg_list_next((CgList *)netIfList)
#define cg_net_interfacelist_add(netIfList,netIf) cg_list_add((CgList *)netIfList, (CgList *)netIf)

CgNetworkInterface *cg_net_interfacelist_get(CgNetworkInterfaceList *netIfList, char *name);

/****************************************
* Function
****************************************/

#if defined(ITRON)
void cg_net_setinterface(char *ifaddr);
#endif

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList);

BOOL cg_net_isipv6address(char *addr);
int cg_net_getipv6scopeid(char *addr);

#ifdef  __cplusplus
}
#endif

#endif

