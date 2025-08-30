samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpigddump

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I../ -I../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@ $(CODE_COVERAGE_CFLAGS)

noinst_HEADERS = \
	../../common/kb_util.h

upnpigddump_SOURCES = \
	../upnpigddump.c \
	../../common/kb_util.c

upnpigddump_LDADD = ../../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
