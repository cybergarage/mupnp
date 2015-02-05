############################################################
#
# CyberGarage for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

noinst_PROGRAMS = clinkctestunit

AM_CPPFLAGS = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	..//ClinkUnitTest.h \
	..//TestDevice.h

clinkctestunit_SOURCES = \
	..//ControlPointTest.cpp \
	..//DeviceTest.cpp \
	..//DictionaryTest.cpp \
	..//HttpTest.cpp \
	..//mUpnpTestMain.cpp \
	..//NetworkTest.cpp \
	..//ServiceTest.cpp \
	..//StateVariableTest.cpp \
	..//StringTest.cpp \
	..//SubscriptionTest.cpp \
	..//TestDevice.c \
	..//UPnPFuncTest.cpp \
	..//URITest.cpp \
	..//XMLTest.cpp
#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
