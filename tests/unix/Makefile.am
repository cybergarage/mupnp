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
	../TestDevice.h \
	../ClinkUnitTest.h

mupnptest_SOURCES = \
	../URITest.cpp \
	../ControlPointTest.cpp \
	../ServiceTest.cpp \
	../mUpnpTestMain.cpp \
	../UPnPFuncTest.cpp \
	../XMLTest.cpp \
	../StringTest.cpp \
	../TestDevice.c \
	../StateVariableTest.cpp \
	../DeviceTest.cpp \
	../SubscriptionTest.cpp \
	../NetworkTest.cpp \
	../DictionaryTest.cpp \
	../HttpTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
