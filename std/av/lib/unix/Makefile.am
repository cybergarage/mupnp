#if HAVE_LIBTOOL
#lib_LTLIBRARIES = libmupnpav.la
#else
lib_LIBRARIES = libmupnpav.a
#endif

AM_CPPFLAGS = -I$(top_srcdir)/include

########################################
# Sources
########################################

clinkcav_source = ../../src/cybergarage/upnp/std/av/ccontent.c \
	../../src/cybergarage/upnp/std/av/ccontent_list.c \
	../../src/cybergarage/upnp/std/av/cdidl.c \
	../../src/cybergarage/upnp/std/av/cmd5.c \
	../../src/cybergarage/upnp/std/av/cresource.c \
	../../src/cybergarage/upnp/std/av/cresource_list.c \
	../../src/cybergarage/upnp/std/av/cresource_data.c \
	../../src/cybergarage/upnp/std/av/server/cconnectionmgr_service.c \
	../../src/cybergarage/upnp/std/av/server/ccontentdir_service.c \
	../../src/cybergarage/upnp/std/av/server/cmediaserver_device.c \
	../../src/cybergarage/upnp/std/av/cprotocolinfo_list.c \
	../../src/cybergarage/upnp/std/av/cprotocolinfo.c \
    ../../src/cybergarage/upnp/std/av/renderer/cavtransport_service.c \
    ../../src/cybergarage/upnp/std/av/renderer/cmediarenderer_device.c \
    ../../src/cybergarage/upnp/std/av/renderer/crenderingcontrol_service.c \
    ../../src/cybergarage/upnp/std/av/renderer/cconnectionmgrr_service.c

########################################
# Library
########################################

libmupnpavincludedir = $(includedir)/cybergarage
nobase_libmupnpavinclude_HEADERS =  \
        $(clinkcav_header)

#if HAVE_LIBTOOL
#libmupnpav_la_SOURCES = \
#        $(clinkcav_source)
#libmupnpav_a_CFLAGS = $(AM_CFLAGS)
#libmupnpav_la_LIBADD =
#else
libmupnpav_a_SOURCES = \
        $(clinkcav_source)
libmupnpav_a_CFLAGS = $(AM_CFLAGS)
libmupnpav_a_LIBADD =
#endif
