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
  const char* XML_CHILD_NODE_NAME = "cnode";
  const char* XML_CHILD_NODE_VALUE = "cnode_value";

  mUpnpXmlNode* parentNode = mupnp_xml_node_new();
  BOOST_CHECK(parentNode);

  BOOST_CHECK(!mupnp_xml_node_removechildnode(parentNode, XML_CHILD_NODE_NAME));

  // Set child node

  mupnp_xml_node_setchildnode(parentNode, XML_CHILD_NODE_NAME, XML_CHILD_NODE_VALUE);
  mUpnpXmlNode* childNode = mupnp_xml_node_getchildnode(parentNode, XML_CHILD_NODE_NAME);
  BOOST_CHECK(childNode);
  BOOST_CHECK_EQUAL(strcmp(XML_CHILD_NODE_VALUE, mupnp_xml_node_getvalue(childNode)), 0);
  BOOST_CHECK_EQUAL(strcmp(XML_CHILD_NODE_VALUE, mupnp_xml_node_getchildnodevalue(parentNode, XML_CHILD_NODE_NAME)), 0);

  // Remove child node

  BOOST_CHECK(mupnp_xml_node_removechildnode(parentNode, XML_CHILD_NODE_NAME));
  BOOST_CHECK(mupnp_xml_node_getchildnode(parentNode, XML_CHILD_NODE_NAME) == NULL);
}
