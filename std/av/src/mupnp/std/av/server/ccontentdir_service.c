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

#include <mupnp/std/av/cmediaserver.h>

/****************************************
* Service Description (Content Directory)
****************************************/

static char *CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION =
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
/*
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
*/
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

// Descriptor for xbox360 required service
static char *CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_DESCRIPTION =
"<?xml version=\"1.0\" ?>\n"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
"    <specVersion>\n"
"        <major>1</major>\n"
"        <minor>0</minor>\n"
"    </specVersion>\n"
"    <actionList>\n"
"        <action>\n"
"            <name>IsAuthorized</name>\n"
"            <argumentList>\n"
"                <argument>\n"
"                    <name>DeviceID</name>\n"
"                    <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_DeviceID</relatedStateVariable>\n"
"                </argument>\n"
"                <argument>\n"
"                    <name>Result</name>\n"
"                    <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"                </argument>\n"
"            </argumentList>\n"
"        </action>\n"
"        <action>\n"
"            <name>RegisterDevice</name>\n"
"            <argumentList>\n"
"                <argument>\n"
"                    <name>RegistrationReqMsg</name>\n"
"                    <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_RegistrationReqMsg</relatedStateVariable>\n"
"                </argument>\n"
"                <argument>\n"
"                    <name>RegistrationRespMsg</name>\n"
"                    <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_RegistrationRespMsg</relatedStateVariable>\n"
"                </argument>\n"
"            </argumentList>\n"
"        </action>\n"
"        <action>\n"
"            <name>IsValidated</name>\n"
"            <argumentList>\n"
"                <argument>\n"
"                    <name>DeviceID</name>\n"
"                    <direction>in</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_DeviceID</relatedStateVariable>\n"
"                </argument>\n"
"                <argument>\n"
"                    <name>Result</name>\n"
"                    <direction>out</direction>\n"
"                    <relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\n"
"                </argument>\n"
"            </argumentList>\n"
"        </action>\n"
"    </actionList>\n"
"    <serviceStateTable>\n"
"        <stateVariable sendEvents=\"no\">\n"
"            <name>A_ARG_TYPE_DeviceID</name>\n"
"            <dataType>string</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"no\">\n"
"            <name>A_ARG_TYPE_Result</name>\n"
"            <dataType>int</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"no\">\n"
"            <name>A_ARG_TYPE_RegistrationReqMsg</name>\n"
"            <dataType>bin.base64</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"no\">\n"
"            <name>A_ARG_TYPE_RegistrationRespMsg</name>\n"
"            <dataType>bin.base64</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"yes\">\n"
"            <name>AuthorizationGrantedUpdateID</name>\n"
"            <dataType>ui4</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"yes\">\n"
"            <name>AuthorizationDeniedUpdateID</name>\n"
"            <dataType>ui4</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"yes\">\n"
"            <name>ValidationSucceededUpdateID</name>\n"
"            <dataType>ui4</dataType>\n"
"        </stateVariable>\n"
"        <stateVariable sendEvents=\"yes\">\n"
"            <name>ValidationRevokedUpdateID</name>\n"
"            <dataType>ui4</dataType>\n"
"        </stateVariable>\n"
"    </serviceStateTable>\n"
"</scpd>\n";





/****************************************
* mupnp_upnpav_dms_condir_setsystemupdateid
****************************************/

void mupnp_upnpav_dms_condir_setsystemupdateid(mUpnpAvServer *dms, int id)
{
	mUpnpDevice *dev;
	mUpnpStateVariable *var;
	char intBuf[MUPNP_STRING_INTEGER_BUFLEN];

	dev = mupnp_upnpav_dms_getdevice(dms);
	if (!dev)
		return;

	var = mupnp_device_getstatevariablebyname(dev, CG_UPNPAV_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID);
	if (!var)
		return;

	mupnp_statevariable_setvalue(var, mupnp_int2str(id, intBuf, MUPNP_STRING_INTEGER_BUFLEN));
}

/****************************************
* mupnp_upnpav_dms_condir_getsystemupdateid
****************************************/

int mupnp_upnpav_dms_condir_getsystemupdateid(mUpnpAvServer *dms)
{
	mUpnpDevice *dev;
	mUpnpStateVariable *var;

	dev = mupnp_upnpav_dms_getdevice(dms);
	if (!dev)
		return 0;

	var = mupnp_device_getstatevariablebyname(dev, CG_UPNPAV_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID);
	if (!var)
		return 0;

    /* Thanks for Jorgen Lundman(2011-05-09) */
    if (!mupnp_statevariable_getvalue(var))
        return 0;

	return mupnp_str2int(mupnp_statevariable_getvalue(var));
}

