/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdictionary.h
*
*	Revision:
*
*	05/26/09
*		- first revision
*
******************************************************************/

#ifndef _CG_UTIL_CDICTIONARY_H_
#define _CG_UTIL_CDICTIONARY_H_

#include <mupnp/typedef.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
 * Define
 ****************************************/

/****************************************
 * Data Type
 ****************************************/

/**
 * \brief The generic wrapper struct for CyberLinkC's dictionarys.
 *
 * This wrapper has been created to enable 100% code
 * compatibility between different platforms (Linux, Win32 etc..)
 */
typedef struct _CgDictionary {
	BOOL headFlag;
	struct _CgDictionary *prev;
	struct _CgDictionary *next;
	CgString *key;
	CgString *value;
} CgDictionaryElement, CgDictionary;

/****************************************
* Function
****************************************/

/**
 * Create a new dictionary
 */
CgDictionaryElement *mupnp_dictionary_element_new();

/**
 * Destroy a dictionary.
 *
 * \param dirElem Dictionary to destroy
 */
BOOL mupnp_dictionary_element_delete(CgDictionaryElement *dirElem);

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

#define mupnp_dictionary_element_next(dirElem) (CgDictionaryElement *)mupnp_list_next((CgList *)dirElem)
#define mupnp_dictionary_element_remove(dirElem) mupnp_list_remove((CgList *)dirElem)

/****************************************
* Function (Dictionary List)
****************************************/

/**
 * Create a new dictionary list
 *
 * \return Dictionary list
 */
CgDictionary *mupnp_dictionary_new();

/**
 * Destroy a dictionary list
 *
 * \param dir The dictionary list in question
 */
void mupnp_dictionary_delete(CgDictionary *dir);

/**
 * Clear the contents of a dictionary list
 *
 * \param dir Dictionary list in question
 */
#define mupnp_dictionary_clear(dir) mupnp_list_clear((CgList *)dir, (CG_LIST_DESTRUCTORFUNC)mupnp_dictionary_element_delete)

/**
 * Get the size of a dictionary list
 *
 * \param dir The dictionary list in question
 */
#define mupnp_dictionary_size(dir) mupnp_list_size((CgList *)dir)

/**
 * Get the first actual item from a dictionary list to use as an iterator
 *
 * \param dir The dictionary list in question
 */
#define mupnp_dictionary_gets(dir) (CgDictionaryElement *)mupnp_list_next((CgList *)dir)

/**
 * Add a dictionary into a dictionary list
 *
 * \param dir The dictionary list in question
 * \param dirElem The dictionary to add to the list
 */
#define mupnp_dictionary_add(dir, dirElem) mupnp_list_add((CgList *)dir, (CgList *)dirElem)

/**
 * Remove a dictionary from dictionary list
 *
 * \param dirElem The dictionary to be removed 
 */
#define mupnp_dictionary_remove(dirElem) mupnp_list_remove((CgList *)dirElem)

/**
 * Get a element of the specified key
 *
 * \param dir The dictionary list in question
 * \param key Name to search.
 * \return Element of the specified key.
 */
CgDictionaryElement *mupnp_dictionary_get(CgDictionary *dir, const char *key);

/**
 * Set a new element into a dictionary 
 *
 * \param dir The dictionary list in question
 * \param key Name to set.
 * \param value Value to set.
 */
void mupnp_dictionary_setvalue(CgDictionary *dir, const char *key, const char *value);

/**
 * Get a value of the specified key
 *
 * \param dir The dictionary list in question
 * \param key Name to search.
 * \return Value of the specified key.
 */
const char *mupnp_dictionary_getvalue(CgDictionary *dir, const char *key);

#ifdef  __cplusplus

} /* extern "C" */

#endif

#endif
