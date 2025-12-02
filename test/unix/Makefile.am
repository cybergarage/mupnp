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
	../ClinkUnitTest.h \
	../TestDevice.h

mupnptest_SOURCES = \
	../StringTest.cpp \
	../URITest.cpp \
	../SubscriptionTest.cpp \
	../ServiceTest.cpp \
	../ControlPointTest.cpp \
	../NetworkTest.cpp \
	../TestDevice.c \
	../UPnPFuncTest.cpp \
	../DictionaryTest.cpp \
	../StateVariableTest.cpp \
	../XMLTest.cpp \
	../DeviceTest.cpp \
	../TestMain.cpp \
	../HttpTest.cpp
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
