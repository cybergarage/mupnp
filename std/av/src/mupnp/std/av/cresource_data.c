/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cresource_data.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <mupnp/std/av/cresource.h>
#include <mupnp/util/string.h>

/****************************************
* mupnp_upnpav_resource_data_new
****************************************/

mUpnpAvResourceData *mupnp_upnpav_resource_data_new()
{
	mUpnpAvResourceData *resData;
	
	resData = (mUpnpAvResourceData *)malloc(sizeof(mUpnpAvResourceData));
	resData->mimeType = mupnp_string_new();
	resData->dlnaAttr = mupnp_string_new();

	return resData;
}

/****************************************
* mupnp_upnpav_resource_data_delete
****************************************/

void mupnp_upnpav_resource_data_delete(mUpnpAvResourceData *resData)
{
	mupnp_string_delete(resData->mimeType);
	mupnp_string_delete(resData->dlnaAttr);
	free(resData);
}

/****************************************
* mupnp_upnpav_resource_data_copy
****************************************/

void mupnp_upnpav_resource_data_copy(mUpnpAvResourceData *destResData, mUpnpAvResourceData *srcResData)
{
	mupnp_string_setvalue(destResData->mimeType, mupnp_string_getvalue(srcResData->mimeType));
}
