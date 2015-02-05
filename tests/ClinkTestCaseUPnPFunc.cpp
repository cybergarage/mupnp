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

#include <limits.h>

#include <mupnp/upnp_function.h>

#include <boost/test/unit_test.hpp>

////////////////////////////////////////
// BootID
////////////////////////////////////////

BOOST_AUTO_TEST_CASE(BootID)
{
  int bootId = mupnp_upnp_createbootid();
  BOOST_CHECK(0 < bootId);
  BOOST_CHECK(bootId < INT_MAX);
}
