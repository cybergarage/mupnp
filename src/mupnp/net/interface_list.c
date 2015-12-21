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

#include <mupnp/util/list.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_net_interfacelist_new
****************************************/

mUpnpNetworkInterfaceList* mupnp_net_interfacelist_new()
{
  mUpnpNetworkInterfaceList* netIfList;

  mupnp_log_debug_l4("Entering...\n");

  netIfList = (mUpnpNetworkInterfaceList*)malloc(sizeof(mUpnpNetworkInterfaceList));

  if (NULL != netIfList) {
    mupnp_list_header_init((mUpnpList*)netIfList);
    netIfList->name = NULL;
    netIfList->ipaddr = NULL;
  }

  return netIfList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_delete
****************************************/

void mupnp_net_interfacelist_delete(mUpnpNetworkInterfaceList* netIfList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_net_interfacelist_clear(netIfList);
  free(netIfList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_getinterface
****************************************/

mUpnpNetworkInterface* mupnp_net_interfacelist_get(mUpnpNetworkInterfaceList* netIfList, char* name)
{
  mUpnpNetworkInterface* netIf;
  char* ifName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (netIf = mupnp_net_interfacelist_gets(netIfList); netIf != NULL; netIf = mupnp_net_interface_next(netIf)) {
    ifName = mupnp_net_interface_getname(netIf);
    if (ifName == NULL)
      continue;
    if (mupnp_strcasecmp(ifName, name) == 0)
      return netIf;
  }

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interfacelist_getchanges
****************************************/

void mupnp_net_interfacelist_getchanges(mUpnpNetworkInterfaceList* netIfListOld,
    mUpnpNetworkInterfaceList* netIfListNew,
    mUpnpNetworkInterfaceList* netIfListAdded,
    mUpnpNetworkInterfaceList* netIfListRemoved)
{
  mUpnpNetworkInterface *netIfOld, *netIfNew, *tmp;
  bool found;

  mupnp_log_debug_l4("Entering...\n");

  /* Browse through old interfaces and check, if they are in the new */
  tmp = mupnp_net_interfacelist_gets(netIfListOld);
  while (tmp != NULL) {
    netIfOld = tmp;
    tmp = mupnp_net_interface_next(netIfOld);

    found = false;
    for (netIfNew = mupnp_net_interfacelist_gets(netIfListNew); netIfNew != NULL;
         netIfNew = mupnp_net_interface_next(netIfNew)) {
      if (mupnp_net_interface_cmp(netIfOld, netIfNew) == 0) {
        found = true;
        break;
      }
    }

    /* Old interface was not found in new ones, so it's removed */
    if (found == false) {
      mupnp_net_interface_remove(netIfOld);
      if (netIfListRemoved != NULL)
        mupnp_net_interfacelist_add(netIfListRemoved,
            netIfOld);
    }
  }

  /* Browse through new interfaces and check, if they are in the 
	   remaining old interfaces */
  tmp = mupnp_net_interfacelist_gets(netIfListNew);
  while (tmp != NULL) {
    netIfNew = tmp;
    tmp = mupnp_net_interface_next(netIfNew);

    found = false;
    for (netIfOld = mupnp_net_interfacelist_gets(netIfListOld); netIfOld != NULL;
         netIfOld = mupnp_net_interface_next(netIfOld)) {
      if (mupnp_net_interface_cmp(netIfOld, netIfNew) == 0) {
        found = true;
        break;
      }
    }

    /* New interface was not found in old ones, so it's added */
    if (found == false) {
      mupnp_net_interface_remove(netIfNew);
      if (netIfListAdded != NULL)
        mupnp_net_interfacelist_add(netIfListAdded,
            netIfNew);
    }
  }

  mupnp_log_debug_l4("Leaving...\n");
}
