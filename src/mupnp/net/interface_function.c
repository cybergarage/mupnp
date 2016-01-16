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

#include <mupnp/net/interface.h>
#include <mupnp/net/socket.h>
#include <mupnp/util/log.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

#if defined(WIN32)
#define MUPNP_USE_WIN32_GETADAPTERSINFO 1
#endif

#if defined(__APPLE_CC__) && !defined(HAVE_IFADDRS_H)
#define HAVE_IFADDRS_H 1
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(ITRON) && !defined(_W32_WCE)
#include <Iptypes.h>
#include <Iphlpapi.h>
#elif defined(_W32_WCE)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#elif defined(BTRON) || (defined(TENGINE) && !defined(MUPNP_TENGINE_NET_KASAGO))
#include <net/sock_com.h>
#include <btron/bsocket.h>
#elif defined(ITRON)
#include <kernel.h>
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
#include <tk/tkernel.h>
#include <btron/kasago.h>
#include <sys/svc/ifkasago.h>
#else
#if defined(HAVE_IFADDRS_H)
#include <ifaddrs.h>
#if defined(HAVE_SIOCGIFHWADDR)
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#else
#include <sys/ioctl.h>
#endif
#include <netdb.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
extern ttUserInterface kaInterfaceHandle;
#endif

#if defined(ITRON)
char InterfaceAddress[MUPNP_NET_IPV6_ADDRSTRING_MAXSIZE];
bool IsInterfaceAddressInitialized = false;
#endif

/****************************************
* mupnp_net_gethostinterfaces (WIN32)
* (WINCE follows below)
****************************************/

#if ((defined(WIN32) && !defined(WINCE)) || defined(__CYGWIN__)) && !defined(ITRON)

#pragma message("******** WIN32 && !WINCE selected!")

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
#if !defined(MUPNP_USE_WIN32_GETHOSTADDRESSES) && !defined(MUPNP_USE_WIN32_GETADAPTERSINFO)
  mUpnpNetworkInterface* netIf;
  SOCKET sd;
  int nNumInterfaces;
  INTERFACE_INFO InterfaceList[20];
  unsigned long nBytesReturned, *pnBytesReturned = &nBytesReturned;
  struct sockaddr_in* pAddress;
  struct sockaddr_in* pNetmask;
  char* host;
  char* netmask;
  u_long nFlags;
  int i;

  mupnp_socket_startup();
  mupnp_net_interfacelist_clear(netIfList);

  sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
  //Theo Beisch WINSOCK2API WSASocket will return INVALID_SOCKET on error, not SOCKET_ERROR
  if (sd == INVALID_SOCKET) {
    return 0;
  }

  if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
    return 0;

  nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
  for (i = 0; i < nNumInterfaces; ++i) {
    nFlags = InterfaceList[i].iiFlags;
    //if (CyberNet::HostInterface::USE_ONLY_IPV4_ADDR == false) {
    if (nFlags & IFF_LOOPBACK)
      continue;
    //}
    if (!(nFlags & IFF_UP))
      continue;
    //if (IsUseAddress(host) == false)
    //	continue;

    netIf = mupnp_net_interface_new();

    pAddress = (struct sockaddr_in*)&(InterfaceList[i].iiAddress);
    host = inet_ntoa(pAddress->sin_addr);
    mupnp_net_interface_setaddress(netIf, host);

    pNetmask = (struct sockaddr_in*)&(InterfaceList[i].iiNetmask);
    netmask = inet_ntoa(pNetmask->sin_addr);
    mupnp_net_interface_setnetmask(netIf, netmask);

    mupnp_net_interfacelist_add(netIfList, netIf);
  }

