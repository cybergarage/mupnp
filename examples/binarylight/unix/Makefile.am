samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpbinarylight

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(srcdir)/../ -I$(srcdir)/../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@ $(CODE_COVERAGE_CFLAGS)

noinst_HEADERS = \
	../binarylight_device.h \
	../../common/kb_util.h

upnpbinarylight_SOURCES = \
	binarylight_main.c \
	../binarylight_device.c \
	../../common/kb_util.c

upnpbinarylight_LDADD = ../../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
