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

#ifndef _MUPNP_STATEVARIABLE_H_
#define _MUPNP_STATEVARIABLE_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>
#include <mupnp/util/string.h>

#include <mupnp/upnp_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/**
 * Definition for state variable XML element name
 */
#define MUPNP_STATEVARIABLE_ELEM_NAME "stateVariable"

/**
 * Definition for service state table XML element name
 */
#define MUPNP_SERVICESTATETALBE_ELEM_NAME "serviceStateTable"

/**
 * Definition for state variable name element
 */
#define MUPNP_STATEVARIABLE_NAME "name"

/**
 * Definition for state variable data type element
 */
#define MUPNP_STATEVARIABLE_DATATYPE "dataType"

/**
 * Definition for state variable send events element name
 */
#define MUPNP_STATEVARIABLE_SENDEVENTS "sendEvents"

/**
 * Definition for state variable send events element value
 */
#define MUPNP_STATEVARIABLE_SENDEVENTS_YES "yes"

/**
 * Definition for state variable send events element value
 */
#define MUPNP_STATEVARIABLE_SENDEVENTS_NO "no"

/**
 * Definition for state variable allowed values list
 */
#define MUPNP_STATEVARIABLE_ALLOWEDVALUELIST "allowedValueList"

/****************************************
* Data Type
****************************************/

/**
  * Data type for allowed state variables allowed values table
  */
typedef struct _mUpnpAllowedValuesList {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpString* value;
} mUpnpAllowedValuesList, mUpnpAllowedValue;

/**
 * Data type for state variable and state table
 */
typedef struct _mUpnpStateVariable {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpAllowedValuesList* allowedValueList;
  //tb: fixmelater verify type mUpnpService* ???
  void* parentService;
  mUpnpXmlNode* stateVarNode;
  /* Dummy pointers to maintain ABI, Please remove during the
	 * appropriate development stage */
  void* dummy1;
  void* dummy2;
  /**** Execution Data ****/
  mUpnpString* value;
  bool (*listener)(struct _mUpnpStateVariable*);
  mUpnpStatus* upnpStatus;
  void* userData;

} mUpnpStateVariable, mUpnpServiceStateTable;

/**
 * Type definition for state variable listener function
 */
typedef bool (*MUPNP_STATEVARIABLE_LISTNER)(mUpnpStateVariable*);

/****************************************
* Function (StateVariable)
****************************************/

/**
 * Create new state variable object
 */
mUpnpStateVariable* mupnp_statevariable_new();

/**
 * Delete state variable object
 *
 * @param statVar state variable
 */
void mupnp_statevariable_delete(mUpnpStateVariable* statVar);

/**
 * Get next element from state variable list
 *
 * @param statVar state variable list node
 */
#define mupnp_statevariable_next(statVar) (mUpnpStateVariable*) mupnp_list_next((mUpnpList*)statVar)

/**
 * Check if XML node is state variable node
 *
 * @param node XML node
 */
#define mupnp_statevariable_isstatevariablenode(node) mupnp_xml_node_isname(node, MUPNP_STATEVARIABLE_ELEM_NAME)

/**** XML Node ****/

/**
 * Set XML node for this state variable
 *
 * @param statVar State variable
 * @param node XML node
 */
void mupnp_statevariable_setstatevariablenode(mUpnpStateVariable* statVar, mUpnpXmlNode* node);

/**
 * Get XML node from this state variable
 *
 * @param statVar State variable
 */
#define mupnp_statevariable_getstatevariablenode(statVar) (statVar->stateVarNode)

/**** Service ****/

/**
 * Set service for this state variable
 *
 * @param statVar State variable
 * @param service Service
 */
#define mupnp_statevariable_setservice(statVar, service) (statVar->parentService = service)

/**
 * Get service from state variable
 *
 * @param statVar State variable
 *
 * @return UPnP service
 */
#define mupnp_statevariable_getservice(statVar) ((mUpnpService*)statVar->parentService)

/**** allowedValueList ****/

/**
 * Get allowed value list
 *
 * @param statVar State variable
 *
 * @return Allowed value list
 */
mUpnpAllowedValuesList* mupnp_statevariable_getallowedvaluelist(mUpnpStateVariable* statVar);

/**
 * Check if state variable has allowed value list
 *
 * @param statVar State variable
 *
 * @return 0 if it does not have allowed value list integer greater than zero otherwise
 */
#define mupnp_statevariable_hasallowedvaluelist(statVar) mupnp_allowedvaluelist_size(statVar->allowedValueList)

/**
 * Check if state variable allowes to receive value
 *
 * @parap value value to be checked
 * @param statVar State variable
 *
 * @return 0 if it does allowed to receive value
*/
int mupnp_statevariable_is_allowed_value(mUpnpStateVariable* statVar, const char* value);

/**
 * Create new allowed state variable values list
 */
mUpnpAllowedValuesList* mupnp_allowedvaluelist_new();

/**** allowedValueRange ****/

/**
 * Get allowed value range for state variable
 *
 * @param statVar State variable
 *
 * @return Allowed value range
 */
#define mupnp_statevariable_getallowedvaluerange(statVar) (statVar->allowedValueRange)

/**
 * Checks if state variable has allowed variable range
 *
 * @param statVar State variable
 *
 * @return True if allowed value range exists, false otherwise
 */
#define mupnp_statevariable_hasallowedvaluerange(statVar) ((statVar->allowedValueRange != NULL) ? true : false)

/**** name ****/

/**
 * Set state variable name
 *
 * @param statVar State variable
 * @param value New state variable name
 */
#define mupnp_statevariable_setname(statVar, value) mupnp_xml_node_setchildnode(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_NAME, value)

/**
 * Get state variable name
 *
 * @param statVar State variable
 *
 * @return State variable name
 */
