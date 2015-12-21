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

#include <mupnp/event/event.h>
#include <mupnp/event/notify.h>
#include <mupnp/event/property.h>
#include <mupnp/util/log.h>

#include <time.h>

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (Begin)
****************************************/

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

/****************************************
* mupnp_property_new
****************************************/

mUpnpProperty* mupnp_property_new()
{
  mUpnpProperty* prop;

  mupnp_log_debug_l4("Entering...\n");

  prop = (mUpnpProperty*)malloc(sizeof(mUpnpProperty));

  if (NULL != prop) {
    mupnp_list_node_init((mUpnpList*)prop);

    prop->name = mupnp_string_new();
    prop->value = mupnp_string_new();
    prop->sid = mupnp_string_new();

    mupnp_property_setseq(prop, 0);
  }

  return prop;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_property_delete
****************************************/

void mupnp_property_delete(mUpnpProperty* prop)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_property_clear(prop);
  mupnp_list_remove((mUpnpList*)prop);

  mupnp_string_delete(prop->name);
  mupnp_string_delete(prop->value);
  mupnp_string_delete(prop->sid);

  free(prop);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_property_clear
****************************************/

void mupnp_property_clear(mUpnpProperty* prop)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_string_setvalue(prop->name, NULL);
  mupnp_string_setvalue(prop->value, NULL);
  mupnp_string_setvalue(prop->sid, NULL);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* MUPNP_NOUSE_SUBSCRIPTION (End)
****************************************/

#endif
