noinst_PROGRAMS = clinkctestunit

INCLUDES = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	../ClinkTestCase.h \
	../TestDevice.h

clinkctestunit_SOURCES = \
	../TestDevice.c \
	../ClinkTestCase.cpp \
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
	../cppunitmain.cpp

#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
