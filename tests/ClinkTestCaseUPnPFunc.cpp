#include <limits.h>

#include <cybergarage/upnp/cupnp_function.h>

#include "ClinkTestCase.h"

////////////////////////////////////////
// testHttpServer
////////////////////////////////////////

void ClinkTestCase::testBootID()
{
  int bootId = cg_upnp_createbootid();
  CPPUNIT_ASSERT(0 < bootId);
  CPPUNIT_ASSERT(bootId < INT_MAX);
}
