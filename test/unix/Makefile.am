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
	../ClinkUnitTest.h \
	../TestDevice.h

mupnptest_SOURCES = \
	../UPnPFuncTest.cpp \
	../XMLTest.cpp \
	../StringTest.cpp \
	../URITest.cpp \
	../TestDevice.c \
	../SubscriptionTest.cpp \
	../NetworkTest.cpp \
	../mUpnpTestMain.cpp \
	../DictionaryTest.cpp \
	../StateVariableTest.cpp \
	../ControlPointTest.cpp \
	../HttpTest.cpp \
	../ServiceTest.cpp \
	../DeviceTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
