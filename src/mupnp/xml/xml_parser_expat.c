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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/typedef.h>

/****************************************
*	Compiler Switch (BEGIN)
****************************************/

#if defined(MUPNP_XMLPARSER_EXPAT) || (!defined(HAVE_CONFIG_H) && !defined(TARGET_OS_IPHONE))

/****************************************
* Header Files
****************************************/

#include <mupnp/xml/xml.h>
#include <mupnp/util/log.h>

#if defined(WIN32)
#define XML_STATIC
#endif

#if !defined(WIN32)
#define XMLCALL
#endif

#if defined(HAVE_EXPAT_EXPAT_H)
#include <expat/expat.h>
#else
#include <expat.h>
#endif

#if defined DEBUG_XML
static int indent = 0;
#endif

#ifdef MUPNP_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

extern long int mupnp_total_elapsed_time;
#endif

/****************************************
* mupnp_xml_parse (Expat)
****************************************/

typedef struct _mUpnpExpatData {
  mUpnpXmlNode* rootNode;
  mUpnpXmlNode* currNode;
} mUpnpExpatData;

static void XMLCALL mupnp_expat_element_start(void* userData, const char* el, const char** attr)
{
  mUpnpExpatData* expatData;
  mUpnpXmlNode* node;
  int n;
#if defined DEBUG_XML
  int out, j;
  char* outbuf;
  char* lpout;
#endif

  mupnp_log_debug_l4("Entering...\n");

#if defined DEBUG_XML
  outbuf = (char*)malloc(4096);
  lpout = outbuf;
  outbuf[4095] = (char)0;
  for (out = 0; out < indent; out++)
    outbuf[out] = (char)' ';
  lpout += out;
  for (j = 0; attr[j]; j += 2) {
    lpout += sprintf(lpout, "%s='%s'", attr[j], attr[j + 1]);
  }
  lpout[0] = 0;
  printf("%8x XML Node start %s - %d Attribute %s\n", userData, el, j / 2, outbuf);
  free(outbuf);
  ++indent;
#endif
  //memdiags_memlist_report_unmarkedsize();

  expatData = (mUpnpExpatData*)userData;

  node = mupnp_xml_node_new();
  mupnp_xml_node_setname(node, (char*)el);

  for (n = 0; attr[n]; n += 2)
    mupnp_xml_node_setattribute(node, (char*)attr[n], (char*)attr[n + 1]);

  if (expatData->rootNode != NULL) {
    if (expatData->currNode != NULL)
      mupnp_xml_node_addchildnode(expatData->currNode, node);
    else
      mupnp_xml_node_addchildnode(expatData->rootNode, node);
  }
  else
    expatData->rootNode = node;

  expatData->currNode = node;

  mupnp_log_debug_l4("Leaving...\n");
}

static void XMLCALL mupnp_expat_element_end(void* userData, const char* el)
{
  mUpnpExpatData* expatData;

  mupnp_log_debug_l4("Entering...\n");

  expatData = (mUpnpExpatData*)userData;

#if defined DEBUG_XML
  //printf("%8x XML end %s\n",userData,el);
  --indent;
#endif
  //memdiags_memlist_report_unmarkedsize();
  if (expatData->currNode != NULL)
    expatData->currNode = mupnp_xml_node_getparentnode(expatData->currNode);

  mupnp_log_debug_l4("Leaving...\n");
}

static void XMLCALL mupnp_expat_character_data(void* userData, const XML_Char* s, int len)
{
  mUpnpExpatData* expatData;
#if defined XML_IGNORE_WHITESPACE
  int i;
#endif

  mupnp_log_debug_l4("Entering...\n");

//	printf("%8x XML data len=%-3d now=%d %s d[0]0x%x\n",userData, len, mupnp_strlen(out->value), out->value, (char)(out->value[0]));

#if defined XML_IGNORE_WHITESPACE
  for (i = 0; i < len; i++) {
    if (!isspace((char)s[i])) {
#endif
      expatData = (mUpnpExpatData*)userData;
      if (expatData->currNode != NULL)
        mupnp_xml_node_naddvalue(expatData->currNode, (char*)s, len);
#if defined XML_IGNORE_WHITESPACE
      break;
    }
  }
#endif
}

bool mupnp_xml_parse(mUpnpXmlParser* parser, mUpnpXmlNodeList* nodeList, const char* data, size_t len)
{
#if defined DEBUG_XML_RESULT
  mUpnpString* resdata = NULL;
#endif
  XML_Parser p;
  mUpnpExpatData expatData;
#ifdef MUPNP_SHOW_TIMINGS
  struct timeval start_time, end_time, elapsed_time;
#endif

  mupnp_log_debug_l4("Entering...\n");

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&start_time, NULL);
#endif

  if (!data || len <= 0)
    return false;

  p = XML_ParserCreate(NULL);
  if (!p)
    return false;
  /* Fix to get expat parser to work with DLink-routers */
  if (data[len - 1] == 0)
    len--;

  expatData.rootNode = NULL;
  expatData.currNode = NULL;
  XML_SetUserData(p, &expatData);
  XML_SetElementHandler(p, mupnp_expat_element_start, mupnp_expat_element_end);
  XML_SetCharacterDataHandler(p, mupnp_expat_character_data);

  parser->parseResult = XML_Parse(p, data, len, 1);
  XML_ParserFree(p);

  if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
    if (expatData.rootNode != NULL)
      mupnp_xml_node_delete(expatData.rootNode);
#if defined DEBUG_XML_RESULT
    resdata = mupnp_string_new();
    mupnp_string_naddvalue(resdata, data, len);
    printf("XML parse Error on data %s\n time used = %ds\n",
        mupnp_string_getvalue(resdata),
        time(NULL) - startTime);
    mupnp_string_delete(resdata);
#endif
    return false;
  }

  mupnp_xml_nodelist_add(nodeList, expatData.rootNode);

#ifdef MUPNP_SHOW_TIMINGS
  gettimeofday(&end_time, NULL);
  timersub(&end_time, &start_time, &elapsed_time);
  mupnp_log_debug_s("Parsing XML completed. Elapsed time: "
                    "%ld msec\n",
      ((elapsed_time.tv_sec * 1000) + (elapsed_time.tv_usec / 1000)));
  mupnp_total_elapsed_time += (elapsed_time.tv_sec * 1000000) + (elapsed_time.tv_usec);
  mupnp_log_debug_s("Total elapsed time: %ld msec\n", mupnp_total_elapsed_time / 1000);
#endif

#if defined DEBUG_XML_RESULT
  resdata = mupnp_string_new();
  mupnp_string_naddvalue(resdata, data, len);
  printf("XML parse success - time used %ds\n", time(NULL) - startTime);
  mupnp_string_delete(resdata);
#endif

  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* Compiler Switch (END)
****************************************/

#endif
