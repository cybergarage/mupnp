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

#ifndef _MUPNP_UTIL_DICTIONARY_H_
#define _MUPNP_UTIL_DICTIONARY_H_

#include <mupnp/typedef.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
 * Define
 ****************************************/

/****************************************
 * Data Type
 ****************************************/

/**
 * \brief The generic wrapper struct for mUPnP's dictionarys.
 *
 * This wrapper has been created to enable 100% code
 * compatibility between different platforms (Linux, Win32 etc..)
 */
typedef struct _mUpnpDictionary {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* key;
  mUpnpString* value;
} mUpnpDictionaryElement, mUpnpDictionary;

/****************************************
* Function
****************************************/

/**
 * Create a new dictionary
 */
mUpnpDictionaryElement* mupnp_dictionary_element_new();

/**
 * Destroy a dictionary.
 *
 * \param dirElem Dictionary to destroy
 */
bool mupnp_dictionary_element_delete(mUpnpDictionaryElement* dirElem);

/**
 * Set a key
 *
 * \param dirElem Dictionary struct
 * \param name String to set 
 */
#define mupnp_dictionary_element_setkey(dirElem, name) mupnp_string_setvalue(dirElem->key, name)
/**
 * Get a key
 *
 * \param dirElem Dictionary struct
 */
#define mupnp_dictionary_element_getkey(dirElem) mupnp_string_getvalue(dirElem->key)

/**
 * Set a value
 *
 * \param dirElem Dictionary struct
 * \param value String to set
 */
#define mupnp_dictionary_element_setvalue(dirElem, val) mupnp_string_setvalue(dirElem->value, val)
/**
 * Get a value
 *
 * \param dirElem Dictionary struct
 */
#define mupnp_dictionary_element_getvalue(dirElem) mupnp_string_getvalue(dirElem->value)

#define mupnp_dictionary_element_next(dirElem) (mUpnpDictionaryElement*) mupnp_list_next((mUpnpList*)dirElem)
#define mupnp_dictionary_element_remove(dirElem) mupnp_list_remove((mUpnpList*)dirElem)

/****************************************
* Function (Dictionary List)
****************************************/

/**
 * Create a new dictionary list
 *
 * \return Dictionary list
 */
mUpnpDictionary* mupnp_dictionary_new();

/**
 * Destroy a dictionary list
 *
 * \param dir The dictionary list in question
 */
void mupnp_dictionary_delete(mUpnpDictionary* dir);

/**
 * Clear the contents of a dictionary list
 *
 * \param dir Dictionary list in question
 */
#define mupnp_dictionary_clear(dir) mupnp_list_clear((mUpnpList*)dir, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_dictionary_element_delete)

/**
 * Get the size of a dictionary list
 *
 * \param dir The dictionary list in question
 */
#define mupnp_dictionary_size(dir) mupnp_list_size((mUpnpList*)dir)

/**
 * Get the first actual item from a dictionary list to use as an iterator
 *
 * \param dir The dictionary list in question
 */
#define mupnp_dictionary_gets(dir) (mUpnpDictionaryElement*) mupnp_list_next((mUpnpList*)dir)

/**
 * Add a dictionary into a dictionary list
 *
 * \param dir The dictionary list in question
 * \param dirElem The dictionary to add to the list
 */
#define mupnp_dictionary_add(dir, dirElem) mupnp_list_add((mUpnpList*)dir, (mUpnpList*)dirElem)

/**
 * Remove a dictionary from dictionary list
 *
 * \param dirElem The dictionary to be removed 
 */
#define mupnp_dictionary_remove(dirElem) mupnp_list_remove((mUpnpList*)dirElem)

/**
 * Get a element of the specified key
 *
 * \param dir The dictionary list in question
 * \param key Name to search.
 * \return Element of the specified key.
 */
mUpnpDictionaryElement* mupnp_dictionary_get(mUpnpDictionary* dir, const char* key);

/**
 * Set a new element into a dictionary 
 *
 * \param dir The dictionary list in question
 * \param key Name to set.
 * \param value Value to set.
 */
void mupnp_dictionary_setvalue(mUpnpDictionary* dir, const char* key, const char* value);

/**
 * Get a value of the specified key
 *
 * \param dir The dictionary list in question
 * \param key Name to search.
 * \return Value of the specified key.
 */
const char* mupnp_dictionary_getvalue(mUpnpDictionary* dir, const char* key);

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
