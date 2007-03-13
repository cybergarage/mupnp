#ifndef CPP_UNIT_CDISTTESTCASE_H
#define CPP_UNIT_CDISTTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class ClinkTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ClinkTestCase );

  CPPUNIT_TEST( testXXXXX );

  CPPUNIT_TEST_SUITE_END();

public:

	void setUp();
	void tearDown();

protected:
  
	void testXXXXX();
};

#endif
