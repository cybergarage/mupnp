/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontentdir_service.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/upnp/std/av/cmediaserver.h>

/****************************************
* Service Description (Content Directory)
****************************************/

static char *CG_UPNP_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"   <specVersion>\n"
"      <major>1</major>\n"
"      <minor>0</minor>\n"
"   </specVersion>\n"
"   <actionList>\n"
"      <action>\n"
"         <name>ExportResource</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>SourceURI</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>DestinationURI</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TransferID</name>\n"
"               <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>StopTransferResource</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>TransferID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>DestroyObject</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ObjectID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>DeleteResource</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ResourceURI</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>UpdateObject</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ObjectID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>CurrentTagValue</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TagValueList</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NewTagValue</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TagValueList</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>Browse</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>ObjectID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>BrowseFlag</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Filter</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>StartingIndex</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>RequestedCount</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>SortCriteria</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>Result</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>NumberReturned</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TotalMatches</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>UpdateID</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>\n"
"            </argument>\n"
"         </argumentList>\n"
"      </action>\n"
"      <action>\n"
"         <name>GetTransferProgress</name>\n"
"         <argumentList>\n"
"            <argument>\n"
"               <name>TransferID</name>\n"
"               <direction>in</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TransferStatus</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TransferStatus</relatedStateVariable>\n"
"            </argument>\n"
"            <argument>\n"
"               <name>TransferLength</name>\n"
"               <direction>out</direction>\n"
"               <relatedStateVariable>A_ARG_TYPE_TransferLength</relatedStateVariable>\n"
"            </argument>\n"
"             <argument>\n"
"                <name>TransferTotal</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_TransferTotal</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>GetSearchCapabilities</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>SearchCaps</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>SearchCapabilities</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>CreateObject</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>ContainerID</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Elements</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>ObjectID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Result</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>Search</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>ContainerID</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>SearchCriteria</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_SearchCriteria</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Filter</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>StartingIndex</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>RequestedCount</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>SortCriteria</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>Result</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>NumberReturned</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>TotalMatches</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>UpdateID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>GetSortCapabilities</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>SortCaps</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>SortCapabilities</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>ImportResource</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>SourceURI</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>DestinationURI</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_URI</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>TransferID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_TransferID</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>CreateReference</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>ContainerID</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>ObjectID</name>\n"
"                <direction>in</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"             <argument>\n"
"                <name>NewID</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"       <action>\n"
"          <name>GetSystemUpdateID</name>\n"
"          <argumentList>\n"
"             <argument>\n"
"                <name>Id</name>\n"
"                <direction>out</direction>\n"
"                <relatedStateVariable>SystemUpdateID</relatedStateVariable>\n"
"             </argument>\n"
"          </argumentList>\n"
"       </action>\n"
"    </actionList>\n"
"    <serviceStateTable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_SortCriteria</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_TransferLength</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>TransferIDs</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_UpdateID</name>\n"
"          <dataType>ui4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_SearchCriteria</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_Filter</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>ContainerUpdateIDs</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_Result</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_Index</name>\n"
"          <dataType>ui4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_TransferID</name>\n"
"          <dataType>ui4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_TagValueList</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_URI</name>\n"
"          <dataType>uri</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_ObjectID</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>SortCapabilities</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>SearchCapabilities</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_Count</name>\n"
"          <dataType>ui4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_BrowseFlag</name>\n"
"          <dataType>string</dataType>\n"
"          <allowedValueList>\n"
"             <allowedValue>BrowseMetadata</allowedValue>\n"
"             <allowedValue>BrowseDirectChildren</allowedValue>\n"
"          </allowedValueList>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"yes\">\n"
"          <name>SystemUpdateID</name>\n"
"          <dataType>ui4</dataType>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_TransferStatus</name>\n"
"          <dataType>string</dataType>\n"
"          <allowedValueList>\n"
"             <allowedValue>COMPLETED</allowedValue>\n"
"             <allowedValue>ERROR</allowedValue>\n"
"             <allowedValue>IN_PROGRESS</allowedValue>\n"
"             <allowedValue>STOPPED</allowedValue>\n"
"          </allowedValueList>\n"
"       </stateVariable>\n"
"       <stateVariable sendEvents=\"no\">\n"
"          <name>A_ARG_TYPE_TransferTotal</name>\n"
"          <dataType>string</dataType>\n"
"       </stateVariable>\n"
"    </serviceStateTable>\n"
" </scpd>\n";

/****************************************
* cg_upnp_dms_condir_setsystemupdateid
****************************************/

void cg_upnp_dms_condir_setsystemupdateid(CgUpnpMediaServer *dms, int id)
{
	CgUpnpDevice *dev;
	CgUpnpStateVariable *var;
	char intBuf[CG_STRING_INTEGER_BUFLEN];

	dev = cg_upnp_dms_getdevice(dms);
	if (!dev)
		return;

	var = cg_upnp_device_getstatevariablebyname(dev, CG_UPNP_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID);
	if (!var)
		return;

	cg_upnp_statevariable_setvalue(var, cg_int2str(id, intBuf, CG_STRING_INTEGER_BUFLEN));
}

