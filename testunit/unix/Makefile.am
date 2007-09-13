noinst_PROGRAMS = clinkctestunit

INCLUDES = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	../ClinkTestCase.h

clinkctestunit_SOURCES = \
	../ClinkTestCase.cpp \
	../cppunitmain.cpp

if HAVE_LIBTOOL
clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
endif