#define mupnp_statevariable_getname(statVar) mupnp_xml_node_getchildnodevalue(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_NAME)

/**
 * Checks if state variable has certain name
 *
 * @param statVar State variable
 * @param name Name to be checked
 *
 * @return True if State variable name is same as provided, false otherwise
 */
#define mupnp_statevariable_isname(statVar, name) mupnp_streq(mupnp_statevariable_getname(statVar), name)

/**** dataType ****/

/**
 * Set data type for state variable
 *
 * @param statVar State variable
 * @param value Data type
 */
#define mupnp_statevariable_setdatatype(statVar, value) mupnp_xml_node_setchildnode(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_DATATYPE, value)

/**
 * Get data type from state variable
 *
 * @param statVar State variable
 *
 * @return Data type
 */
#define mupnp_statevariable_getdatatype(statVar) mupnp_xml_node_getchildnodevalue(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_DATATYPE)

/**** sendEvents ****/

/**
 * Set send events option for state variable, can be used to disable eventing 
 * for certain variables.
 *
 * @param statVar State variable
 * @param value New value
 */
#define mupnp_statevariable_setsendevents(statVar, value) mupnp_xml_node_setattribute(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_SENDEVENTS, value)

/**
 * Get state variable event sending option value
 *
 * @param statVar State variable
 * 
 * @return Current value associated to state variable
 */
#define mupnp_statevariable_getsendevents(statVar) mupnp_xml_node_getattributevalue(mupnp_statevariable_getstatevariablenode(statVar), MUPNP_STATEVARIABLE_SENDEVENTS)

/**
 * Check if events are sent to subscribers when this state variable changes
 *
 * @param statVar State variable
 *
 * @return True if events are sent, false otherwise
 */
#define mupnp_statevariable_issendevents(statVar) (mupnp_streq(mupnp_statevariable_getsendevents(statVar), MUPNP_STATEVARIABLE_SENDEVENTS_YES))

/****************************************
* Function (StateVariable - Execution Data)
****************************************/

/**** value ****/

/**
 * Set state variable value
 *
 * @param statVar State variable
 * @param data New value associated with this state variable, and the value is send to other devices which subscribe the service.
 */
void mupnp_statevariable_setvalue(mUpnpStateVariable* statVar, const char* data);

/**
 * Set state variable value
 *
 * @param statVar State variable
 * @param data New value associated with this state variable
 */
void mupnp_statevariable_setvaluewithoutnotify(mUpnpStateVariable* statVar, const char* data);

/**
 * Get value associated with this state variable
 *
 * @param statVar State variable
 *
 * @return Value associated with this state variable
 */
#define mupnp_statevariable_getvalue(statVar) mupnp_string_getvalue(statVar->value)

/*** listener ****/

/**
 * Set listener callback for this state variable
 *
 * @param statVar State variable
 * @param func Listener function
 */
#define mupnp_statevariable_setlistener(statVar, func) (statVar->listener = func)

/**
 * Get lister function associated with state variable
 *
 * @param statVar State variable
 */
#define mupnp_statevariable_getlistener(statVar) (statVar->listener)

/**** UPnPStatus ****/

/**
 * Set status code for state variable
 *
 * @param statVar State variable
 * @param code Status code
 */
#define mupnp_statevariable_setstatuscode(statVar, code) mupnp_status_setcode(statVar->upnpStatus, code)

/** 
 * Get status code of this state variable
 *
 * @param statVar Status variable
 */
#define mupnp_statevariable_getstatuscode(statVar) mupnp_status_getcode(statVar->upnpStatus)

/**
 * Set status description for state variable
 *
 * @param statVar State variable
 * @param value Status description
 */
#define mupnp_statevariable_setstatusdescription(statVar, value) mupnp_status_setdescription(statVar->upnpStatus, value)

/**
 * Get status description from this state variable
 *
 * @param statVar State variable
 */
#define mupnp_statevariable_getstatusdescription(statVar) mupnp_status_getdescription(statVar->upnpStatus)

/**** User Data****/

/**
 * Set arbitrary user data associated with this state variable
 *
 * @param statVar State variable
 * @param value Data
 */
#define mupnp_statevariable_setuserdata(statVar, value) (statVar->userData = value)

/**
 * Get user data from this state variable
 *
 * @param statVar State variable
 *
 * @return User data
 */
#define mupnp_statevariable_getuserdata(statVar) (statVar->userData)

/****************************************
* Function (ServiceStateTable)
****************************************/

/**
 * Create service state table object
 */
mUpnpServiceStateTable* mupnp_servicestatetable_new();

/**
 * Delete service state table object
 *
 * @param servicestatetable Service state table
 */
void mupnp_servicestatetable_delete(mUpnpServiceStateTable* servicestatetable);

/**
 * Clear service state table
 *
 * @param stateTable Service state table
 */
#define mupnp_servicestatetable_clear(stateTable) mupnp_list_clear((mUpnpList*)stateTable, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_statevariable_delete)

/**
 * Get size of service state table
 *
 * @param stateTable Service state table
 */
#define mupnp_servicestatetable_size(stateTable) mupnp_list_size((mUpnpList*)stateTable)

/**
 * Get next service state table from service state table list
 *
 * @param stateTable Service state table
 *
 * @return Service state table (node)
 */
#define mupnp_servicestatetable_gets(stateTable) (mUpnpStateVariable*) mupnp_list_next((mUpnpList*)stateTable)

/**
 * Add new service state table into service state table list
 *
 * @param stateTable Service state table
 * @param statVar State variable
 */
#define mupnp_servicestatetable_add(stateTable, statVar) mupnp_list_add((mUpnpList*)stateTable, (mUpnpList*)statVar)

#ifdef __cplusplus
}
#endif

#endif
