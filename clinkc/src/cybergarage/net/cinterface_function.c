/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cinterface_function.cpp
*
*	Revision:
*
*	02/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/net/cinterface.h>
#include <cybergarage/net/csocket.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(ITRON)
	#include <Iptypes.h>
	#include <Iphlpapi.h>
#elif defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	#include <net/sock_com.h>
	#include <btron/bsocket.h>
#elif defined(ITRON)
	#include <kernel.h>
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	#include <tk/tkernel.h>
	#include <btron/kasago.h>
	#include <sys/svc/ifkasago.h>
#else
	#if defined(HAVE_IFADDRS_H)
		#include <ifaddrs.h>
	#else
		#include <sys/ioctl.h>
	#endif
	#include <netdb.h>
	#include <net/if.h>
#endif

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
extern ttUserInterface kaInterfaceHandle;
#endif

#if defined(ITRON)
char InterfaceAddress[CG_NET_IPV6_ADDRSTRING_MAXSIZE];
BOOL IsInterfaceAddressInitialized = FALSE;
#endif

/****************************************
* cg_net_gethostinterfaces (WIN32)
****************************************/

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(ITRON)

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
#if !defined(CG_USE_WIN32_GETHOSTADDRESSES)
	CgNetworkInterface *netIf;
	SOCKET sd;
	int nNumInterfaces;
	INTERFACE_INFO InterfaceList[20];
	unsigned long nBytesReturned;
	struct sockaddr_in *pAddress;
	char *host;
	u_long nFlags;
	int i;

	cg_socket_startup();
	cg_net_interfacelist_clear(netIfList);

	sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if (sd == SOCKET_ERROR)
		return 0;

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
		return 0;

	nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
    for (i = 0; i < nNumInterfaces; ++i) {
		pAddress = (struct sockaddr_in *) & (InterfaceList[i].iiAddress);
		host = inet_ntoa(pAddress->sin_addr);
		nFlags = InterfaceList[i].iiFlags;
		//if (CyberNet::HostInterface::USE_ONLY_IPV4_ADDR == false) {
			if (nFlags & IFF_LOOPBACK)
				continue;
		//}
		if (!(nFlags & IFF_UP))
			continue;
		//if (IsUseAddress(host) == false)
		//	continue;
		netIf = cg_net_interface_new();
		cg_net_interface_setaddress(netIf, host);
		cg_net_interfacelist_add(netIfList, netIf);
	}

#else
	IP_ADAPTER_ADDRESSES *pAdapterAddresses, *ai;
	DWORD ifFlags;
	ULONG outBufLen;
	IP_ADAPTER_UNICAST_ADDRESS *uai;
	SOCKET_ADDRESS sockaddr;
	SOCKADDR *saddr;
	INT saddrlen;
	char addr[NI_MAXHOST];
	char port[NI_MAXSERV];
	int namInfoRet;
	int ifIdx;
	CgNetworkInterface *netIf;

	cg_socket_startup();
	cg_net_interfacelist_clear(netIfList);

	outBufLen = 0;
	ifFlags = 
		GAA_FLAG_SKIP_ANYCAST | 
		GAA_FLAG_SKIP_FRIENDLY_NAME | 
		GAA_FLAG_SKIP_MULTICAST | 
		GAA_FLAG_SKIP_DNS_SERVER;

	GetAdaptersAddresses(AF_UNSPEC, ifFlags, NULL, NULL, &outBufLen);
	pAdapterAddresses = (IP_ADAPTER_ADDRESSES *) LocalAlloc(LMEM_ZEROINIT, outBufLen);
	GetAdaptersAddresses(AF_UNSPEC, ifFlags, NULL, pAdapterAddresses, &outBufLen);
	ai = pAdapterAddresses;
	while (ai != NULL) {
		if (ai->OperStatus != IfOperStatusUp) {
			ai = ai->Next;
			continue;
		}
		if (ai->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
			ai = ai->Next;
			continue;
		}
		if (ai->IfType == IF_TYPE_TUNNEL) {
			ai = ai->Next;
			continue;
		}
		uai = ai->FirstUnicastAddress;
		while (uai != NULL) {
			sockaddr = uai->Address;
			saddr = sockaddr.lpSockaddr;
			saddrlen = sockaddr.iSockaddrLength;
			namInfoRet = getnameinfo(saddr, saddrlen, addr, sizeof(addr), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
			if (namInfoRet == 0) {
				//if (IsUseAddress(addr) == true) {
					ifIdx = 0;
					if (cg_net_isipv6address(addr) == TRUE)
						ifIdx = cg_net_getipv6scopeid(addr);
					netIf = cg_net_interface_new();
					cg_net_interface_setaddress(netIf, addr);
					cg_net_interface_setindex(netIf, ifIdx);
					cg_net_interfacelist_add(netIfList, netIf);
				//}
			}
			else {
				int err = WSAGetLastError();
			}
			uai = uai->Next;
		}
		ai = ai->Next;
	}
	LocalFree(pAdapterAddresses);

#endif

	return cg_net_interfacelist_size(netIfList);
}