/****************************************
* mupnp_upnpav_dms_condir_updatesystemupdateid
****************************************/

void mupnp_upnpav_dms_condir_updatesystemupdateid(mUpnpAvServer *dms)
{
	mupnp_upnpav_dms_condir_setsystemupdateid(dms, (mupnp_upnpav_dms_condir_getsystemupdateid(dms) + 1));
}

/****************************************
* mupnp_upnpav_dms_condir_browsemetadata
****************************************/
static bool mupnp_upnpav_dms_condir_browsemetadata(mUpnpAvServer *dms, mUpnpAction *action)
{
	char *objectID;
	mUpnpAvContent *objectContent;
	mUpnpXmlNode *didlNode;
	mUpnpString *resultStr;
	char intBuf[MUPNP_STRING_INTEGER_BUFLEN];
	mUpnpAvContent *copyContent;

	objectID = mupnp_action_getargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
	if (mupnp_strlen(objectID) <= 0)
		return false;

	objectContent = mupnp_upnpav_dms_findcontentbyid(dms, objectID);
	if (!objectContent)
		return false;

    didlNode = mupnp_upnpav_didl_node_new();
    copyContent = mupnp_upnpav_content_new();
    mupnp_upnpav_content_copy(copyContent, objectContent);

	mupnp_xml_node_addchildnode(didlNode, copyContent);

	resultStr = mupnp_string_new();
	mupnp_upnpav_didl_node_tostring(didlNode, resultStr);

	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_RESULT, mupnp_string_getvalue(resultStr));
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED, "1");
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES, "1");
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID, (char*)mupnp_int2str(mupnp_upnpav_dms_condir_getsystemupdateid(dms), intBuf, sizeof(intBuf)));

    /*
      printf("browsemetadata reply: '%s'\n",
           mupnp_string_getvalue(resultStr));
    */

	mupnp_string_delete(resultStr);
	mupnp_upnpav_didl_node_delete(didlNode);

	return true;
}

/****************************************
* mupnp_upnpav_dms_condir_browsedirectchildren
****************************************/

static bool mupnp_upnpav_dms_condir_browsedirectchildren(mUpnpAvServer *dms, mUpnpAction *action)
{
	char *objectID;
	mUpnpAvContent *objectContent;
	mUpnpAvContent *content;
	mUpnpAvContent *copyContent;
	mUpnpAvContentList *sortedContentList;
	mUpnpAvContent **sortedContentArray;
	int startingIndex;
	int requestedCount;
	int numberReturned;
	int totalMachesCnt;
	char intBuf[MUPNP_STRING_INTEGER_BUFLEN];
	int n;
	mUpnpXmlNode *didlNode;
	mUpnpString *resultStr;

	objectID = mupnp_action_getargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
	if (mupnp_strlen(objectID) <= 0)
		return false;

	objectContent = mupnp_upnpav_dms_findcontentbyid(dms, objectID);
	if (!objectContent)
		return false;

	totalMachesCnt = 0;
	sortedContentList = mupnp_upnpav_contentlist_new();
	for (content=mupnp_upnpav_content_getchildcontents(objectContent); content; content=mupnp_upnpav_content_next(content)) {
		copyContent = mupnp_upnpav_content_new();
		mupnp_upnpav_content_copy(copyContent, content);
		mupnp_upnpav_contentlist_add(sortedContentList, copyContent);
		totalMachesCnt++;
	}

	/* Not Implemented
	// Sort Content Node Lists
	string sortCriteria = action->getSortCriteria();
	ContentNodeList sortedContentNodeBufList(false);
	ContentNodeList *sortedContentNodeList = sortContentNodeList(&contentNodeList, sortCriteria.c_str(), sortedContentNodeBufList);
	*/

	startingIndex = mupnp_str2int(mupnp_action_getargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_STARTING_INDEX));
	if (startingIndex <= 0)
		startingIndex = 0;

	requestedCount = mupnp_str2int(mupnp_action_getargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_REQUESTED_COUNT));
	if (requestedCount <= 0)
		requestedCount = mupnp_upnpav_contentlist_size(sortedContentList);

	numberReturned = 0;

	didlNode = mupnp_upnpav_didl_node_new();

	sortedContentArray = (mUpnpAvContent **)malloc(sizeof(mUpnpAvContent*) * totalMachesCnt);
	n = 0;
	for (content=mupnp_upnpav_contentlist_gets(sortedContentList); content; content=mupnp_upnpav_content_next(content)) {
		sortedContentArray[n] = content;
		n++;
	}

	for (n=startingIndex; (n<totalMachesCnt && numberReturned<requestedCount); n++) {
		content = sortedContentArray[n];
		mupnp_upnpav_content_remove(content);
		mupnp_upnpav_content_addchildcontent(didlNode, content);
		mupnp_upnpav_content_setparentid(content, objectID);
		numberReturned++;
	}

	resultStr = mupnp_string_new();
	mupnp_upnpav_didl_node_tostring(didlNode, resultStr);

	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_RESULT, mupnp_string_getvalue(resultStr));
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_NUMBER_RETURNED, (char*)mupnp_int2str(numberReturned, intBuf, sizeof(intBuf)));
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_TOTAL_MACHES, (char*)mupnp_int2str(totalMachesCnt, intBuf, sizeof(intBuf)));
	mupnp_action_setargumentvaluebyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_UPDATE_ID, (char*)mupnp_int2str(mupnp_upnpav_dms_condir_getsystemupdateid(dms), intBuf, sizeof(intBuf)));

	mupnp_string_delete(resultStr);
	free(sortedContentArray);
	mupnp_upnpav_didl_node_delete(didlNode);
	mupnp_upnpav_contentlist_delete(sortedContentList);

	return true;
}

