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
	../XMLTest.cpp \
	../ControlPointTest.cpp \
	../TestDevice.c \
	../mUpnpTestMain.cpp \
	../DeviceTest.cpp \
	../UPnPFuncTest.cpp \
	../StringTest.cpp \
	../ServiceTest.cpp \
	../SubscriptionTest.cpp \
	../NetworkTest.cpp \
	../DictionaryTest.cpp \
	../URITest.cpp \
	../StateVariableTest.cpp \
	../HttpTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