#else

/****************************************
* cg_net_gethostinterfaces (UNIX)
****************************************/

#if defined(HAVE_IFADDRS_H)

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;
	struct ifaddrs *ifaddr;
	char addr[NI_MAXHOST+1];
	char *ifname;
	int ifIdx;
	
	cg_net_interfacelist_clear(netIfList);
	
	if (getifaddrs(&ifaddr) != 0)
		return 0;
	while (ifaddr != NULL) {
		if (!(ifaddr->ifa_flags & IFF_UP)) {
			ifaddr = ifaddr->ifa_next;
			continue;
		}
		if (ifaddr->ifa_flags & IFF_LOOPBACK) {
			ifaddr = ifaddr->ifa_next;
			continue;
		}		
		if (getnameinfo(ifaddr->ifa_addr, sizeof(struct sockaddr), addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
			//if (IsUseAddress(addr) == true) {
				ifname = ifaddr->ifa_name;
				ifIdx = if_nametoindex(ifname);
				//printf("%s %d\n",  ifname, ifIdx);
				netIf = cg_net_interface_new();
				cg_net_interface_setname(netIf, ifname);
				cg_net_interface_setaddress(netIf, addr);
				cg_net_interface_setindex(netIf,ifIdx);
				cg_net_interfacelist_add(netIfList, netIf);
			//}
		}
		ifaddr = ifaddr->ifa_next;
	}
	freeifaddrs(ifaddr);
	return cg_net_interfacelist_size(netIfList);
}

#elif !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)

static const char *PATH_PROC_NET_DEV = "/proc/net/dev";

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;
	FILE *fd;
	int s;
	char buffer[256+1];
	char ifaddr[20+1];
	char *ifname;
	char *sep;
	
	cg_net_interfacelist_clear(netIfList);
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		return 0;
	fd = fopen(PATH_PROC_NET_DEV, "r");
	fgets(buffer, sizeof(buffer)-1, fd);
	fgets(buffer, sizeof(buffer)-1, fd);
	while (!feof(fd)) {
		ifname = buffer;
		sep;
		if (fgets(buffer, sizeof(buffer)-1, fd) == NULL)
			break;
		sep = strrchr(buffer, ':');
		if (sep)
			*sep = 0;
		while (*ifname == ' ')
			ifname++;
		struct ifreq req;
		strcpy(req.ifr_name, ifname);
		if (ioctl(s, SIOCGIFFLAGS, &req) < 0)
			continue;
		if (!(req.ifr_flags & IFF_UP))
			continue;
		if (req.ifr_flags & IFF_LOOPBACK)
			continue;
		if (ioctl(s, SIOCGIFADDR, &req) < 0)
			continue;
		strncpy(ifaddr, inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr), sizeof(struct ifaddr)-1);
		netIf = cg_net_interface_new();
		cg_net_interface_setname(netIf, ifname);
		cg_net_interface_setaddress(netIf, ifaddr);
		cg_net_interfacelist_add(netIfList, netIf);
		//cout << ifname << ", " << ifaddr << endl;
	}
	fclose(fd);
	close(s);
	return cg_net_interfacelist_size(netIfList);
}