/****************************************
* mupnp_upnpav_dms_condir_actionreceived
****************************************/

bool mupnp_upnpav_dms_condir_actionreceived(mUpnpAction *action)
{
	mUpnpAvServer *dms;
	mUpnpDevice *dev;
	char *actionName;
	mUpnpArgument *arg;
	char *argValue;
	char intBuf[MUPNP_STRING_INTEGER_BUFLEN];

	actionName = (char*)mupnp_action_getname(action);
	if (mupnp_strlen(actionName) <= 0)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(mupnp_action_getservice(action));
	if (!dev)
		return false;

	dms = (mUpnpAvServer *)mupnp_device_getuserdata(dev);
	if (!dms)
		return false;

	/* Browse */
	if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE))
	{
		arg = mupnp_action_getargumentbyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_FLAG);
		if (!arg)
			return false;
		argValue = mupnp_argument_getvalue(arg);
		if (mupnp_streq(argValue, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_METADATA))
			return mupnp_upnpav_dms_condir_browsemetadata(dms, action);
		if (mupnp_streq(argValue, CG_UPNPAV_DMS_CONTENTDIRECTORY_BROWSE_BROWSE_DIRECT_CHILDREN))
			return mupnp_upnpav_dms_condir_browsedirectchildren(dms, action);
		return false;
	}

	/* Search */
	if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH)) {
		/* Not Implemented */
		return false;
	}

	/* Sort Capabilities */
	if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SORT_CAPABILITIES)) {
		arg = mupnp_action_getargumentbyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPS);
		if (!arg)
			return false;
		mupnp_argument_setvalue(arg, "");
		return true;
	}

	/* Search Capabilities */
	if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SEARCH_CAPABILITIES)) {
		arg = mupnp_action_getargumentbyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPS);
		if (!arg)
			return false;
		mupnp_argument_setvalue(arg, "");
		return true;
	}

	/* System Update ID */
	if (mupnp_streq(actionName, CG_UPNPAV_DMS_CONTENTDIRECTORY_GET_SYSTEM_UPDATE_ID)) {
		arg = mupnp_action_getargumentbyname(action, CG_UPNPAV_DMS_CONTENTDIRECTORY_ID);
		if (!arg)
			return false;
		mupnp_argument_setvalue(arg, mupnp_int2str(mupnp_upnpav_dms_condir_getsystemupdateid(dms), intBuf, MUPNP_STRING_INTEGER_BUFLEN));
		return true;
	}

	return false;
}

/****************************************
* mupnp_upnpav_dms_condir_queryreceived
****************************************/


