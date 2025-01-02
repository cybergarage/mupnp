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
	../ClinkUnitTest.h \
	../TestDevice.h

mupnptest_SOURCES = \
	../TestDevice.c \
	../StringTest.cpp \
	../StateVariableTest.cpp \
	../SubscriptionTest.cpp \
	../URITest.cpp \
	../XMLTest.cpp \
	../NetworkTest.cpp \
	../UPnPFuncTest.cpp \
	../HttpTest.cpp \
	../ControlPointTest.cpp \
	../mUpnpTestMain.cpp \
	../DeviceTest.cpp \
	../DictionaryTest.cpp \
	../ServiceTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@ $(CODE_COVERAGE_LIBS)
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
#endif