/****************************************
* cg_upnp_dms_condir_getsystemupdateid
****************************************/

int cg_upnp_dms_condir_getsystemupdateid(CgUpnpMediaServer *dms)
{
	CgUpnpDevice *dev;
	CgUpnpStateVariable *var;

	dev = cg_upnp_dms_getdevice(dms);
	if (!dev)
		return 0;

	var = cg_upnp_device_getstatevariablebyname(dev, CG_UPNP_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID);
	if (!var)
		return 0;

	return cg_str2int(cg_upnp_statevariable_getvalue(var));
}

/****************************************
* cg_upnp_dms_condir_updatesystemupdateid
****************************************/

void cg_upnp_dms_condir_updatesystemupdateid(CgUpnpMediaServer *dms)
{
	cg_upnp_dms_condir_setsystemupdateid(dms, (cg_upnp_dms_condir_getsystemupdateid(dms) + 1));
}

/****************************************
* cg_upnp_dms_condir_browsemetadata
****************************************/

static BOOL cg_upnp_dms_condir_browsemetadata(CgUpnpMediaServer *dms, CgUpnpAction *action)
{
	char *objectID;
	CgUpnpMediaContent *objectContent;
	CgXmlNode *didlNode;
	CgString *resultStr;
	char intBuf[CG_STRING_INTEGER_BUFLEN];
	
	objectID = cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
	if (cg_strlen(objectID) <= 0)
		return FALSE;

	objectContent = cg_upnp_dms_findcontentbyid(dms, objectID);
	if (!objectContent)
		return FALSE;

	didlNode = cg_upnp_media_didl_node_new();
	cg_upnp_media_contentlist_add(didlNode, objectContent);

	resultStr = cg_string_new();
	cg_upnp_media_didl_node_tostring(didlNode, resultStr);

	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_RESULT, cg_string_getvalue(resultStr));
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED, "1");
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES, "1");
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID, cg_int2str(cg_upnp_dms_condir_getsystemupdateid(dms), intBuf, sizeof(intBuf)));

	cg_string_delete(resultStr);
	
	cg_upnp_media_didl_node_delete(didlNode);

	return TRUE;
}

/****************************************
* cg_upnp_dms_condir_browsedirectchildren
****************************************/

static BOOL cg_upnp_dms_condir_browsedirectchildren(CgUpnpMediaServer *dms, CgUpnpAction *action)
{
	char *objectID;
	CgUpnpMediaContent *objectContent;
	CgUpnpMediaContent *content;
	CgUpnpMediaContent *copyContent;
	CgUpnpMediaContentList *sortedContentList;
	CgUpnpMediaContent **sortedContentArray;
	int startingIndex;
	int requestedCount;
	int numberReturned;
	int totalMachesCnt;
	char intBuf[CG_STRING_INTEGER_BUFLEN];
	int n;
	CgXmlNode *didlNode;
	CgString *resultStr;

	objectID = cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
	if (cg_strlen(objectID) <= 0)
		return FALSE;

	objectContent = cg_upnp_dms_findcontentbyid(dms, objectID);
	if (!objectContent)
		return FALSE;

	totalMachesCnt = 0;
	sortedContentList = cg_upnp_media_contentlist_new();
	for (content=cg_upnp_media_content_getchildcontents(objectContent); content; content=cg_upnp_media_content_next(content)) {
		copyContent = cg_upnp_media_content_new();
		cg_upnp_media_content_copy(copyContent, content);
		cg_upnp_media_contentlist_add(sortedContentList, copyContent);
		totalMachesCnt++;
	}

	/* Not Implemented 
	// Sort Content Node Lists
	string sortCriteria = action->getSortCriteria();
	ContentNodeList sortedContentNodeBufList(false);
	ContentNodeList *sortedContentNodeList = sortContentNodeList(&contentNodeList, sortCriteria.c_str(), sortedContentNodeBufList);
	*/

	startingIndex = cg_str2int(cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_STARTING_INDEX));
	if (startingIndex <= 0)
		startingIndex = 0;

	requestedCount = cg_str2int(cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_REQUESTED_COUNT));
	if (requestedCount <= 0)
		requestedCount = cg_upnp_media_contentlist_size(sortedContentList);

	numberReturned = 0;

	didlNode = cg_upnp_media_didl_node_new();

	sortedContentArray = (CgUpnpMediaContent **)malloc(sizeof(CgUpnpMediaContent*) * totalMachesCnt);
	n = 0;
	for (content=cg_upnp_media_contentlist_gets(sortedContentList); content; content=cg_upnp_media_content_next(content)) {
		sortedContentArray[n] = content;
		n++;
	}

	for (n=startingIndex; (n<totalMachesCnt && numberReturned<requestedCount); n++) {
		content = sortedContentArray[n];
		cg_upnp_media_content_remove(content);
		cg_upnp_media_content_addchildcontent(didlNode, content);
		cg_upnp_media_content_setparentid(content, objectID);
		numberReturned++;
	}

	resultStr = cg_string_new();
	cg_upnp_media_didl_node_tostring(didlNode, resultStr);

	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_RESULT, cg_string_getvalue(resultStr));
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED, cg_int2str(numberReturned, intBuf, sizeof(intBuf)));
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES, cg_int2str(totalMachesCnt, intBuf, sizeof(intBuf)));
	cg_upnp_action_setargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID, cg_int2str(cg_upnp_dms_condir_getsystemupdateid(dms), intBuf, sizeof(intBuf)));

	cg_string_delete(resultStr);
	free(sortedContentArray);
	cg_upnp_media_didl_node_delete(didlNode);
	cg_upnp_media_contentlist_delete(sortedContentList);

	return TRUE;
}

