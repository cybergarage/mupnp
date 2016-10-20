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
	../ControlPointTest.cpp \
	../TestDevice.c \
	../SubscriptionTest.cpp \
	../HttpTest.cpp \
	../DictionaryTest.cpp \
	../DeviceTest.cpp \
	../StateVariableTest.cpp \
	../URITest.cpp \
	../UPnPFuncTest.cpp \
	../StringTest.cpp \
	../XMLTest.cpp \
	../NetworkTest.cpp \
	../mUpnpTestMain.cpp \
	../ServiceTest.cpp
#if HAVE_LIBTOOL
#mupnptest_LDADD = ../../lib/unix/libmupnp.la @XML_LIBS@
#else
mupnptest_LDADD = ../../lib/unix/libmupnp.a @XML_LIBS@
#endif
