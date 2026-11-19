############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

TESTS = renderer-api.sh mupnptest device-start-no-interfaces.sh controlpoint-start-no-interfaces.sh thread-shutdown.sh socket-wait.sh
check_PROGRAMS = mupnptest
EXTRA_DIST = renderer-api.sh renderer-objc.sh ../RendererObjCTest.m ../RendererLifecycleTest.c ../AvChkLifecycleTest.c ../RendererTest.c ../RendererNetworkGuard.c device-start-no-interfaces.sh controlpoint-start-no-interfaces.sh thread-shutdown.sh socket-wait.sh ../posix/ThreadShutdownTest.c ../posix/SocketWaitTest.c ../posix/README.md ../../doc/posix-shutdown.md
TESTS_ENVIRONMENT = CC="$(CC)" XML_LIBS="@XML_LIBS@" LIBS="$(LIBS)" CODE_COVERAGE_LIBS="$(CODE_COVERAGE_LIBS)"

AM_CPPFLAGS = -I$(top_srcdir)/include -I../ $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

noinst_HEADERS = \
	../GatewayFixture.h \
	../TestDevice.h \
	../ClinkUnitTest.h

mupnptest_SOURCES = \
	../DiscoveryFetchTest.cpp \
	../LocationChangeTest.cpp \
	../ActionResultTest.cpp \
	../GatewayTest.cpp \
	../NetworkTest.cpp \
	../SubscriptionTest.cpp \
	../ServiceTest.cpp \
	../StringTest.cpp \
	../TestDevice.c \
	../DeviceTest.cpp \
	../XMLTest.cpp \
	../ControlPointTest.cpp \
	../DictionaryTest.cpp \
	../HttpTest.cpp \
	../StateVariableTest.cpp \
	../URITest.cpp \
	../TestMain.cpp \
	../UPnPFuncTest.cpp
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