bool mupnp_upnpav_dms_condir_queryreceived(mUpnpStateVariable *var)
{
	mUpnpAvServer *dms;
	mUpnpDevice *dev;
	char *varName;
	char intBuf[MUPNP_STRING_INTEGER_BUFLEN];

	varName = (char*)mupnp_statevariable_getname(var);
	if (mupnp_strlen(varName) <= 0)
		return false;

	dev = (mUpnpDevice *)mupnp_service_getdevice(mupnp_statevariable_getservice(var));
	if (!dev)
		return false;

	dms = (mUpnpAvServer *)mupnp_device_getuserdata(dev);
	if (!dms)
		return false;

	/* Sort Capabilities */
	if (mupnp_streq(varName, CG_UPNPAV_DMS_CONTENTDIRECTORY_SORT_CAPABILITIES)) {
		mupnp_statevariable_setvalue(var, "");
		return true;
	}

	/* Search Capabilities */
	if (mupnp_streq(varName, CG_UPNPAV_DMS_CONTENTDIRECTORY_SEARCH_CAPABILITIES)) {
		mupnp_statevariable_setvalue(var, "");
		return true;
	}

	/* System Update ID */
	if (mupnp_streq(varName, CG_UPNPAV_DMS_CONTENTDIRECTORY_SYSTEM_UPDATE_ID)) {
		mupnp_statevariable_setvalue(var, mupnp_int2str(mupnp_upnpav_dms_condir_getsystemupdateid(dms), intBuf, MUPNP_STRING_INTEGER_BUFLEN));
		return true;
	}

	return false;
}

/****************************************
* mupnp_upnpav_dms_condir_init
****************************************/

bool mupnp_upnpav_dms_condir_init(mUpnpAvServer *dms)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAction* action;

	dev = mupnp_upnpav_dms_getdevice(dms);
	if (!dev)
		return false;

	service = mupnp_device_getservicebytype(dev, CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_TYPE);
	if (!service)
		return false;

	if (mupnp_service_parsedescription(service, CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION, mupnp_strlen(CG_UPNPAV_DMS_CONTENTDIRECTORY_SERVICE_DESCRIPTION)) == false)
		return false;

	mupnp_service_setactionlistener(service, mupnp_upnpav_dms_condir_actionreceived);
	for (action=mupnp_service_getactions(service); action; action=mupnp_action_next(action))
		mupnp_action_setuserdata(action, dms);

	mupnp_upnpav_dms_condir_setsystemupdateid(dms, 1);


	return true;
}



/****************************************
* mupnp_upnpav_dms_medrec_actionreceived
****************************************/
// Used for xbox360 support, see README.360
bool mupnp_upnpav_dms_medrec_actionreceived(mUpnpAction *action)
{
    char *actionName;
    mUpnpArgument *arg;

    actionName = (char*)mupnp_action_getname(action);

    if (mupnp_strlen(actionName) <= 0)
        return false;


    if (mupnp_streq(actionName, CG_UPNPAV_DMS_MEDIARECEIVER_IS_AUTHORIZED )) {
        arg = mupnp_action_getargumentbyname(action,
                                               CG_UPNPAV_DMS_MEDIARECEIVER_RESULT);
        if (!arg)
            return false;
        mupnp_argument_setvalue(arg, "1");
        return true;
    }

    if (mupnp_streq(actionName, CG_UPNPAV_DMS_MEDIARECEIVER_IS_VALIDATED)) {
        arg = mupnp_action_getargumentbyname(action,
                                               CG_UPNPAV_DMS_MEDIARECEIVER_RESULT);
        if (!arg)
            return false;
        mupnp_argument_setvalue(arg, "1");
        return true;
    }

    if (mupnp_streq(actionName, CG_UPNPAV_DMS_MEDIARECEIVER_REGISTER_DEVICE)) {
        arg = mupnp_action_getargumentbyname(action,
                                               CG_UPNPAV_DMS_MEDIARECEIVER_REGISTRATION_RESP_MSG);
        if (!arg)
            return false;
        // Specifications say to return base64 message.
        mupnp_argument_setvalue(arg, "1");
        return true;
    }


    return true;
}



/****************************************
* mupnp_upnpav_dms_medrec_init
****************************************/

bool mupnp_upnpav_dms_medrec_init(mUpnpAvServer *dms)
{
	mUpnpDevice *dev;
	mUpnpService *service;
	mUpnpAction* action;

	dev = mupnp_upnpav_dms_getdevice(dms);
	if (!dev)
		return false;

	service = mupnp_device_getservicebytype(dev, CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_TYPE);
	if (!service)
		return false;

	if (mupnp_service_parsedescription(service, CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_DESCRIPTION, mupnp_strlen(CG_UPNPAV_DMS_MEDIARECEIVER_SERVICE_DESCRIPTION)) == false)
		return false;

	mupnp_service_setactionlistener(service, mupnp_upnpav_dms_medrec_actionreceived);
	for (action=mupnp_service_getactions(service); action; action=mupnp_action_next(action))
		mupnp_action_setuserdata(action, dms);

	return true;
}
