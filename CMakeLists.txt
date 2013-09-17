cmake_minimum_required(VERSION 2.6)

project(CyberLinkForC)

# TODO: Support options
# option(CG_HTTP_CURL "Enable Libcurl support" OFF)
# option(CG_HTTP_USE_PERSISTENT_CONNECTIONS "Enable persistent connections support" ON)
# option(CG_NET_USE_ANYADDR "Enables the usage of IPV4_ADDRANY instead of registering an explicit server for each interface" ON)
# option(CG_OPTIMIZED_CP_MODE "Enable optimized control point mode" OFF)
# option(CG_USE_CFILE "Enable file functions" OFF)
# option(CG_USE_OPENSSL "Enable OpenSSL support" OFF)
# option(CG_XMLPARSER_LIBXML2 "Enable Libxml2 support" OFF)

if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif(MSVC)

set(CLINKC_HDRS
  include/cybergarage/http/chttp.h
  include/cybergarage/io/cfile.h
  include/cybergarage/net/cinterface.h
  include/cybergarage/net/csocket.h
  include/cybergarage/net/curi.h
  include/cybergarage/net/curl.h
  include/cybergarage/soap/csoap.h
  include/cybergarage/typedef.h
  include/cybergarage/upnp/caction.h
  include/cybergarage/upnp/cargument.h
  include/cybergarage/upnp/ccontrolpoint.h
  include/cybergarage/upnp/cdevice.h
  include/cybergarage/upnp/cicon.h
  include/cybergarage/upnp/control/ccontrol.h
  include/cybergarage/upnp/cservice.h
  include/cybergarage/upnp/cstatevariable.h
  include/cybergarage/upnp/cupnp.h
  include/cybergarage/upnp/cupnp_function.h
  include/cybergarage/upnp/cupnp_limit.h
  include/cybergarage/upnp/cupnp_status.h
  include/cybergarage/upnp/event/cevent.h
  include/cybergarage/upnp/event/cnotify.h
  include/cybergarage/upnp/event/cproperty.h
  include/cybergarage/upnp/event/csubscriber.h
  include/cybergarage/upnp/event/csubscription.h
  include/cybergarage/upnp/ssdp/cssdp.h
  include/cybergarage/upnp/ssdp/cssdp_server.h
  include/cybergarage/util/ccond.h
  include/cybergarage/util/cdictionary.h
  include/cybergarage/util/clist.h
  include/cybergarage/util/clog.h
  include/cybergarage/util/cmutex.h
  include/cybergarage/util/cstring.h
  include/cybergarage/util/cthread.h
  include/cybergarage/util/ctime.h
  include/cybergarage/xml/cxml.h
  )
