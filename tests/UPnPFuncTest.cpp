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
#include <mupnp/upnp_function.h>

////////////////////////////////////////
// BootID
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(BootID)
{
  int bootId = mupnp_createbootid();
  BOOST_CHECK(0 < bootId);
  BOOST_CHECK(bootId < INT_MAX);
}
