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

#ifndef _MUPNP_ICON_H_
#define _MUPNP_ICON_H_

#include <mupnp/typedef.h>
#include <mupnp/xml/xml.h>
#include <mupnp/util/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_ICON_ELEM_NAME "icon"
#define MUPNP_ICONLIST_ELEM_NAME "iconList"

#define MUPNP_ICON_MIME_TYPE "mimetype"
#define MUPNP_ICON_WIDTH "width"
#define MUPNP_ICON_HEIGHT "height"
#define MUPNP_ICON_DEPTH "depth"
#define MUPNP_ICON_URL "url"

/****************************************
* Data Type
****************************************/
/**
 * The generic icon struct
 */
typedef struct _mUpnpIcon {
  MUPNP_LIST_STRUCT_MEMBERS

  mUpnpXmlNode* iconNode;
} mUpnpIcon, mUpnpIconList;

/****************************************
* Function (Icon)
****************************************/

/**
 * Create a new icon struct
 */
mUpnpIcon* mupnp_icon_new();

/**
 * Delete an icon struct
 *
 * \param dev Icon to delete
 */
void mupnp_icon_delete(mUpnpIcon* dev);

/**
 * Get the next icon in list. Use as an iterator.
 * 
 * \param icon Node in a list of icons
 */
#define mupnp_icon_next(icon) (mUpnpIcon*) mupnp_list_next((mUpnpList*)icon)

/**
 * Check whether the given node is a member of an icon list
 * 
 * \param node List node
 */
#define mupnp_icon_isiconnode(node) mupnp_xml_node_isname(node, MUPNP_ICON_ELEM_NAME)

/**
 * Set the XML icon node to an icon struct
 * 
 * \param icon The icon struct
 * \param node XML node
 */
#define mupnp_icon_seticonnode(icon, node) (icon->iconNode = node)

/**
 * Get the XML icon node from an icon struct
 * 
 * \param icon The icon struct
 */
#define mupnp_icon_geticonnode(icon) (icon->iconNode)

/*****************************************************************************
 * mimeType
 *****************************************************************************/

/**
 * Set the icon's MIME type
 * 
 * \param icon Icon in question
 * \param value MIME type
 */
#define mupnp_icon_setmimetype(icon, value) mupnp_xml_node_setchildnode(mupnp_icon_geticonnode(icon), MUPNP_ICON_MIME_TYPE, value)

/**
 * Get the icon's MIME type
 * 
 */
#define mupnp_icon_getmimetype(icon) mupnp_xml_node_getchildnodevalue(mupnp_icon_geticonnode(icon), MUPNP_ICON_MIME_TYPE)

/*****************************************************************************
 * width 
 *****************************************************************************/

/**
 * Set icon width
 * 
 * \param icon Icon in question
 * \param value Icon width
 */
#define mupnp_icon_setwidth(icon, value) mupnp_xml_node_setchildnode(mupnp_icon_geticonnode(icon), MUPNP_ICON_WIDTH, value)

/**
 * Get icon width
 * 
 */
#define mupnp_icon_getwidth(icon) mupnp_xml_node_getchildnodeintvalue(mupnp_icon_geticonnode(icon), MUPNP_ICON_WIDTH)

/*****************************************************************************
 * height 
 *****************************************************************************/

/**
 * Set icon height
 * 
 * \param icon Icon in question
 * \param value Icon height
 */
#define mupnp_icon_setheight(icon, value) mupnp_xml_node_setchildnode(mupnp_icon_geticonnode(icon), MUPNP_ICON_HEIGHT, value)

/**
 * Get icon height
 * 
 */
#define mupnp_icon_getheight(icon) mupnp_xml_node_getchildnodeintvalue(mupnp_icon_geticonnode(icon), MUPNP_ICON_HEIGHT)

/*****************************************************************************
 * Colour Depth 
 *****************************************************************************/

/**
 * Set icon's colour depth
 * 
 * \param icon Icon in question
 * \param value Colour depth
 */
#define mupnp_icon_setdepth(icon, value) mupnp_xml_node_setchildnode(mupnp_icon_geticonnode(icon), MUPNP_ICON_DEPTH, value)

/**
 * Get icon's colour depth
 * 
 */
#define mupnp_icon_getdepth(icon) mupnp_xml_node_getchildnodeintvalue(mupnp_icon_geticonnode(icon), MUPNP_ICON_DEPTH)

/*****************************************************************************
 * URL
 *****************************************************************************/

/**
 * Set icon's URL
 * 
 * \param icon Icon in question
 * \param value Icon's URL
 */
#define mupnp_icon_seturl(icon, value) mupnp_xml_node_setchildnode(mupnp_icon_geticonnode(icon), MUPNP_ICON_URL, value)

/**
 * Get icon's URL
 * 
 */
#define mupnp_icon_geturl(icon) mupnp_xml_node_getchildnodevalue(mupnp_icon_geticonnode(icon), MUPNP_ICON_URL)

/*****************************************************************************
 * Function (IconList)
 *****************************************************************************/

/**
 * Create a new list of icons
 * 
 */
mUpnpIconList* mupnp_iconlist_new();

/**
 * Delete a list of icons
 * 
 * \param iconList List of icons
 */
void mupnp_iconlist_delete(mUpnpIconList* iconList);

/**
 * Clear the contents of an icon list
 * 
 * \param iconList List of icons
 */
#define mupnp_iconlist_clear(iconList) mupnp_list_clear((mUpnpList*)iconList, (MUPNP_LIST_DESTRUCTORFUNC)mupnp_icon_delete)

/**
 * Get the size (number of elements) of an icon list
 * 
 * \param iconList List of icons
 */
#define mupnp_iconlist_size(iconList) mupnp_list_size((mUpnpList*)iconList)

/**
 * \todo Get the first icon from the list for iteration
 * 
 * \param iconList List of icons
 */
#define mupnp_iconlist_gets(iconList) (mUpnpIcon*) mupnp_list_next((mUpnpList*)iconList)

/**
 * Add an icon to an icon list
 * 
 * \param iconList List of icons
 * \param icon Icon to add
 */
#define mupnp_iconlist_add(iconList, icon) mupnp_list_add((mUpnpList*)iconList, (mUpnpList*)icon)

#ifdef __cplusplus

} /* extern "C" */

#endif

#endif