#elif defined(MUPNP_USE_WIN32_GETADAPTERSINFO)
#pragma comment(lib, "Iphlpapi.lib")

  mUpnpNetworkInterface* netIf;
  PIP_ADAPTER_INFO pAdapterInfo = NULL, pAdapter = NULL;
  ULONG ulOutBufLen;
  DWORD dwRetVal;
  DWORD nOfInterfaces;

  mupnp_socket_startup();
  mupnp_net_interfacelist_clear(netIfList);

  ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
  }

  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
    for (pAdapter = pAdapterInfo, nOfInterfaces = 0; pAdapter; pAdapter = pAdapter->Next, ++nOfInterfaces) {
      if (pAdapter->Type == MIB_IF_TYPE_LOOPBACK)
        continue;
      if (mupnp_streq(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0"))
        continue;
      netIf = mupnp_net_interface_new();
      mupnp_net_interface_setaddress(netIf, pAdapter->IpAddressList.IpAddress.String);
      mupnp_net_interface_setnetmask(netIf, pAdapter->IpAddressList.IpMask.String);
      if (pAdapter->AddressLength == MUPNP_NET_MACADDR_SIZE)
        mupnp_net_interface_setmacaddress(netIf, pAdapter->Address);
      mupnp_net_interfacelist_add(netIfList, netIf);
    }
  }
  free(pAdapterInfo);

#elif defined(MUPNP_USE_WIN32_GETHOSTADDRESSES)
#pragma comment(lib, "Iphlpapi.lib")

  IP_ADAPTER_ADDRESSES *pAdapterAddresses, *ai;
  DWORD ifFlags;
  ULONG outBufLen;
  IP_ADAPTER_UNICAST_ADDRESS* uai;
  SOCKET_ADDRESS sockaddr;
  SOCKADDR* saddr;
  INT saddrlen;
  char addr[NI_MAXHOST];
  char port[NI_MAXSERV];
  int namInfoRet;
  int ifIdx;
  mUpnpNetworkInterface* netIf;

  mupnp_socket_startup();
  mupnp_net_interfacelist_clear(netIfList);

  outBufLen = 0;
  ifFlags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

  GetAdaptersAddresses(AF_UNSPEC, ifFlags, NULL, NULL, &outBufLen);
  pAdapterAddresses = (IP_ADAPTER_ADDRESSES*)LocalAlloc(LMEM_ZEROINIT, outBufLen);
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
        if (mupnp_net_isipv6address(addr) == true)
          ifIdx = mupnp_net_getipv6scopeid(addr);
        netIf = mupnp_net_interface_new();
        mupnp_net_interface_setaddress(netIf, addr);
        if (ai->PhysicalAddressLength == MUPNP_NET_MACADDR_SIZE)
          mupnp_net_interface_setmacaddress(netIf, ai->PhysicalAddress);
        mupnp_net_interface_setindex(netIf, ifIdx);
        mupnp_net_interfacelist_add(netIfList, netIf);
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

  return mupnp_net_interfacelist_size(netIfList);
}

#elif defined(WINCE)

/****************************************
* mupnp_net_gethostinterfaces (WINCE)
****************************************/

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mUpnpNetworkInterface* netIf;

  //iphelper API vars
  PIP_ADAPTER_INFO pAdapterInfo = NULL, pAdapter = NULL;
  ULONG ulOutBufLen;
  DWORD dwRetVal;
  DWORD nOfInterfaces;
  int i = 0;

  //mupnp_socket_startup();
  mupnp_net_interfacelist_clear(netIfList);

  // new code to determine interfaces available

  //try with default for single adapter
  ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
  }

  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
    for (pAdapter = pAdapterInfo, nOfInterfaces = 0; pAdapter; ++nOfInterfaces) {
#if defined(DEBUG)
      printf("IF Name:%s\n", pAdapter->AdapterName);
      printf("IF Desc:%s\n", pAdapter->Description);
      printf("IF Addr:%ld\n", pAdapter->Address);
      printf("IF Type:%d\n", pAdapter->Type);
      printf("IF Address:%s\n", pAdapter->IpAddressList.IpAddress.String);
      printf("IF Mask:%s\n", pAdapter->IpAddressList.IpMask.String);
      printf("IF Gateway:%s\n", pAdapter->GatewayList.IpAddress.String);
#endif

      /*
			if (pAdapter->DhcpEnabled) {
				printf("\tDHCP Enabled: Yes\n");
				printf("\t\tDHCP Server: \t%s\n", pAdapter->DhcpServer.IpAddress.String);
				printf("\tLease Obtained: %ld\n", pAdapter->LeaseObtained);
			}
			else
				printf("\tDHCP Enabled: No\n");
    
		    if (pAdapter->HaveWins) {
				printf("\tHave Wins: Yes\n");
				printf("\t\tPrimary Wins Server: \t%s\n", pAdapter->PrimaryWinsServer.IpAddress.String);
				printf("\t\tSecondary Wins Server: \t%s\n", pAdapter->SecondaryWinsServer.IpAddress.String);
			}
			else
				printf("\tHave Wins: No\n");
*/

      // now add adapter to list

      if (pAdapter->Type == MIB_IF_TYPE_ETHERNET) {
        // List will not contain loopback
        // IFF_UP check not required, ce only returns UP interfaces here
        //host = inet_ntoa(pAdapter->Address);
        netIf = mupnp_net_interface_new();
        mupnp_net_interface_setaddress(netIf, pAdapter->IpAddressList.IpAddress.String);
        mupnp_net_interface_setmacaddress(netIf, pAdapter->IpAddressList.Address.String);
        mupnp_net_interfacelist_add(netIfList, netIf);
      }
      pAdapter = pAdapter->Next;
    }
