############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

#if HAVE_LIBTOOL
#lib_LTLIBRARIES = libmupnp.la
#else
lib_LIBRARIES = libmupnp.a
#endif

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include @XML_CFLAGS@ @HTTP_CFLAGS@ @CLOG_DEBUG_FLAGS@

mupnp_sources = \
	../../src/mupnp/xml/xml_parser.c \
	../../src/mupnp/xml/xml_parser_expat.c \
	../../src/mupnp/xml/xml_node.c \
	../../src/mupnp/xml/xml_attribute_list.c \
	../../src/mupnp/xml/xml_parser_libxml2.c \
	../../src/mupnp/xml/xml_function.c \
	../../src/mupnp/xml/xml_attribute.c \
	../../src/mupnp/xml/xml_node_list.c \
	../../src/mupnp/service_list.c \
	../../src/mupnp/action_list.c \
	../../src/mupnp/argument_list.c \
	../../src/mupnp/argument.c \
	../../src/mupnp/controlpoint_http_server.c \
	../../src/mupnp/icon_list.c \
	../../src/mupnp/upnp_status.c \
	../../src/mupnp/http/http_response.c \
	../../src/mupnp/http/http_packet.c \
	../../src/mupnp/http/http_persistent_connection.c \
	../../src/mupnp/http/http_server_list.c \
	../../src/mupnp/http/http_date.c \
	../../src/mupnp/http/http_server.c \
	../../src/mupnp/http/http_header_list.c \
	../../src/mupnp/http/http_header.c \
	../../src/mupnp/http/http.c \
	../../src/mupnp/http/http_request.c \
	../../src/mupnp/service_notify.c \
	../../src/mupnp/servicestate_table.c \
	../../src/mupnp/net/datagram_packet.c \
	../../src/mupnp/net/net_function.c \
	../../src/mupnp/net/interface.c \
	../../src/mupnp/net/url.c \
	../../src/mupnp/net/uri.c \
	../../src/mupnp/net/interface_function.c \
	../../src/mupnp/net/socket.c \
	../../src/mupnp/net/interface_list.c \
	../../src/mupnp/net/socket_list.c \
	../../src/mupnp/upnp_function.c \
	../../src/mupnp/device.c \
	../../src/mupnp/statevariable.c \
	../../src/mupnp/controlpoint.c \
	../../src/mupnp/device_list.c \
	../../src/mupnp/io/file_list.c \
	../../src/mupnp/io/file.c \
	../../src/mupnp/action.c \
	../../src/mupnp/ssdp/ssdp.c \
	../../src/mupnp/ssdp/ssdp_server.c \
	../../src/mupnp/ssdp/ssdp_request.c \
	../../src/mupnp/ssdp/ssdp_response.c \
	../../src/mupnp/ssdp/ssdp_response_server.c \
	../../src/mupnp/ssdp/httpmu_socket.c \
	../../src/mupnp/ssdp/ssdp_socket.c \
	../../src/mupnp/ssdp/ssdp_server_list.c \
	../../src/mupnp/ssdp/ssdp_response_server_list.c \
	../../src/mupnp/ssdp/httpu_socket.c \
	../../src/mupnp/ssdp/ssdp_packet.c \
	../../src/mupnp/service_ssdp_server.c \
	../../src/mupnp/device_advertiser.c \
	../../src/mupnp/control/action_request.c \
	../../src/mupnp/control/query_request.c \
	../../src/mupnp/control/query_response.c \
	../../src/mupnp/control/action_ctrl.c \
	../../src/mupnp/control/query_ctrl.c \
	../../src/mupnp/control/control_function.c \
	../../src/mupnp/control/action_response.c \
	../../src/mupnp/event/eventlistener_list.c \
	../../src/mupnp/event/subscriber_list.c \
	../../src/mupnp/event/subscriber.c \
	../../src/mupnp/event/subscription_request.c \
	../../src/mupnp/event/notify_request.c \
	../../src/mupnp/event/subscription_response.c \
	../../src/mupnp/event/subscription.c \
	../../src/mupnp/event/property_list.c \
	../../src/mupnp/event/property.c \
	../../src/mupnp/device_http_server.c \
	../../src/mupnp/service_std_dcp.c \
	../../src/mupnp/soap/soap_request.c \
	../../src/mupnp/soap/soap.c \
	../../src/mupnp/soap/soap_response.c \
	../../src/mupnp/util/thread.c \
	../../src/mupnp/util/thread_list.c \
	../../src/mupnp/util/string_tokenizer.c \
	../../src/mupnp/util/mutex.c \
	../../src/mupnp/util/list.c \
	../../src/mupnp/util/time_wince.c \
	../../src/mupnp/util/dictionary_elem.c \
	../../src/mupnp/util/dictionary.c \
	../../src/mupnp/util/string_function.c \
	../../src/mupnp/util/log.c \
	../../src/mupnp/util/cond.c \
	../../src/mupnp/util/time.c \
	../../src/mupnp/util/string.c \
	../../src/mupnp/device_ssdp_server.c \
	../../src/mupnp/service.c \
	../../src/mupnp/icon.c \
	../../src/mupnp/controlpoint_event.c

libmupnpincludedir = $(includedir)/mupnp
nobase_libmupnpinclude_HEADERS =  \
        $(mupnp_headers)

#if HAVE_LIBTOOL
#libmupnp_la_SOURCES = \
#        $(mupnp_source)
#libmupnp_la_LIBADD = @HTTP_LIBS@ @XML_LIBS@
#else
libmupnp_a_SOURCES = \
        $(mupnp_sources)
libmupnp_a_CFLAGS = $(AM_CFLAGS)
#libmupnp_a_LIBADD = @HTTP_LIBS@ @XML_LIBS@
#endif
