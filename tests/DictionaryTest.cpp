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

#include <mupnp/util/dictionary.h>

////////////////////////////////////////
// testDictionary
////////////////////////////////////////

#define MUPNP_TESTCASE_DICTIONARY_KEY "hello"
#define MUPNP_TESTCASE_DICTIONARY_VALUE "hello_value"

BOOST_AUTO_TEST_CASE(DictionaryTest)
{
  mUpnpDictionary* dir;
  const char* dirVal;

  /* Basic Unit Test */
  dir = mupnp_dictionary_new();
  BOOST_CHECK((dir != NULL));
  mupnp_dictionary_delete(dir);

  /* Complet Unit Test */
  dir = mupnp_dictionary_new();
  BOOST_CHECK((dir != NULL));
  BOOST_CHECK((mupnp_dictionary_size(dir) == 0));
  mupnp_dictionary_setvalue(dir, MUPNP_TESTCASE_DICTIONARY_KEY, MUPNP_TESTCASE_DICTIONARY_VALUE);
  BOOST_CHECK((mupnp_dictionary_size(dir) == 1));
  mupnp_dictionary_setvalue(dir, MUPNP_TESTCASE_DICTIONARY_KEY, MUPNP_TESTCASE_DICTIONARY_VALUE);
  BOOST_CHECK((mupnp_dictionary_size(dir) == 1));
  dirVal = mupnp_dictionary_getvalue(dir, MUPNP_TESTCASE_DICTIONARY_KEY);
  BOOST_CHECK((0 < mupnp_strlen(dirVal)));
  BOOST_CHECK(mupnp_streq(dirVal, MUPNP_TESTCASE_DICTIONARY_VALUE));
  mupnp_dictionary_delete(dir);
}
