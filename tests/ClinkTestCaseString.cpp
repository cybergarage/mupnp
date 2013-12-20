#include "ClinkTestCase.h"

#include <cybergarage/util/cstring.h>

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

#define CLINK_TESTCASE_INTVALUE -100
#define CLINK_TESTCASE_INTVALUE_STRING "-100"
#define CLINK_TESTCASE_UINTVALUE 100
#define CLINK_TESTCASE_UINTVALUE_STRING "100"

#define CLINK_TESTCASE_DBLVALUE -12.3
#define CLINK_TESTCASE_DBLVALUE_STRING "-12.3"
#define CLINK_TESTCASE_UDBLVALUE 12.3
#define CLINK_TESTCASE_UDBLVALUE_STRING "12.3"

void ClinkTestCase::testString()
{
  char buf[128];

  cg_int2str(CLINK_TESTCASE_INTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_INTVALUE_STRING));
  cg_int2str(CLINK_TESTCASE_UINTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UINTVALUE_STRING));

  cg_long2str(CLINK_TESTCASE_INTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_INTVALUE_STRING));
  cg_long2str(CLINK_TESTCASE_UINTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UINTVALUE_STRING));

  /*
  cg_float2str(CLINK_TESTCASE_DBLVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_DBLVALUE_STRING));
  cg_float2str(CLINK_TESTCASE_UDBLVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UDBLVALUE_STRING));

  cg_double2str(CLINK_TESTCASE_DBLVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_DBLVALUE_STRING));
  cg_double2str(CLINK_TESTCASE_UDBLVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UDBLVALUE_STRING));
  */

  cg_sizet2str(CLINK_TESTCASE_UINTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UINTVALUE_STRING));

  cg_ssizet2str(CLINK_TESTCASE_INTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_INTVALUE_STRING));
  cg_ssizet2str(CLINK_TESTCASE_UINTVALUE, buf, sizeof(buf));
	CPPUNIT_ASSERT(cg_streq(buf, CLINK_TESTCASE_UINTVALUE_STRING));
}
