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

#ifndef _MUPNP_ARGUMENT_H_
#define _MUPNP_ARGUMENT_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/** Definition for argument element name */
#define MUPNP_ARGUMENT_ELEM_NAME "argument"

/** Definition for argument list element name */
#define MUPNP_ARGUMENTLIST_ELEM_NAME "argumentList"

/** Definition for argument passing direction (in) */
#define MUPNP_ARGUMENT_IN_DIR "in"

/** Definition for argument passing direction (out) */
#define MUPNP_ARGUMENT_OUT_DIR "out"

/** Definition for argument element name */
#define MUPNP_ARGUMENT_NAME "name"

/** Definition for argument element "direction" */
#define MUPNP_ARGUMENT_DIRECTION "direction"

/** Definition for argument related state variable */
#define MUPNP_ARGUMENT_RELATED_STATE_VARIABLE "relatedStateVariable"

/****************************************
* Data Type
****************************************/

/**
 * Data type definition for UPnP argument and UPnP argument list
 */
typedef struct _mUpnpArgument {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpXmlNode* serviceNode;
  mUpnpXmlNode* argumentNode;
  /**** Execution Data ****/
  mUpnpString* value;
} mUpnpArgument, mUpnpArgumentList;

/****************************************
* Function (Argument)
****************************************/

/**
 * Create new argument
 */
mUpnpArgument* mupnp_argument_new();

/**
 * Delete argument
 *
 * @param dev Argument
 */
void mupnp_argument_delete(mUpnpArgument* dev);

/**
 * Get next argument using argument as iterator
 *
 * @param arg Argument node
 */
#define mupnp_argument_next(arg) (mUpnpArgument*) mupnp_list_next((mUpnpList*)arg)

/**
 * Check if XML node is argument node
 *
 * @param node XML node
 *
 * @return True if node is argument node, false otherwise
 */
#define mupnp_argument_isargumentnode(node) mupnp_xml_node_isname(node, MUPNP_ARGUMENT_ELEM_NAME)

/**
 * Set XML service node for argument
 *
 * @param arg Argument
 * @param node XML service node
 */
#define mupnp_argument_setservicenode(arg, node) (arg->serviceNode = node)

/**
 * Get service node from argument
 *
 * @param arg Argument
 */
#define mupnp_argument_getservicenode(arg) (arg->serviceNode)

/**
 * Set XML argument node for argument
 *
 * @param arg Argument
 * @param node XML argument node
 */
#define mupnp_argument_setargumentnode(arg, node) (arg->argumentNode = node)

/**
 * Get XML argument node from argument
 *
 * @param arg Argument
 */
#define mupnp_argument_getargumentnode(arg) (arg->argumentNode)

/**** name ****/

/**
 * Set name for argument
 *
 * @param arg Argument
 * @param value Argument name
 */
#define mupnp_argument_setname(arg, value) mupnp_xml_node_setchildnode(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_NAME, value)

/**
 * Get argument name
 *
 * @param arg Argument
 */
#define mupnp_argument_getname(arg) mupnp_xml_node_getchildnodevalue(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_NAME)

/**
 * Check if current argument name matches with the provided name
 *
 * @param arg Argument
 * @param name Name matched with argument name
 *
 * @return True if argument name equals with the provided name, false otherwise
 */
#define mupnp_argument_isname(arg, name) mupnp_streq(mupnp_argument_getname(arg), name)

/**** direction ****/

/**
 * Set direction for argument
 *
 * @param arg Argument
 * @param value New direction value
 */
#define mupnp_argument_setdirection(arg, value) mupnp_xml_node_setchildnode(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_DIRECTION, value)

/**
 * Get argument direction
 * 
 * @param arg Argument
 *
 * @return Argument direction
 */
#define mupnp_argument_getdirection(arg) mupnp_xml_node_getchildnodevalue(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_DIRECTION)

/**
 * Check if argument direction is "in"
 *
 * @param arg Argument
 *
 * @return True if argument directon is "in"
 */
#define mupnp_argument_isindirection(arg) mupnp_streq(mupnp_argument_getdirection(arg), MUPNP_ARGUMENT_IN_DIR)

/**
 * Check if argument direction is "out"
 *
 * @param arg Argument
 *
 * @return True if argument direction is "out"
 */
