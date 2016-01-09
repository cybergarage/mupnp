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
	../ClinkUnitTest.h \
	../TestDevice.h

mupnptest_SOURCES = \
	../ServiceTest.cpp \
	../DeviceTest.cpp \
	../NetworkTest.cpp \
	../ControlPointTest.cpp \
	../StateVariableTest.cpp \
	../StringTest.cpp \
	../DictionaryTest.cpp \
	../TestDevice.c \
	../HttpTest.cpp \
	../UPnPFuncTest.cpp \
	../URITest.cpp \
	../mUpnpTestMain.cpp \
	../SubscriptionTest.cpp \
	../XMLTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
