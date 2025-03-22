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

#include <boost/test/unit_test.hpp>
#include <limits.h>
#include <mupnp/xml/xml.h>

////////////////////////////////////////
// XML
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(XMLChildNode)
{
  const char* xmlChildNodeName = "cnode";
  const char* xmlChildNodeValue = "cnode_value";

  mUpnpXmlNode* parentNode = mupnp_xml_node_new();
  BOOST_REQUIRE(parentNode);

  BOOST_REQUIRE(!mupnp_xml_node_removechildnode(parentNode, xmlChildNodeName));

  // Set child node

  mupnp_xml_node_setchildnode(parentNode, xmlChildNodeName, xmlChildNodeValue);
  mUpnpXmlNode* childNode = mupnp_xml_node_getchildnode(parentNode, xmlChildNodeName);
  BOOST_REQUIRE(childNode);
  BOOST_REQUIRE_EQUAL(strcmp(xmlChildNodeValue, mupnp_xml_node_getvalue(childNode)), 0);
  BOOST_REQUIRE_EQUAL(strcmp(xmlChildNodeValue, mupnp_xml_node_getchildnodevalue(parentNode, xmlChildNodeName)), 0);

  // Remove child node

  BOOST_REQUIRE(mupnp_xml_node_removechildnode(parentNode, xmlChildNodeName));
  BOOST_REQUIRE(mupnp_xml_node_getchildnode(parentNode, xmlChildNodeName) == NULL);
}
