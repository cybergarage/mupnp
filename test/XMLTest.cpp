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
#include <string>

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

static bool parse_bytes(const char* bytes, size_t size)
{
  mUpnpXmlParser* parser = mupnp_xml_parser_new();
  mUpnpXmlNodeList* nodes = mupnp_xml_nodelist_new();
  bool parsed = mupnp_xml_parse(parser, nodes, bytes, size);
  mupnp_xml_nodelist_delete(nodes);
  mupnp_xml_parser_delete(parser);
  return parsed;
}

BOOST_AUTO_TEST_CASE(XMLByteSpan)
{
  const char bytes[] = { '<', 'r', '/', '>' };
  BOOST_CHECK(parse_bytes(bytes, sizeof(bytes)));
  const char embeddedNul[] = { '<', 'r', '/', '>', '\0', '<', 's', '/', '>' };
  BOOST_CHECK(!parse_bytes(embeddedNul, sizeof(embeddedNul)));
  const char incompleteUtf8[] = { '<', 'r', '>', (char)0xf0, (char)0x90 };
  BOOST_CHECK(!parse_bytes(incompleteUtf8, sizeof(incompleteUtf8)));
  BOOST_CHECK(parse_bytes("<r>normal</r>", strlen("<r>normal</r>")));
  BOOST_CHECK(parse_bytes("<r/>", sizeof("<r/>")));
  BOOST_CHECK(!parse_bytes(bytes, (size_t)INT_MAX + 1));
}

BOOST_AUTO_TEST_CASE(XMLDepthLimit)
{
  std::string xml;
  for (int depth = 0; depth < MUPNP_XML_MAX_DEPTH; depth++)
    xml += "<r>";
  std::string close;
  for (int depth = 0; depth < MUPNP_XML_MAX_DEPTH; depth++)
    close += "</r>";
  BOOST_CHECK(parse_bytes((xml + close).data(), (xml + close).size()));
  xml += "<r>";
  close += "</r>";
  BOOST_CHECK(!parse_bytes((xml + close).data(), (xml + close).size()));
  for (int depth = 0; depth < 10000; depth++)
    xml += "<r>";
  BOOST_CHECK(!parse_bytes(xml.data(), xml.size()));
  BOOST_CHECK(parse_bytes("<r><a/><b/><c/></r>", strlen("<r><a/><b/><c/></r>")));
}
