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

#include <boost/test/unit_test.hpp>

#include <mupnp/net/interface.h>

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(NetworkInterface)
{
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
  mUpnpByte macAddr[MUPNP_NET_MACADDR_SIZE];
  mUpnpByte nullMacAddr[MUPNP_NET_MACADDR_SIZE];
  memset(nullMacAddr, 0, MUPNP_NET_MACADDR_SIZE);
#endif

  mUpnpNetworkInterfaceList* netIfList = mupnp_net_interfacelist_new();
  BOOST_CHECK(netIfList);
  BOOST_CHECK(0 < mupnp_net_gethostinterfaces(netIfList));
  for (mUpnpNetworkInterface* netIf = mupnp_net_interfacelist_gets(netIfList); netIf; netIf = mupnp_net_interface_next(netIf)) {
    char* ipaddr = mupnp_net_interface_getaddress(netIf);
    BOOST_CHECK(0 < mupnp_strlen(ipaddr));
    BOOST_CHECK(mupnp_streq(ipaddr, "0.0.0.0") == false);
#if defined(HAVE_SOCKADDR_DL) || defined(HAVE_SIOCGIFHWADDR)
    mupnp_net_interface_getmacaddress(netIf, macAddr);
    BOOST_CHECK(memcmp(macAddr, nullMacAddr, MUPNP_NET_MACADDR_SIZE) != 0);
#endif
    //BOOST_CHECK(0 < mupnp_strlen(mupnp_net_interface_getname(netIf)));
    //BOOST_CHECK(0 < mupnp_strlen(mupnp_net_interface_getnetmask(netIf)));
  }
  mupnp_net_interfacelist_delete(netIfList);
}
