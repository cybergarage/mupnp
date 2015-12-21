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

#include <mupnp/util/dictionary.h>
#include <mupnp/util/log.h>

/****************************************
* mupnp_dictionary_new
****************************************/

mUpnpDictionary* mupnp_dictionary_new()
{
  mUpnpDictionary* dictionaryList;

  mupnp_log_debug_l4("Entering...\n");

  dictionaryList = (mUpnpDictionary*)malloc(sizeof(mUpnpDictionary));

  if (NULL != dictionaryList) {
    mupnp_list_header_init((mUpnpList*)dictionaryList);
    dictionaryList->key = NULL;
    dictionaryList->value = NULL;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return dictionaryList;
}

/****************************************
* mupnp_dictionary_delete
****************************************/

void mupnp_dictionary_delete(mUpnpDictionary* dictionaryList)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_dictionary_clear(dictionaryList);
  free(dictionaryList);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_dictionary_getelement
****************************************/

mUpnpDictionaryElement* mupnp_dictionary_getelement(mUpnpDictionary* dir, const char* key)
{
  mUpnpDictionaryElement* dirElem;
  char* dirElemKey;

  mupnp_log_debug_l4("Entering...\n");

  if (NULL == dir || mupnp_strlen(key) <= 0)
    return NULL;

  for (dirElem = mupnp_dictionary_gets(dir); dirElem != NULL; dirElem = mupnp_dictionary_element_next(dirElem)) {
    dirElemKey = mupnp_dictionary_element_getkey(dirElem);
    if (mupnp_strlen(dirElemKey) <= 0)
      continue;
    if (mupnp_streq(dirElemKey, key) == true)
      return dirElem;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return NULL;
}

/****************************************
* mupnp_dictionary_setvalue
****************************************/

void mupnp_dictionary_setvalue(mUpnpDictionary* dir, const char* key, const char* value)
{
  mUpnpDictionaryElement* dirElem;

  mupnp_log_debug_l4("Entering...\n");

  dirElem = mupnp_dictionary_getelement(dir, key);
  if (NULL == dirElem) {
    dirElem = mupnp_dictionary_element_new();
    mupnp_dictionary_add(dir, dirElem);
  }
  mupnp_dictionary_element_setkey(dirElem, key);
  mupnp_dictionary_element_setvalue(dirElem, value);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_dictionary_getvalue
****************************************/

const char* mupnp_dictionary_getvalue(mUpnpDictionary* dir, const char* key)
{
  mUpnpDictionaryElement* dirElem;

  mupnp_log_debug_l4("Entering...\n");

  dirElem = mupnp_dictionary_getelement(dir, key);
  if (NULL == dirElem)
    return NULL;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_dictionary_element_getvalue(dirElem);
}
