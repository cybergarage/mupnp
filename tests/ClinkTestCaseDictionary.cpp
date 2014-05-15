/************************************************************
 *
 *	CyberLink for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ************************************************************/

#include <boost/test/unit_test.hpp>

#include <cybergarage/util/cdictionary.h>

////////////////////////////////////////
// testDictionary
////////////////////////////////////////

#define CLINK_TESTCASE_DICTIONARY_KEY "hello"
#define CLINK_TESTCASE_DICTIONARY_VALUE "hello_value"

BOOST_AUTO_TEST_CASE(DictionaryTest)
{
	CgDictionary *dir;
	const char *dirVal;

	/* Basic Unit Test */
	dir = cg_dictionary_new();
	BOOST_CHECK((dir != NULL));
	cg_dictionary_delete(dir);

	/* Complet Unit Test */
	dir = cg_dictionary_new();
	BOOST_CHECK((dir != NULL));
	BOOST_CHECK((cg_dictionary_size(dir) == 0));
	cg_dictionary_setvalue(dir, CLINK_TESTCASE_DICTIONARY_KEY, CLINK_TESTCASE_DICTIONARY_VALUE);
	BOOST_CHECK((cg_dictionary_size(dir) == 1));
	cg_dictionary_setvalue(dir, CLINK_TESTCASE_DICTIONARY_KEY, CLINK_TESTCASE_DICTIONARY_VALUE);
	BOOST_CHECK((cg_dictionary_size(dir) == 1));
	dirVal = cg_dictionary_getvalue(dir, CLINK_TESTCASE_DICTIONARY_KEY);
	BOOST_CHECK((0 < cg_strlen(dirVal)));
	BOOST_CHECK(cg_streq(dirVal, CLINK_TESTCASE_DICTIONARY_VALUE));
	cg_dictionary_delete(dir);
}
