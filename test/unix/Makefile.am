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
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

noinst_HEADERS = \
	../ClinkUnitTest.h \
	../TestDevice.h

mupnptest_SOURCES = \
	../TestDevice.c \
	../StringTest.cpp \
	../StateVariableTest.cpp \
	../TestMain.cpp \
	../SubscriptionTest.cpp \
	../URITest.cpp \
	../XMLTest.cpp \
	../NetworkTest.cpp \
	../UPnPFuncTest.cpp \
	../HttpTest.cpp \
	../ControlPointTest.cpp \
	../DeviceTest.cpp \
	../DictionaryTest.cpp \
	../ServiceTest.cpp
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
