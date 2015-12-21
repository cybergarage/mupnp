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
#include <mupnp/util/log.h>

/****************************************
* mupnp_list_header_init
****************************************/

void mupnp_list_header_init(mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return;

  list->headFlag = true;
  list->prev = list->next = list;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_list_node_init
****************************************/

void mupnp_list_node_init(mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return;

  list->headFlag = false;
  list->prev = list->next = list;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_list_size
****************************************/

int mupnp_list_size(mUpnpList* headList)
{
  mUpnpList* list;
  int listCnt;

  mupnp_log_debug_l5("Entering...\n");

  if (NULL == headList)
    return 0;

  listCnt = 0;
  for (list = mupnp_list_next(headList); list != NULL; list = mupnp_list_next(list))
    listCnt++;

  mupnp_log_debug_l5("Leaving...\n");

  return listCnt;
}

/****************************************
* mupnp_list_get
****************************************/

mUpnpList* mupnp_list_get(mUpnpList* headList, int index)
{
  mUpnpList* list;
  int n;

  mupnp_log_debug_l5("Entering...\n");

  if (NULL == headList)
    return NULL;

  list = mupnp_list_next(headList);
  for (n = 0; n < index; n++) {
    if (NULL == list)
      break;
    list = mupnp_list_next(list);
  }

  mupnp_log_debug_l5("Leaving...\n");

  return list;
}

/****************************************
* mupnp_list_insert
****************************************/

void mupnp_list_insert(
    mUpnpList* prevList,
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if ((NULL == prevList) || (NULL == list))
    return;

  list->prev = prevList;
  list->next = prevList->next;
  prevList->next->prev = list;
  prevList->next = list;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_list_add
****************************************/

void mupnp_list_add(
    mUpnpList* headList,
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if ((NULL == headList) || (NULL == list))
    return;

  if (NULL == headList->prev)
    return;

  mupnp_list_insert(headList->prev, list);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_list_remove
****************************************/

void mupnp_list_remove(mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return;

  if ((NULL == list->prev) || (NULL == list->next))
    return;

  list->prev->next = list->next;
  list->next->prev = list->prev;
  list->prev = list->next = list;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_list_prev_circular
****************************************/

mUpnpList* mupnp_list_prev_circular(
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return NULL;

  if (NULL == list->prev)
    return NULL;

  if (list->prev->headFlag)
    list = list->prev;

  mupnp_log_debug_l5("Leaving...\n");

  return list->prev;
}

/****************************************
* mupnp_list_prev
****************************************/

mUpnpList* mupnp_list_prev(
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return NULL;

  if (NULL == list->prev)
    return NULL;

  if (list->prev->headFlag == true)
    return NULL;

  mupnp_log_debug_l5("Leaving...\n");

  return list->prev;
}

/****************************************
* mupnp_list_next_circular
****************************************/

mUpnpList* mupnp_list_next_circular(
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return NULL;

  if (NULL == list->next)
    return NULL;

  if (list->next->headFlag == true)
    list = list->next;

  mupnp_log_debug_l5("Leaving...\n");

  return list->next;
}

/****************************************
* mupnp_list_next
****************************************/

mUpnpList* mupnp_list_next(
    mUpnpList* list)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == list)
    return NULL;

  if (NULL == list->next)
    return NULL;

  if (list->next->headFlag == true)
    return NULL;

  mupnp_log_debug_l5("Leaving...\n");

  return list->next;
}

/****************************************
* mupnp_list_clear
****************************************/

void mupnp_list_clear(mUpnpList* headList, MUPNP_LIST_DESTRUCTORFUNC destructorFunc)
{
  mUpnpList* list;

  mupnp_log_debug_l5("Entering...\n");

  if (NULL == headList)
    return;

  list = mupnp_list_next(headList);
  while (list != NULL) {
    mupnp_list_remove(list);
    //Theo Beisch: use destructorFunc or just free(listElement)
    if (destructorFunc != NULL) {
      destructorFunc(list);
    }
    else {
      free(list);
    }
    list = mupnp_list_next(headList);
  }

  /*** list header must be deleted by user ***/
  /* free(headList); */

  mupnp_log_debug_l5("Leaving...\n");
}
