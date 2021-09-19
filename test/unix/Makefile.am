############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

TESTS = mupnptest
check_PROGRAMS = mupnptest

AM_CPPFLAGS = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	..//ClinkUnitTest.h \
	..//TestDevice.h

mupnptest_SOURCES = \
	..//UPnPFuncTest.cpp \
	..//XMLTest.cpp \
	..//DictionaryTest.cpp \
	..//TestDevice.c \
	..//DeviceTest.cpp \
	..//SubscriptionTest.cpp \
	..//StringTest.cpp \
	..//mUpnpTestMain.cpp \
	..//ControlPointTest.cpp \
	..//ServiceTest.cpp \
	..//NetworkTest.cpp \
	..//HttpTest.cpp \
	..//URITest.cpp \
	..//StateVariableTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