#define mupnp_argument_isoutdirection(arg) mupnp_streq(mupnp_argument_getdirection(arg), MUPNP_ARGUMENT_OUT_DIR)

/**** relatedStateVariable ****/

/**
 * Set related state variable
 *
 * @param arg Argument
 * @param value Related state variable name
 */
#define mupnp_argument_setrelatedstatevariable(arg, value) mupnp_xml_node_setchildnode(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_RELATED_STATE_VARIABLE, value)

/**
 * Get related state variable
 *
 * @param arg Argument
 *
 * @return String representing the related state variable
 */
#define mupnp_argument_getrelatedstatevariable(arg) mupnp_xml_node_getchildnodevalue(mupnp_argument_getargumentnode(arg), MUPNP_ARGUMENT_RELATED_STATE_VARIABLE)

/****************************************
* Function (Argument - Execution Data)
****************************************/

/**** value ****/

/**
 * Set argument value
 *
 * @param arg Argument
 * @param data Argument value
 */
#define mupnp_argument_setvalue(arg, data) mupnp_string_setvalue(arg->value, data)

/**
 * Get argument value
 *
 * @param arg Argument
 *
 * @return Argument value
 */
#define mupnp_argument_getvalue(arg) mupnp_string_getvalue(arg->value)

/**
 * Set argument value
 *
 * @param arg Argument
 * @param data Argument value
 */
#define mupnp_argument_setintvalue(arg, data) mupnp_string_setintvalue(arg->value, data)

/**
 * Get argument value
 *
 * @param arg Argument
 *
 * @return Argument value
 */
#define mupnp_argument_getintvalue(arg) mupnp_string_getintvalue(arg->value)

/**
 * Set argument value
 *
 * @param arg Argument
 * @param data Argument value
 */
#define mupnp_argument_setfloatvalue(arg, data) mupnp_string_setfloatvalue(arg->value, data)

/**
 * Get argument value
 *
 * @param arg Argument
 *
 * @return Argument value
 */
#define mupnp_argument_getfloatvalue(arg) mupnp_string_getfloatvalue(arg->value)

/**
 * Set argument value
 *
 * @param arg Argument
 * @param data Argument value
 */
#define mupnp_argument_setdoublevalue(arg, data) mupnp_string_setdoublevalue(arg->value, data)

/**
 * Get argument value
 *
 * @param arg Argument
 *
 * @return Argument value
 */
#define mupnp_argument_getdoublevalue(arg) mupnp_string_getdoublevalue(arg->value)

/****************************************
* Function (ArgumentList)
****************************************/

/**
 * Create new argument list
 */
mUpnpArgumentList* mupnp_argumentlist_new();

/**
 * Delete argument list
 *
 * @param argumentList Argument list
 */
void mupnp_argumentlist_delete(mUpnpArgumentList* argumentList);

/**
 * Clear argument list
 *
 * @param argList Argument list
 */
#define mupnp_argumentlist_clear(argList) mupnp_list_clear((mUpnpList*)argList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_argument_delete)

/**
 * Get argument list size
 *
 * @param argList Argument list
 */
#define mupnp_argumentlist_size(argList) mupnp_list_size((mUpnpList*)argList)

/**
 * Get next argument from argument list
 *
 * @param argList Argument list
 */
#define mupnp_argumentlist_gets(argList) (mUpnpArgument*) mupnp_list_next((mUpnpList*)argList)

/**
 * Add argument into argument list
 *
 * @param argList Argument list
 * @param arg Argument
 */
#define mupnp_argumentlist_add(argList, arg) mupnp_list_add((mUpnpList*)argList, (mUpnpList*)arg)

/**
 * Get argument from argument list based on argument name
 *
 * @param argumentList argumentList
 * @param name Argument name
 *
 * @return Argument
 */
mUpnpArgument* mupnp_argumentlist_get(mUpnpArgumentList* argumentList, const char* name);

/**
 * Set argument values by using source argument list. If there is an argument with the 
 * name in argument list and source argument list argument data is set from source
 * argument list to the matching argument list.
 *
 * @param argumentList Argument list
 * @param srcArgumentList Source argument list
 */
void mupnp_argumentlist_set(mUpnpArgumentList* argumentList, mUpnpArgumentList* srcArgumentList);

#ifdef __cplusplus
}
#endif

#endif