set(CLINKC_SRCS
  src/cybergarage/http/chttp.c
  src/cybergarage/http/chttp_date.c
  src/cybergarage/http/chttp_header.c
  src/cybergarage/http/chttp_header_list.c
  src/cybergarage/http/chttp_packet.c
  src/cybergarage/http/chttp_persistent_connection.c
  src/cybergarage/http/chttp_request.c
  src/cybergarage/http/chttp_response.c
  src/cybergarage/http/chttp_server.c
  src/cybergarage/http/chttp_server_list.c
  src/cybergarage/io/cfile.c
  src/cybergarage/io/cfile_list.c
  src/cybergarage/net/cdatagram_packet.c
  src/cybergarage/net/cinterface.c
  src/cybergarage/net/cinterface_function.c
  src/cybergarage/net/cinterface_list.c
  src/cybergarage/net/cnet_function.c
  src/cybergarage/net/csocket.c
  src/cybergarage/net/csocket_list.c
  src/cybergarage/net/curi.c
  src/cybergarage/net/curl.c
  src/cybergarage/soap/csoap.c
  src/cybergarage/soap/csoap_request.c
  src/cybergarage/soap/csoap_response.c
  src/cybergarage/upnp/caction.c
  src/cybergarage/upnp/caction_list.c
  src/cybergarage/upnp/cargument.c
  src/cybergarage/upnp/cargument_list.c
  src/cybergarage/upnp/ccontrolpoint.c
  src/cybergarage/upnp/ccontrolpoint_event.c
  src/cybergarage/upnp/ccontrolpoint_http_server.c
  src/cybergarage/upnp/cdevice.c
  src/cybergarage/upnp/cdevice_advertiser.c
  src/cybergarage/upnp/cdevice_http_server.c
  src/cybergarage/upnp/cdevice_list.c
  src/cybergarage/upnp/cdevice_ssdp_server.c
  src/cybergarage/upnp/cicon.c
  src/cybergarage/upnp/cicon_list.c
  src/cybergarage/upnp/control/caction_ctrl.c
  src/cybergarage/upnp/control/caction_request.c
  src/cybergarage/upnp/control/caction_response.c
  src/cybergarage/upnp/control/ccontrol_function.c
  src/cybergarage/upnp/control/cquery_ctrl.c
  src/cybergarage/upnp/control/cquery_request.c
  src/cybergarage/upnp/control/cquery_response.c
  src/cybergarage/upnp/cservice.c
  src/cybergarage/upnp/cservice_list.c
  src/cybergarage/upnp/cservice_notify.c
  src/cybergarage/upnp/cservice_ssdp_server.c
  src/cybergarage/upnp/cservicestate_table.c
  src/cybergarage/upnp/cstatevariable.c
  src/cybergarage/upnp/cupnp_function.c
  src/cybergarage/upnp/cupnp_status.c
  src/cybergarage/upnp/event/ceventlistener_list.c
  src/cybergarage/upnp/event/cnotify_request.c
  src/cybergarage/upnp/event/cproperty.c
  src/cybergarage/upnp/event/cproperty_list.c
  src/cybergarage/upnp/event/csubscriber.c
  src/cybergarage/upnp/event/csubscriber_list.c
  src/cybergarage/upnp/event/csubscription.c
  src/cybergarage/upnp/event/csubscription_request.c
  src/cybergarage/upnp/event/csubscription_response.c
  src/cybergarage/upnp/ssdp/chttpmu_socket.c
  src/cybergarage/upnp/ssdp/chttpu_socket.c
  src/cybergarage/upnp/ssdp/cssdp.c
  src/cybergarage/upnp/ssdp/cssdp_packet.c
  src/cybergarage/upnp/ssdp/cssdp_request.c
  src/cybergarage/upnp/ssdp/cssdp_response.c
  src/cybergarage/upnp/ssdp/cssdp_response_server.c
  src/cybergarage/upnp/ssdp/cssdp_response_server_list.c
  src/cybergarage/upnp/ssdp/cssdp_server.c
  src/cybergarage/upnp/ssdp/cssdp_server_list.c
  src/cybergarage/upnp/ssdp/cssdp_socket.c
  src/cybergarage/util/ccond.c
  src/cybergarage/util/cdictionary.c
  src/cybergarage/util/cdictionary_elem.c
  src/cybergarage/util/clist.c
  src/cybergarage/util/clog.c
  src/cybergarage/util/cmutex.c
  src/cybergarage/util/cstring.c
  src/cybergarage/util/cstring_function.c
  src/cybergarage/util/cstring_tokenizer.c
  src/cybergarage/util/cthread.c
  src/cybergarage/util/cthread_list.c
  src/cybergarage/util/ctime.c
  src/cybergarage/xml/cxml_attribute.c
  src/cybergarage/xml/cxml_attribute_list.c
  src/cybergarage/xml/cxml_function.c
  src/cybergarage/xml/cxml_node.c
  src/cybergarage/xml/cxml_node_list.c
  src/cybergarage/xml/cxml_parser.c
  src/cybergarage/xml/cxml_parser_expat.c
  src/cybergarage/xml/cxml_parser_libxml2.c
  )

include_directories(include)
add_library(clinkc STATIC ${CLINKC_SRCS})

if(NOT WIN32)
  find_library(LIBUUID_LIBRARY uuid)
  find_path(LIBUUID_INCLUDE_DIR uuid/uuid.h)
  # TODO: try_compile() for HAVE_UUID_UNPARSE_LOWER
  add_definitions(
    -DHAVE_LIBUUID
    -DHAVE_UUID_UNPARSE_LOWER
    -DHAVE_UUID_UUID_H
    )
  include_directories(${LIBUUID_INCLUDE_DIR})
  target_link_libraries(clinkc ${LIBUUID_LIBRARY})
endif(NOT WIN32)

find_library(LIBEXPAT_LIBRARY expat)
find_path(LIBEXPAT_INCLUDE_DIR expat.h PATH_SUFFIXES expat)
include_directories(${LIBEXPAT_INCLUDE_DIR})
target_link_libraries(clinkc ${LIBEXPAT_LIBRARY})

# TODO: add_subdirectory(sample)

# FIXME: Headers subdirectories
install(FILES ${CLINKC_HDRS} DESTINATION include)
install(TARGETS clinkc ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
