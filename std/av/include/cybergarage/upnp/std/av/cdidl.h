/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2004-2006
*
*	File: cdlna.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_DIDL_H_
#define _CG_CLINKCAV_DIDL_H_

#include <mupnp/xml/xml.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Function
****************************************/

mUpnpXmlNode* mupnp_upnpav_didl_node_new();

#define mupnp_upnpav_didl_node_delete(node) mupnp_xml_node_delete(node)
#define mupnp_upnpav_didl_node_tostring(node, str) mupnp_xml_node_tostring(node, TRUE, str)

#ifdef __cplusplus
}
#endif

#endif