#if defined(DEBUG)
    printf("* %d Adapters found\n", nOfInterfaces);
    printf("  %d usable\n", mupnp_net_interfacelist_size(netIfList));
#endif
  }
  free(pAdapterInfo);

  i = mupnp_net_interfacelist_size(netIfList);

  if (i == 0) {
    printf("* no Adapters found - try at least localhost\n");
    netIf = mupnp_net_interface_new();
    mupnp_net_interface_setaddress(netIf, MUPNP_NET_IPV4_LOOPBACK);
    mupnp_net_interfacelist_add(netIfList, netIf);
  }

  return mupnp_net_interfacelist_size(netIfList);
}

#else

/****************************************
* mupnp_net_gethostinterfaces (UNIX)
****************************************/

#if defined(HAVE_IFADDRS_H)

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mUpnpNetworkInterface* netIf;
  struct ifaddrs* ifaddr;
  char addr[NI_MAXHOST + 1];
  char netmask[NI_MAXHOST + 1];
  char* ifname;
  struct ifaddrs* i;
#if defined(HAVE_SOCKADDR_DL)
  struct sockaddr_dl* dladdr;
#elif defined(HAVE_SIOCGIFHWADDR)
  int sock;
  struct ifreq ifr;
#endif

  mupnp_log_debug_l4("Entering...\n");

  mupnp_net_interfacelist_clear(netIfList);

  if (getifaddrs(&ifaddr) != 0) {
    mupnp_log_debug("No addresses for interfaces!\n");
    return 0;
  }

  for (i = ifaddr; i != NULL; i = i->ifa_next) {

    // Thanks for Ricardo Rivldo (04/10/12)
    //  - for some reason, vmware and virtualbox \"virtual\" interfaces does not return ifa_addr
    if (i->ifa_addr == NULL || i->ifa_netmask == NULL)
      continue;

    // Thanks for Tobias.Gansen (01/15/06)
    if (i->ifa_addr->sa_family != AF_INET)
      continue;
    if (!(i->ifa_flags & IFF_UP))
      continue;
    if (i->ifa_flags & IFF_LOOPBACK)
      continue;

    if (getnameinfo(i->ifa_addr, sizeof(struct sockaddr), addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) != 0)
      continue;

    if (getnameinfo(i->ifa_netmask, sizeof(struct sockaddr), netmask, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) != 0)
      continue;

    ifname = i->ifa_name;
    mupnp_log_debug("Interface name: %s, address: %s\n", ifname, addr);
    netIf = mupnp_net_interface_new();
    mupnp_net_interface_setname(netIf, ifname);
    mupnp_net_interface_setaddress(netIf, addr);
    mupnp_net_interface_setnetmask(netIf, netmask);
#if defined(HAVE_SOCKADDR_DL)
    dladdr = (struct sockaddr_dl*)(i->ifa_addr);
    mupnp_net_interface_setmacaddress(netIf, LLADDR(dladdr));
#elif defined(HAVE_SIOCGIFHWADDR)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    ifr.ifr_addr.sa_family = AF_INET;
    ioctl(sock, SIOCGIFHWADDR, &ifr);
    mupnp_net_interface_setmacaddress(netIf, ifr.ifr_hwaddr.sa_data);
    close(sock);
#endif
    mupnp_net_interfacelist_add(netIfList, netIf);
  }
  freeifaddrs(ifaddr);

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_net_interfacelist_size(netIfList);
}

#elif !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)

/* DELETE Fabrice Fontaine Orange 16/04/2007
// Deleting this line, so mUPnP for C stack can be used on target without getifaddrs
// function (like on uclibc)
#error This implementation lacks the interface id and cannot be currently used.
DELETE END Fabrice Fontaine Orange 16/04/2007 */

