############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

TESTS = mupnptest
check_PROGRAMS = mupnptest

AM_CPPFLAGS = -I$(top_srcdir)/include -I../ $(CODE_COVERAGE_CPPFLAGS)

noinst_HEADERS = \
	..//ClinkUnitTest.h \
	..//TestDevice.h

mupnptest_SOURCES = \
	..//UPnPFuncTest.cpp \
	..//XMLTest.cpp \
	..//DictionaryTest.cpp \
	..//TestMain.cpp \
	..//TestDevice.c \
	..//DeviceTest.cpp \
	..//SubscriptionTest.cpp \
	..//StringTest.cpp \
	..//ControlPointTest.cpp \
	..//ServiceTest.cpp \
	..//NetworkTest.cpp \
	..//HttpTest.cpp \
	..//URITest.cpp \
	..//StateVariableTest.cpp
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