/****************************************
* cg_upnp_dms_condir_actionreceived
****************************************/

BOOL cg_upnp_dms_condir_actionreceived(CgUpnpAction *action)
{
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *actionName;
	CgUpnpArgument *arg;
	char *argValue;
	char intBuf[CG_STRING_INTEGER_BUFLEN];

	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
	if (!dev) 
		return FALSE;

	dms = (CgUpnpMediaServer *)cg_upnp_device_getuserdata(dev);
	if (!dms)
		return FALSE;

	/* Browse */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE))
	{
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_FLAG);
		if (!arg)
			return FALSE;
		argValue = cg_upnp_argument_getvalue(arg);
		if (cg_streq(argValue, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_METADATA))
			return cg_upnp_dms_condir_browsemetadata(dms, action);
		if (cg_streq(argValue, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_DIRECT_CHILDREN))
			return cg_upnp_dms_condir_browsedirectchildren(dms, action);
		return FALSE;
	}

	/* Search */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH)) {
		/* Not Implemented */
		return FALSE;
	}

	/* Sort Capabilities */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_GET_SORT_CAPABILITIES)) {
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_SORT_CAPS);
		if (!arg)
			return FALSE;
		cg_upnp_argument_setvalue(arg, "");
		return TRUE;
	}

	/* Search Capabilities */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_GET_SEARCH_CAPABILITIES)) {
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH_CAPS);
		if (!arg)
			return FALSE;
		cg_upnp_argument_setvalue(arg, "");
		return TRUE;
	}

	/* System Update ID */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_GET_SYSTEM_UPDATE_ID)) {
		arg = cg_upnp_action_getargumentbyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_ID);
		if (!arg)
			return FALSE;
		cg_upnp_argument_setvalue(arg, cg_int2str(cg_upnp_dms_condir_getsystemupdateid(dms), intBuf, CG_STRING_INTEGER_BUFLEN));
		return TRUE;
	}

	return FALSE;
}

/****************************************
* cg_upnp_dms_condir_actionreceived
****************************************/


BOOL cg_upnp_dms_condir_queryreceived(CgUpnpStateVariable *var)
{
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *varName;
	char intBuf[CG_STRING_INTEGER_BUFLEN];

	varName = cg_upnp_statevariable_getname(var);
	if (cg_strlen(varName) <= 0)
		return FALSE;

	dev = (CgUpnpDevice *)cg_upnp_service_getdevice(cg_upnp_statevariable_getservice(var));
	if (!dev) 
		return FALSE;

	dms = (CgUpnpMediaServer *)cg_upnp_device_getuserdata(dev);
	if (!dms)
		return FALSE;

	/* Sort Capabilities */
	if (cg_streq(varName, CG_UPNP_DMS_CONTENTDIRECTORY_SORT_CAPABILITIES)) {
		cg_upnp_statevariable_setvalue(var, "");
		return TRUE;
	}

	/* Search Capabilities */
	if (cg_streq(varName, CG_UPNP_DMS_CONTENTDIRECTORY_SEARCH_CAPABILITIES)) {
		cg_upnp_statevariable_setvalue(var, "");
		return TRUE;
	}

	/* System Update ID */
	if (cg_streq(varName, CG_UPNP_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID)) {
		cg_upnp_statevariable_setvalue(var, cg_int2str(cg_upnp_dms_condir_getsystemupdateid(dms), intBuf, CG_STRING_INTEGER_BUFLEN));
		return TRUE;
	}

	return FALSE;
}

/****************************************
* cg_upnp_dms_condir_init
****************************************/

BOOL cg_upnp_dms_condir_init(CgUpnpMediaServer *dms)
{
	CgUpnpDevice *dev;
	CgUpnpService *service;
	CgUpnpAction* action;

	dev = cg_upnp_dms_getdevice(dms);
	if (!dev)
		return FALSE;

	service = cg_upnp_device_getservicebytype(dev, CG_UPNP_DMS_CONTENTDIRECTORY_SERVICE_TYPE);
	if (!service)
		return FALSE;
	
	if (cg_upnp_service_parsedescription(service, CG_UPNP_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION, cg_strlen(CG_UPNP_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION)) == FALSE)
		return FALSE;
	
	cg_upnp_service_setactionlistener(service, cg_upnp_dms_condir_actionreceived);
	for (action=cg_upnp_service_getactions(service); action; action=cg_upnp_action_next(action))
		cg_upnp_action_setuserdata(action, dms);

	cg_upnp_dms_condir_setsystemupdateid(dms, 1);
	

	return TRUE;
}