static const char* PATH_PROC_NET_DEV = "/proc/net/dev";

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpNetworkInterface* netIf;
  FILE* fd;
  int s;
  char buffer[256 + 1];
  char ifaddr[20 + 1];
  char* ifname;
  char* sep;

  mupnp_net_interfacelist_clear(netIfList);

  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
    return 0;
  fd = fopen(PATH_PROC_NET_DEV, "r");
  fgets(buffer, sizeof(buffer) - 1, fd);
  fgets(buffer, sizeof(buffer) - 1, fd);
  while (!feof(fd)) {
    ifname = buffer;
    sep;
    if (fgets(buffer, sizeof(buffer) - 1, fd) == NULL)
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
    strncpy(ifaddr, inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr), sizeof(ifaddr) - 1);
    netIf = mupnp_net_interface_new();
    mupnp_net_interface_setname(netIf, ifname);
    mupnp_net_interface_setaddress(netIf, ifaddr);
    mupnp_net_interfacelist_add(netIfList, netIf);
    mupnp_log_debug("Interface name: %s, address: %s\n", ifname, ifaddr);
    //cout << ifname << ", " << ifaddr << endl;
  }
  fclose(fd);
  close(s);
  return mupnp_net_interfacelist_size(netIfList);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

#endif

/****************************************
* mupnp_net_gethostinterfaces (BTRON)
****************************************/

#if defined(BTRON) || (defined(TENGINE) && !defined(MUPNP_TENGINE_NET_KASAGO))

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpNetworkInterface* netIf;
  struct hostent hostEnt;
  B buf[HBUFLEN];
  ERR err;
  char* ifname = MUPNP_NET_DEFAULT_IFNAME;
  char ifaddr[32];

  mupnp_net_interfacelist_clear(netIfList);

  err = so_gethostbyname("localhost", &hostEnt, buf);
  if (err != 0)
    return 0;

  inet_ntop(hostEnt.h_addrtype, hostEnt.h_addr, ifaddr, sizeof(ifname));

  netIf = mupnp_net_interface_new();
  mupnp_net_interface_setname(netIf, ifname);
  mupnp_net_interface_setaddress(netIf, ifaddr);
  mupnp_net_interfacelist_add(netIfList, netIf);

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_net_interfacelist_size(netIfList);
}

#endif

/****************************************
* mupnp_net_gethostinterfaces (TENGINE-KASAGO)
****************************************/

#if defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpNetworkInterface* netIf;
  struct in_addr inAddr;
  char ipaddr[MUPNP_NET_IPV6_ADDRSTRING_MAXSIZE];
  int kaRet;

  mupnp_socket_startup();

  inAddr.s_addr = 0;
  kaRet = ka_tfGetIpAddress(kaInterfaceHandle, &(inAddr.s_addr), 0);
  if (kaRet != 0)
    return 0;

  ka_tfInetToAscii((unsigned long)inAddr.s_addr, ipaddr);

  netIf = mupnp_net_interface_new();
  mupnp_net_interface_setname(netIf, MUPNP_NET_DEFAULT_IFNAME);
  mupnp_net_interface_setaddress(netIf, ipaddr);
  mupnp_net_interfacelist_add(netIfList, netIf);

  return mupnp_net_interfacelist_size(netIfList);

  mupnp_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* mupnp_net_gethostinterfaces (ITRON)
****************************************/

#if defined(ITRON)

void mupnp_net_setinterface(const char* ifaddr)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_strcpy(InterfaceAddress, ifaddr);
  IsInterfaceAddressInitialized = true;

  mupnp_log_debug_l4("Leaving...\n");
}

int mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* netIfList)
{
  mUpnpNetworkInterface* netIf;

  mupnp_log_debug_l4("Entering...\n");

  if (IsInterfaceAddressInitialized == false)
    return 0;

  netIf = mupnp_net_interface_new();
  mupnp_net_interface_setname(netIf, "");
  mupnp_net_interface_setaddress(netIf, InterfaceAddress);
  mupnp_net_interfacelist_add(netIfList, netIf);

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_net_interfacelist_size(netIfList);
}

#endif

/****************************************
* mupnp_net_selectaddr
****************************************/

