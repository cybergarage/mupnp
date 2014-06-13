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
	../TestDevice.h \
	../ClinkUnitTest.h

clinkctestunit_SOURCES = \
	../ClinkTestCaseHttp.cpp \
	../ClinkTestCaseNetwork.cpp \
	../TestDevice.c \
	../ClinkTestCaseControlPoint.cpp \
	../ClinkTestMain.cpp \
	../ClinkTestCaseDictionary.cpp \
	../ClinkTestCaseService.cpp \
	../ClinkTestCaseURI.cpp \
	../ClinkTestCaseStateVariable.cpp \
	../ClinkTestCaseString.cpp \
	../ClinkTestCaseSubscription.cpp \
	../ClinkTestCaseUPnPFunc.cpp \
	../ClinkTestCaseDevice.cpp
#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
