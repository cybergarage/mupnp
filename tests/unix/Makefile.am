############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

TESTS = mupnptest
noinst_PROGRAMS = mupnptest

AM_CPPFLAGS = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	..//ClinkUnitTest.h \
	..//TestDevice.h

mupnptest_SOURCES = \
	..//ControlPointTest.cpp \
	..//DeviceTest.cpp \
	..//DictionaryTest.cpp \
	..//HttpTest.cpp \
	..//mUpnpTestMain.cpp \
	..//NetworkTest.cpp \
	..//ServiceTest.cpp \
	..//StateVariableTest.cpp \
	..//StringTest.cpp \
	..//SubscriptionTest.cpp \
	..//TestDevice.c \
	..//UPnPFuncTest.cpp \
	..//URITest.cpp \
	..//XMLTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