#if !defined(HAVE_IFADDRS_H) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
char* mupnp_net_selectaddr(struct sockaddr* remoteaddr)
{
  mUpnpNetworkInterfaceList* netIfList;
  mUpnpNetworkInterface* netIf;
  mUpnpNetworkInterface* selectNetIf;
  char* selectNetIfAddr;
  u_long laddr, lmask, raddr;
  struct addrinfo hints;
  struct addrinfo* netIfAddrInfo;
  struct addrinfo* netMaskAddrInfo;

  netIfList = mupnp_net_interfacelist_new();
  if (!netIfList)
    return mupnp_strdup("127.0.0.1");

  if (mupnp_net_gethostinterfaces(netIfList) <= 0) {
    mupnp_net_interfacelist_delete(netIfList);
    return mupnp_strdup("127.0.0.1");
  }

  raddr = ntohl(((struct sockaddr_in*)remoteaddr)->sin_addr.s_addr);

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;

  selectNetIf = NULL;
  if (1 <= mupnp_net_gethostinterfaces(netIfList)) {
    for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
      if (getaddrinfo(mupnp_net_interface_getaddress(netIf), NULL, &hints, &netIfAddrInfo) != 0)
        continue;
      if (getaddrinfo(mupnp_net_interface_getnetmask(netIf), NULL, &hints, &netMaskAddrInfo) != 0) {
        freeaddrinfo(netIfAddrInfo);
        continue;
      }
      laddr = ntohl(((struct sockaddr_in*)netIfAddrInfo->ai_addr)->sin_addr.s_addr);
      lmask = ntohl(((struct sockaddr_in*)netMaskAddrInfo->ai_addr)->sin_addr.s_addr);
      if ((laddr & lmask) == (raddr & lmask))
        selectNetIf = netIf;
      freeaddrinfo(netIfAddrInfo);
      freeaddrinfo(netMaskAddrInfo);
      if (selectNetIf)
        break;
    }
  }

  if (!selectNetIf)
    selectNetIf = mupnp_net_interfacelist_gets(netIfList);

  selectNetIfAddr = mupnp_strdup(mupnp_net_interface_getaddress(selectNetIf));

  mupnp_net_interfacelist_delete(netIfList);

  return selectNetIfAddr;
}
#else
char* mupnp_net_selectaddr(struct sockaddr* remoteaddr)
{
  struct ifaddrs *ifaddrs, *ifaddr;
  uint32_t laddr, lmask, raddr;
  char *address_candidate = NULL, *auto_ip_address_candidate = NULL;

  raddr = ntohl(((struct sockaddr_in*)remoteaddr)->sin_addr.s_addr);

  if (0 != getifaddrs(&ifaddrs)) {
    return NULL;
  }

  for (ifaddr = ifaddrs; NULL != ifaddr; ifaddr = ifaddr->ifa_next) {
    if (ifaddr->ifa_addr == NULL)
      continue;
    if (!(ifaddr->ifa_flags & IFF_UP))
      continue;
    if (ifaddr->ifa_flags & IFF_LOOPBACK)
      continue;
    if (ifaddr->ifa_flags & IFF_POINTOPOINT)
      continue;

    laddr = ntohl(((struct sockaddr_in*)ifaddr->ifa_addr)->sin_addr.s_addr);
    if (NULL != (struct sockaddr_in*)ifaddr->ifa_netmask)
      lmask = ntohl(((struct sockaddr_in*)ifaddr->ifa_netmask)->sin_addr.s_addr);
    else {
      mupnp_log_debug_s("No netmask for address %u!\n", laddr);
      continue;
    }

    /* Checking if we have an exact subnet match */
    if ((laddr & lmask) == (raddr & lmask)) {
      if (NULL != address_candidate)
        free(address_candidate);
      address_candidate = mupnp_strdup(
          inet_ntoa((struct in_addr)((struct sockaddr_in*)ifaddr->ifa_addr)->sin_addr));
      mupnp_log_debug_s("Address match! Selecting local address (%u)\n", laddr);
      break;
    }

    /* Checking if we have and auto ip address */
    if ((laddr & lmask) == MUPNP_NET_SOCKET_AUTO_IP_NET) {
      mupnp_log_debug_s("Found auto ip address. Selecting it for second address candidate (%u)\n", laddr);
      if (NULL != auto_ip_address_candidate)
        free(auto_ip_address_candidate);
      auto_ip_address_candidate = mupnp_strdup(
          inet_ntoa((struct in_addr)((struct sockaddr_in*)ifaddr->ifa_addr)->sin_addr));
    }
    /* Good. We have others than auto ips present. */
    else {
      mupnp_log_debug_s("Didn't have an exact subnet match, but non auto ip address anyway... (%u)\n", laddr);
      if (NULL != address_candidate)
        free(address_candidate);
      address_candidate = mupnp_strdup(
          inet_ntoa((struct in_addr)((struct sockaddr_in*)ifaddr->ifa_addr)->sin_addr));
    }
  }

  freeifaddrs(ifaddrs);

  if (NULL != address_candidate) {
    if (NULL != auto_ip_address_candidate)
      free(auto_ip_address_candidate);
    return address_candidate;
  }

  if (NULL != auto_ip_address_candidate) {
    if (NULL != address_candidate)
      free(address_candidate);
    return auto_ip_address_candidate;
  }

  /* Starting to feel desperate and returning local address.*/

  return mupnp_strdup("127.0.0.1");
}

#endif
