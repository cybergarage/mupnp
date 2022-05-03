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
	../TestDevice.h \
	../ClinkUnitTest.h

mupnptest_SOURCES = \
	../UPnPFuncTest.cpp \
	../HttpTest.cpp \
	../ServiceTest.cpp \
	../SubscriptionTest.cpp \
	../DictionaryTest.cpp \
	../URITest.cpp \
	../XMLTest.cpp \
	../StringTest.cpp \
	../ControlPointTest.cpp \
	../mUpnpTestMain.cpp \
	../DeviceTest.cpp \
	../StateVariableTest.cpp \
	../NetworkTest.cpp \
	../TestDevice.c
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