#endif

#endif

/****************************************
* cg_net_gethostinterfaces (BTRON)
****************************************/

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;
	struct hostent hostEnt;
	B buf[HBUFLEN];
	ERR err;
	char *ifname = CG_NET_DEFAULT_IFNAME;
	char ifaddr[32];

	cg_net_interfacelist_clear(netIfList);
	
	err = so_gethostbyname("localhost", &hostEnt, buf);
	if (err != 0)
		return 0;
	
	inet_ntop(hostEnt.h_addrtype, hostEnt.h_addr, ifaddr, sizeof(ifname));
	
	netIf = cg_net_interface_new();
	cg_net_interface_setname(netIf, ifname);
	cg_net_interface_setaddress(netIf, ifaddr);
	cg_net_interfacelist_add(netIfList, netIf);
	
	return cg_net_interfacelist_size(netIfList);
}

#endif

/****************************************
* cg_net_gethostinterfaces (TENGINE-KASAGO)
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;
    struct in_addr inAddr;
    char ipaddr[CG_NET_IPV6_ADDRSTRING_MAXSIZE];
	int kaRet;
	
	cg_socket_startup();

    inAddr.s_addr = 0;
    kaRet = ka_tfGetIpAddress(kaInterfaceHandle, &(inAddr.s_addr), 0);
    if(kaRet != 0)
		return 0;
		
    ka_tfInetToAscii((unsigned long)inAddr.s_addr, ipaddr);
	
	netIf = cg_net_interface_new();
	cg_net_interface_setname(netIf, CG_NET_DEFAULT_IFNAME);
	cg_net_interface_setaddress(netIf, ipaddr);
	cg_net_interfacelist_add(netIfList, netIf);
	
	return cg_net_interfacelist_size(netIfList);
}

#endif

/****************************************
* cg_net_gethostinterfaces (TENGINE-KASAGO)
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;
    struct in_addr inAddr;
    char ipaddr[CG_NET_IPV6_ADDRSTRING_MAXSIZE];
	int kaRet;
	
	cg_socket_startup();

    inAddr.s_addr = 0;
    kaRet = ka_tfGetIpAddress(kaInterfaceHandle, &(inAddr.s_addr), 0);
    if(kaRet != 0)
		return 0;
		
    ka_tfInetToAscii((unsigned long)inAddr.s_addr, ipaddr);
	
	netIf = cg_net_interface_new();
	cg_net_interface_setname(netIf, CG_NET_DEFAULT_IFNAME);
	cg_net_interface_setaddress(netIf, ipaddr);
	cg_net_interfacelist_add(netIfList, netIf);
	
	return cg_net_interfacelist_size(netIfList);
}

#endif

/****************************************
* cg_net_gethostinterfaces (ITRON)
****************************************/

#if defined(ITRON)

void cg_net_setinterface(char *ifaddr)
{
	cg_strcpy(InterfaceAddress, ifaddr);
	IsInterfaceAddressInitialized = TRUE;
}

int cg_net_gethostinterfaces(CgNetworkInterfaceList *netIfList)
{
	CgNetworkInterface *netIf;

	if (IsInterfaceAddressInitialized == FALSE)
			return 0;

	netIf = cg_net_interface_new();
	cg_net_interface_setname(netIf, "");
	cg_net_interface_setaddress(netIf, InterfaceAddress);
	cg_net_interfacelist_add(netIfList, netIf);
	
	return cg_net_interfacelist_size(netIfList);
}

#endif
