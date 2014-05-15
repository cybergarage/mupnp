/************************************************************
 *
 *	CyberLink for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ************************************************************/

#include <cybergarage/net/cinterface.h>

#include <boost/test/unit_test.hpp>

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(NetworkInterface)
{
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
	CgByte macAddr[CG_NET_MACADDR_SIZE];
	CgByte nullMacAddr[CG_NET_MACADDR_SIZE];
	memset(nullMacAddr, 0, CG_NET_MACADDR_SIZE);
#endif

	CgNetworkInterfaceList *netIfList = cg_net_interfacelist_new();
	BOOST_CHECK(netIfList);
	BOOST_CHECK(0 < cg_net_gethostinterfaces(netIfList));
	for (CgNetworkInterface *netIf=cg_net_interfacelist_gets(netIfList); netIf; netIf=cg_net_interface_next(netIf)) {
		char *ipaddr = cg_net_interface_getaddress(netIf);
		BOOST_CHECK(0 < cg_strlen(ipaddr));
		BOOST_CHECK(cg_streq(ipaddr, "0.0.0.0") == FALSE);
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
		cg_net_interface_getmacaddress(netIf, macAddr);
		BOOST_CHECK(memcmp(macAddr, nullMacAddr, CG_NET_MACADDR_SIZE) != 0);
#endif
		//BOOST_CHECK(0 < cg_strlen(cg_net_interface_getname(netIf)));
		//BOOST_CHECK(0 < cg_strlen(cg_net_interface_getnetmask(netIf)));
	}
	cg_net_interfacelist_delete(netIfList);
}
