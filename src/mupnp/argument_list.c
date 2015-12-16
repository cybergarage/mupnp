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

#include <mupnp/argument.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_argument_new
****************************************/

mUpnpArgumentList* mupnp_argumentlist_new()
{
  mUpnpArgument* argumentList;

  mupnp_log_debug_l4("Entering...\n");

  argumentList = (mUpnpArgument*)malloc(sizeof(mUpnpArgument));

  if (NULL != argumentList)
    mupnp_list_header_init((mUpnpList*)argumentList);

  return argumentList;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_argument_delete
****************************************/

void mupnp_argumentlist_delete(mUpnpArgumentList* argumentList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_clear((mUpnpList*)argumentList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_argument_delete);
  free(argumentList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_argumentlist_get
****************************************/

mUpnpArgument* mupnp_argumentlist_get(mUpnpArgumentList* argList, const char* name)
{
  mUpnpArgument* arg;
  const char* argName;

  mupnp_log_debug_l4("Entering...\n");

  if (name == NULL)
    return NULL;

  for (arg = mupnp_argumentlist_gets(argList); arg != NULL; arg = mupnp_argument_next(arg)) {
    argName = mupnp_argument_getname(arg);
    if (argName == NULL)
      continue;
    if (mupnp_strcasecmp(argName, name) == 0)
      return arg;
  }

  return NULL;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_argumentlist_set
****************************************/

void mupnp_argumentlist_set(mUpnpArgumentList* argList, mUpnpArgumentList* srcArgList)
{
  mUpnpArgument* arg;
  mUpnpArgument* srcArg;
  const char* srcArgName;

  mupnp_log_debug_l4("Entering...\n");

  for (srcArg = mupnp_argumentlist_gets(srcArgList); srcArg != NULL; srcArg = mupnp_argument_next(srcArg)) {
    srcArgName = mupnp_argument_getname(srcArg);
    arg = mupnp_argumentlist_get(argList, srcArgName);
    if (arg == NULL)
      continue;
    mupnp_argument_setvalue(arg, mupnp_argument_getvalue(srcArg));
  }

  mupnp_log_debug_l4("Leaving...\n");
}
