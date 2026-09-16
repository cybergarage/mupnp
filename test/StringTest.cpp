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

#include <mupnp/util/string.h>

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

#define MUPNP_TESTCASE_INTVALUE -100
#define MUPNP_TESTCASE_INTVALUE_STRING "-100"
#define MUPNP_TESTCASE_UINTVALUE 100
#define MUPNP_TESTCASE_UINTVALUE_STRING "100"

#define MUPNP_TESTCASE_DBLVALUE -12.3
#define MUPNP_TESTCASE_DBLVALUE_STRING "-12.3"
#define MUPNP_TESTCASE_UDBLVALUE 12.3
#define MUPNP_TESTCASE_UDBLVALUE_STRING "12.3"

BOOST_AUTO_TEST_CASE(StringTest)
{
  char buf[128];

  mupnp_int2str(MUPNP_TESTCASE_INTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_INTVALUE_STRING));
  mupnp_int2str(MUPNP_TESTCASE_UINTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UINTVALUE_STRING));

  mupnp_long2str(MUPNP_TESTCASE_INTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_INTVALUE_STRING));
  mupnp_long2str(MUPNP_TESTCASE_UINTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UINTVALUE_STRING));

  /*
  mupnp_float2str(MUPNP_TESTCASE_DBLVALUE, buf, sizeof(buf));
        BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_DBLVALUE_STRING));
  mupnp_float2str(MUPNP_TESTCASE_UDBLVALUE, buf, sizeof(buf));
        BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UDBLVALUE_STRING));

  mupnp_double2str(MUPNP_TESTCASE_DBLVALUE, buf, sizeof(buf));
        BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_DBLVALUE_STRING));
  mupnp_double2str(MUPNP_TESTCASE_UDBLVALUE, buf, sizeof(buf));
        BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UDBLVALUE_STRING));
  */

  mupnp_sizet2str(MUPNP_TESTCASE_UINTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UINTVALUE_STRING));

  mupnp_ssizet2str(MUPNP_TESTCASE_INTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_INTVALUE_STRING));
  mupnp_ssizet2str(MUPNP_TESTCASE_UINTVALUE, buf, sizeof(buf));
  BOOST_REQUIRE(mupnp_streq(buf, MUPNP_TESTCASE_UINTVALUE_STRING));
}

////////////////////////////////////////
// testStringTokenizerEmpty (regression test for buffer overflow)
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(StringTokenizerEmpty)
{
  mUpnpStringTokenizer* tok;

  /* An empty input string previously wrapped lastPos to SIZE_MAX and read
     out of bounds. This exercises the SSDP/HTTP header parsing path with
     an empty datagram/line. */
  tok = mupnp_string_tokenizer_new("", "\r\n");
  BOOST_REQUIRE(mupnp_string_tokenizer_hasmoretoken(tok) == false);
  mupnp_string_tokenizer_delete(tok);

  /* A string consisting only of delimiters must also be handled safely. */
  tok = mupnp_string_tokenizer_new("\r\n", "\r\n");
  while (mupnp_string_tokenizer_hasmoretoken(tok))
    mupnp_string_tokenizer_nexttoken(tok);
  mupnp_string_tokenizer_delete(tok);

  /* Normal tokenization must keep working. */
  tok = mupnp_string_tokenizer_new("a:b:c", ":");
  BOOST_REQUIRE(mupnp_string_tokenizer_hasmoretoken(tok) == true);
  BOOST_REQUIRE(mupnp_streq(mupnp_string_tokenizer_nexttoken(tok), "a"));
  BOOST_REQUIRE(mupnp_streq(mupnp_string_tokenizer_nexttoken(tok), "b"));
  BOOST_REQUIRE(mupnp_streq(mupnp_string_tokenizer_nexttoken(tok), "c"));
  mupnp_string_tokenizer_delete(tok);
}
