/************************************************************
 *
 *	CyberLink for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ************************************************************/

#include <limits.h>

#include <cybergarage/xml/cxml.h>

#include <boost/test/unit_test.hpp>

////////////////////////////////////////
// XML
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(XMLChildNode)
{
  const char *XML_CHILD_NODE_NAME = "cnode";
  const char *XML_CHILD_NODE_VALUE = "cnode_value";
  
  CgXmlNode *parentNode = cg_xml_node_new();
  BOOST_CHECK(parentNode);
  
  BOOST_CHECK(!cg_xml_node_removechildnode(parentNode, XML_CHILD_NODE_NAME));
  
  // Set child node
  
  cg_xml_node_setchildnode(parentNode, XML_CHILD_NODE_NAME, XML_CHILD_NODE_VALUE);
  CgXmlNode *childNode = cg_xml_node_getchildnode(parentNode, XML_CHILD_NODE_NAME);
  BOOST_CHECK(childNode);
  BOOST_CHECK_EQUAL(strcmp(XML_CHILD_NODE_VALUE, cg_xml_node_getvalue(childNode)), 0);
  BOOST_CHECK_EQUAL(strcmp(XML_CHILD_NODE_VALUE, cg_xml_node_getchildnodevalue(parentNode, XML_CHILD_NODE_NAME)), 0);

  // Remove child node
  
  BOOST_CHECK(cg_xml_node_removechildnode(parentNode, XML_CHILD_NODE_NAME));
  BOOST_CHECK(!cg_xml_node_getchildnode(parentNode, XML_CHILD_NODE_NAME));
}
