/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: crss.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "crss.h"
#include "cwebservice.h"

#include <mupnp/http/http.h>
#include <mupnp/xml/xml.h>

#include <mupnp/std/av/ccontent.h>
#include <mupnp/std/av/cmd5.h>

/************************************************************
* mupnp_http_getrestresponse
************************************************************/

mUpnpMediaContent* mupnp_http_getrsscontents(char* rssURL)
{
  mUpnpString* content_str;
  char* content_string;
  mUpnpXmlParser* xmlParser;
  mUpnpXmlNodeList* nodeList;
  mUpnpXmlNode* rootNode;
  mUpnpXmlNode* channelNode;
  mUpnpXmlNode* node;
  mUpnpXmlNode* childNode;
  char* container_title;
  char* content_title;
  char* contentURL;
  char containerID[CG_MD5_STRING_BUF_SIZE];
  char contentID[CG_MD5_STRING_BUF_SIZE];
  long contentSize;
  int nContentent;
  char* contentMimeType;
  mUpnpMediaContent* content;
  mUpnpMediaContent* container;
  mUpnpMediaResource* res;

  content_str = mupnp_string_new();

  if (!mupnp_http_getrestresponse(rssURL, content_str)) {
    mupnp_string_delete(content_str);
    return NULL;
  }

  content_string = mupnp_string_getvalue(content_str);
  if (mupnp_strlen(content_string) <= 0) {
    mupnp_string_delete(content_str);
    return NULL;
  }

  nodeList = mupnp_xml_nodelist_new();
  xmlParser = mupnp_xml_parser_new();
  if (mupnp_xml_parse(xmlParser, nodeList, content_string, mupnp_strlen(content_string)) == FALSE) {
    mupnp_string_delete(content_str);
    mupnp_xml_parser_delete(xmlParser);
    mupnp_xml_nodelist_delete(nodeList);
    return NULL;
  }

  mupnp_string_delete(content_str);
  mupnp_xml_parser_delete(xmlParser);

  nContentent = 0;
  rootNode = mupnp_xml_nodelist_gets(nodeList);
  if (rootNode == NULL) {
    mupnp_xml_nodelist_delete(rootNode);
    return NULL;
  }

  channelNode = mupnp_xml_node_getchildnode(rootNode, "channel");
  if (channelNode == NULL) {
    mupnp_xml_nodelist_delete(rootNode);
    return NULL;
  }

  /**** container ****/

  // Title
  container_title = "";
  childNode = mupnp_xml_node_getchildnode(channelNode, "title");
  if (childNode) {
    if (mupnp_xml_node_getvalue(childNode))
      container_title = mupnp_xml_node_getvalue(childNode);
  }

  if (mupnp_strlen(container_title) <= 0) {
    mupnp_xml_nodelist_delete(rootNode);
    return NULL;
  }

  container = mupnp_media_content_new();
  mupnp_media_content_settype(container, MUPNP_MEDIA_CONTENT_CONTAINER);
  mupnp_media_content_settitle(container, container_title);
  mupnp_str2md5(container_title, containerID);
  mupnp_media_content_setid(container, containerID);

  /**** item ****/
  for (node = mupnp_xml_node_getchildnodes(channelNode); node; node = mupnp_xml_node_next(node)) {

    if (!mupnp_xml_node_isname(node, "item"))
      continue;

    content_title = "";
    contentURL = "";
    contentSize = 0;

    // Title
    childNode = mupnp_xml_node_getchildnode(node, "title");
    if (childNode) {
      if (mupnp_xml_node_getvalue(childNode))
        content_title = mupnp_xml_node_getvalue(childNode);
    }

    // Enclosure
    childNode = mupnp_xml_node_getchildnode(node, "enclosure");
    if (childNode) {
      // url
      if (mupnp_xml_node_getattributevalue(childNode, "url"))
        contentURL = mupnp_xml_node_getattributevalue(childNode, "url");
      // type
      if (mupnp_xml_node_getattributevalue(childNode, "type"))
        contentMimeType = mupnp_xml_node_getattributevalue(childNode, "type");
      // type
      if (mupnp_xml_node_getattributevalue(childNode, "length"))
        contentSize = atol(mupnp_xml_node_getattributevalue(childNode, "length"));
    }

    if (mupnp_strlen(content_title) <= 0 || mupnp_strlen(contentURL) <= 0)
      continue;

    content = mupnp_media_content_new();
    mupnp_media_content_settype(content, MUPNP_MEDIA_CONTENT_ITEM);

    /**** content name ****/
    content_title = mupnp_strtrim(content_title, " ", 1);
    if (mupnp_strlen(content_title) <= 0) {
      continue;
    }
    mupnp_media_content_settitle(content, content_title);

    /**** content id ****/
    mupnp_str2md5(contentURL, contentID);
    mupnp_media_content_setid(content, contentID);
    mupnp_media_content_addchildcontent(container, content);

    res = mupnp_media_resource_new();
    mupnp_media_resource_setmimetype(res, contentMimeType);
    mupnp_media_resource_seturl(res, contentURL);
    mupnp_media_resource_setsize(res, contentSize);
    mupnp_media_content_addresource(content, res);

    nContentent++;
  }

  mupnp_xml_nodelist_delete(nodeList);

  return container;
}
