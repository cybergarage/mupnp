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
	..//ClinkTestCaseControlPoint.cpp \
	..//ClinkTestCaseDevice.cpp \
	..//ClinkTestCaseDictionary.cpp \
	..//ClinkTestCaseHttp.cpp \
	..//ClinkTestCaseNetwork.cpp \
	..//ClinkTestCaseService.cpp \
	..//ClinkTestCaseStateVariable.cpp \
	..//ClinkTestCaseString.cpp \
	..//ClinkTestCaseSubscription.cpp \
	..//ClinkTestCaseUPnPFunc.cpp \
	..//ClinkTestCaseURI.cpp \
	..//ClinkTestMain.cpp \
	..//TestDevice.c
#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
