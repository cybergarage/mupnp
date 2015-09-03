noinst_PROGRAMS = cdms_filesys

AM_CPPFLAGS = -I$(top_srcdir)/include -I../ -I../../../common

noinst_HEADERS = \
	../../../common/ckeyboard.h \
	../cdms_filesys.h

cdms_filesys_SOURCES = \
	../../../common/ckeyboard.c \
	../cdms_filesys.c \
	../cdms_filesys_data.c \
	../cdms_filesys_http.c \
	../cdms_filesys_main.c

if HAVE_LIBTOOL
cdms_filesys_LDADD = -lmupnp ../../../../lib/unix/libmupnpav.la
else
cdms_filesys_LDADD = -lmupnp ../../../../lib/unix/libmupnpav.a
endif
