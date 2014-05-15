noinst_PROGRAMS = clinkctestunit

AM_CPPFLAGS = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	../TestDevice.h

clinkctestunit_SOURCES = \
	../TestDevice.c \
	../ClinkTestCaseControlPoint.cpp \
	../ClinkTestCaseDevice.cpp \
	../ClinkTestCaseDictionary.cpp \
	../ClinkTestCaseHttp.cpp \
	../ClinkTestCaseNetwork.cpp \
	../ClinkTestCaseService.cpp \
	../ClinkTestCaseStateVariable.cpp \
	../ClinkTestCaseString.cpp \
	../ClinkTestCaseSubscription.cpp \
	../ClinkTestCaseURI.cpp \
	../ClinkTestCaseUPnPFunc.cpp \
	../ClinkTestMain.cpp

#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
