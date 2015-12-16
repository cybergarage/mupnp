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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/net/interface.h>
#include <mupnp/util/log.h>
#include <string.h>

/****************************************
* mupnp_net_interface_new
****************************************/

mUpnpNetworkInterface* mupnp_net_interface_new()
{
  mUpnpNetworkInterface* netIf;

  mupnp_log_debug_l4("Entering...\n");

  netIf = (mUpnpNetworkInterface*)malloc(sizeof(mUpnpNetworkInterface));

  if (NULL != netIf) {
    mupnp_list_node_init((mUpnpList*)netIf);
    netIf->name = mupnp_string_new();
    netIf->ipaddr = mupnp_string_new();
    netIf->netmask = mupnp_string_new();
    mupnp_net_interface_setindex(netIf, 0);
    memset(netIf->macaddr, 0, (size_t)MUPNP_NET_MACADDR_SIZE);
  }

  mupnp_log_debug_l4("Leaving...\n");

  return netIf;
}

/****************************************
* mupnp_net_interface_delete
****************************************/

void mupnp_net_interface_delete(mUpnpNetworkInterface* netIf)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)netIf);
  mupnp_string_delete(netIf->name);
  mupnp_string_delete(netIf->ipaddr);
  mupnp_string_delete(netIf->netmask);

  mupnp_log_debug_l4("Leaving...\n");

  free(netIf);
}

/****************************************
* mupnp_net_interface_getany
****************************************/

mUpnpNetworkInterface* mupnp_net_interface_getany()
{
  mUpnpNetworkInterface* netIf;

  mupnp_log_debug_l4("Entering...\n");

  netIf = mupnp_net_interface_new();
  /* It would be more approriate to use INADDR_ANY, but this will do */
  mupnp_net_interface_setname(netIf, "INADDR_ANY");
  mupnp_net_interface_setaddress(netIf, "0.0.0.0");

  mupnp_log_debug_l4("Leaving...\n");

  return netIf;
}

/****************************************
* mupnp_net_interface_setname
****************************************/

void mupnp_net_interface_setname(mUpnpNetworkInterface* netIf, char* name)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(netIf->name, name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_getname
****************************************/

char* mupnp_net_interface_getname(mUpnpNetworkInterface* netIf)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(netIf->name);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_setaddress
****************************************/

void mupnp_net_interface_setaddress(mUpnpNetworkInterface* netIf, char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(netIf->ipaddr, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_getaddress
****************************************/

char* mupnp_net_interface_getaddress(mUpnpNetworkInterface* netIf)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(netIf->ipaddr);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_setnetmask
****************************************/

void mupnp_net_interface_setnetmask(mUpnpNetworkInterface* netIf, char* value)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(netIf->netmask, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_getnetmask
****************************************/

char* mupnp_net_interface_getnetmask(mUpnpNetworkInterface* netIf)
{
  mupnp_log_debug_l4("Entering...\n");

  return mupnp_string_getvalue(netIf->netmask);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_net_interface_cmp
****************************************/

int mupnp_net_interface_cmp(mUpnpNetworkInterface* netIfA,
    mUpnpNetworkInterface* netIfB)
{
  mupnp_log_debug_l4("Entering...\n");

  if (netIfA == NULL && netIfB == NULL)
    return 0;
  if (netIfA == NULL && netIfB != NULL)
    return 1;
  if (netIfA != NULL && netIfB == NULL)
    return -1;

  return mupnp_strcmp(mupnp_net_interface_getaddress(netIfA),
      mupnp_net_interface_getaddress(netIfB));

  mupnp_log_debug_l4("Leaving...\n");
}
