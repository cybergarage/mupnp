/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

/*
 * Allocation-failure regression for the XML attribute path (the ESP32
 * LoadProhibited crash while building XML attributes).
 *
 * Linked with -Wl,--wrap=malloc,--wrap=realloc so that exactly the N-th
 * allocation made by mUPnP fails. For every N up to the number of allocations
 * a successful parse needs, the parser must either succeed or report failure;
 * it must never dereference a NULL allocation. Built and run by
 * run_xml_alloc.sh under ASan/UBSan, which also catches leaks on the failure
 * paths.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mupnp/util/string.h>
#include <mupnp/xml/xml.h>

void* __real_malloc(size_t size);
void* __real_realloc(void* ptr, size_t size);
void* __wrap_malloc(size_t size);
void* __wrap_realloc(void* ptr, size_t size);

static long alloc_count = 0;
static long fail_at = -1; /* -1: not counting, 0: count only, N: fail the N-th (1-based) */

static int injected_failure(void)
{
  if (fail_at < 0)
    return 0;
  alloc_count++;
  return alloc_count == fail_at;
}

void* __wrap_malloc(size_t size)
{
  if (injected_failure())
    return NULL;
  return __real_malloc(size);
}

void* __wrap_realloc(void* ptr, size_t size)
{
  if (injected_failure())
    return NULL;
  return __real_realloc(ptr, size);
}

static void arm(long n)
{
  alloc_count = 0;
  fail_at = n;
}

static long disarm(void)
{
  long n = alloc_count;
  fail_at = -1;
  return n;
}

static const char* const DESCRIPTION = "<?xml version=\"1.0\"?>\n"
                                       "<root xmlns=\"urn:schemas-upnp-org:device-1-0\" configId=\"1\">\n"
                                       "  <specVersion><major>1</major><minor>0</minor></specVersion>\n"
                                       "  <device a=\"1\" b=\"2\" c=\"3\">\n"
                                       "    <deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType>\n"
                                       "    <friendlyName lang=\"en\" x=\"y\">Light</friendlyName>\n"
                                       "    <UDN>uuid:00000000-0000-0000-0000-000000000001</UDN>\n"
                                       "  </device>\n"
                                       "</root>\n";

static int failures = 0;

#define CHECK(cond, ...)                     \
  do {                                       \
    if (!(cond)) {                           \
      fprintf(stderr, "FAIL: " __VA_ARGS__); \
      failures++;                            \
    }                                        \
  } while (0)

/* Count the attributes in the tree; a nameless attribute means a half-built
   attribute leaked into the tree. */
static int count_attrs(mUpnpXmlNode* node)
{
  int n = 0;
  for (mUpnpXmlAttribute* attr = mupnp_xml_node_getattributes(node); attr; attr = mupnp_xml_attribute_next(attr)) {
    if (mupnp_xml_attribute_getname(attr) == NULL)
      return -1000;
    n++;
  }
  for (mUpnpXmlNode* child = mupnp_xml_node_getchildnodes(node); child; child = mupnp_xml_node_next(child)) {
    int c = count_attrs(child);
    if (c < 0)
      return c;
    n += c;
  }
  return n;
}

static void test_parse_under_failure(void)
{
  mUpnpXmlParser* parser = mupnp_xml_parser_new();
  mUpnpXmlNodeList* nodes = mupnp_xml_nodelist_new();
  arm(0);
  bool ok = mupnp_xml_parse(parser, nodes, DESCRIPTION, strlen(DESCRIPTION));
  long total = disarm();
  CHECK(ok, "reference parse failed\n");
  int expected_attrs = count_attrs(mupnp_xml_nodelist_gets(nodes));
  CHECK(expected_attrs == 7, "expected 7 attributes, got %d\n", expected_attrs);
  mupnp_xml_nodelist_delete(nodes);
  mupnp_xml_parser_delete(parser);

  for (long n = 1; n <= total; n++) {
    parser = mupnp_xml_parser_new();
    nodes = mupnp_xml_nodelist_new();
    if (!parser || !nodes) {
      fprintf(stderr, "setup allocation failed\n");
      exit(2);
    }
    arm(n);
    ok = mupnp_xml_parse(parser, nodes, DESCRIPTION, strlen(DESCRIPTION));
    disarm();
    if (ok) {
      /* A reported success must not hide a half-built or dropped attribute.
         Character data may still be dropped by unrelated code paths. */
      int attrs = count_attrs(mupnp_xml_nodelist_gets(nodes));
      CHECK(attrs >= 0, "fail_at=%ld: parse succeeded with a nameless attribute\n", n);
      CHECK(attrs == expected_attrs, "fail_at=%ld: parse succeeded but lost attributes (%d/%d)\n", n, attrs, expected_attrs);
    }
    mupnp_xml_nodelist_delete(nodes);
    mupnp_xml_parser_delete(parser);
  }
  printf("parse: injected a failure at each of %ld allocations\n", total);
}

static void test_attributelist_set_under_failure(void)
{
  /* 1-3: attribute and its two strings, 4: name buffer, 5: value buffer */
  for (long n = 1; n <= 5; n++) {
    mUpnpXmlNode* node = mupnp_xml_node_new();
    arm(n);
    bool ok = mupnp_xml_node_setattribute(node, "name", "value");
    disarm();
    CHECK(!ok, "fail_at=%ld: setattribute reported success\n", n);
    CHECK(mupnp_xml_attributelist_size(node->attrList) == 0,
        "fail_at=%ld: failed setattribute left an attribute behind\n", n);
    mupnp_xml_node_delete(node);
  }
  printf("attributelist_set: failures reported and nothing left behind\n");
}

static void test_string_consistency_after_failure(void)
{
  mUpnpString* str = mupnp_string_new();
  arm(1);
  mupnp_string_setvalue(str, "abcdef");
  disarm();
  CHECK(mupnp_string_getvalue(str) == NULL, "value should be NULL after failed set\n");
  CHECK(mupnp_string_length(str) == 0, "length should be 0 after failed set\n");
  /* Before the fix valueSize stayed 6 and this append wrote at value+6. */
  mupnp_string_addvalue(str, "xy");
  const char* value = mupnp_string_getvalue(str);
  CHECK(value && strcmp(value, "xy") == 0, "append after failed set produced '%s'\n", value ? value : "(null)");
  mupnp_string_delete(str);
  printf("string: consistent after a failed set\n");
}

int main(void)
{
  test_string_consistency_after_failure();
  test_attributelist_set_under_failure();
  test_parse_under_failure();
  if (failures) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  printf("all allocation-failure checks passed\n");
  return 0;
}
