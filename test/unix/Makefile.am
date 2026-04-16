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
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

noinst_HEADERS = \
	../TestDevice.h \
	../ClinkUnitTest.h

mupnptest_SOURCES = \
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
