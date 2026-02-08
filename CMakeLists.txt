cmake_minimum_required(VERSION 2.6)

project(mUPnP LANGUAGES C)

#
# Build options
#

option(CG_HTTP_CURL "Enable Libcurl support" OFF)
option(CG_HTTP_USE_PERSISTENT_CONNECTIONS
  "Enable persistent connections support" ON)
option(CG_NET_USE_ANYADDR
  "Enables the usage of IPV4_ADDRANY instead of registering an explicit server for each interface" ON)
option(CG_OPTIMIZED_CP_MODE "Enable optimized control point mode" OFF)
option(CG_USE_CFILE "Enable file functions" OFF)
option(CG_USE_OPENSSL "Enable OpenSSL support" OFF)
option(CG_XMLPARSER_LIBXML2 "Use Libxml2 instead of Expat for XML parsing" OFF)

foreach(OPT
    CG_HTTP_CURL CG_HTTP_USE_PERSISTENT_CONNECTIONS CG_NET_USE_ANYADDR
    CG_OPTIMIZED_CP_MODE CG_USE_CFILE CG_USE_OPENSSL CG_XMLPARSER_LIBXML2)
  if(${OPT})
    add_definitions(-D${OPT})
  endif(${OPT})
endforeach()

#
# Compiler related
#

if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif(MSVC)

#
# Sources and targets
#

set(MUPNP_SRCS
  src/mupnp/http/http.c
  src/mupnp/http/http_date.c
  src/mupnp/http/http_header.c
  src/mupnp/http/http_header_list.c
  src/mupnp/http/http_packet.c
  src/mupnp/http/http_persistent_connection.c
  src/mupnp/http/http_request.c
  src/mupnp/http/http_response.c
  src/mupnp/http/http_server.c
  src/mupnp/http/http_server_list.c
  src/mupnp/io/file.c
  src/mupnp/io/file_list.c
  src/mupnp/net/datagram_packet.c
  src/mupnp/net/interface.c
  src/mupnp/net/interface_function.c
  src/mupnp/net/interface_list.c
  src/mupnp/net/net_function.c
  src/mupnp/net/socket.c
  src/mupnp/net/socket_list.c
  src/mupnp/net/uri.c
  src/mupnp/net/url.c
  src/mupnp/soap/soap.c
  src/mupnp/soap/soap_request.c
  src/mupnp/soap/soap_response.c
  src/mupnp/action.c
  src/mupnp/action_list.c
  src/mupnp/argument.c
  src/mupnp/argument_list.c
  src/mupnp/controlpoint.c
  src/mupnp/controlpoint_event.c
  src/mupnp/controlpoint_http_server.c
  src/mupnp/device.c
  src/mupnp/device_advertiser.c
  src/mupnp/device_http_server.c
  src/mupnp/device_list.c
  src/mupnp/device_ssdp_server.c
  src/mupnp/icon.c
  src/mupnp/icon_list.c
  src/mupnp/control/action_ctrl.c
  src/mupnp/control/action_request.c
  src/mupnp/control/action_response.c
  src/mupnp/control/control_function.c
  src/mupnp/control/query_ctrl.c
  src/mupnp/control/query_request.c
  src/mupnp/control/query_response.c
  src/mupnp/service.c
  src/mupnp/service_list.c
  src/mupnp/service_notify.c
  src/mupnp/service_ssdp_server.c
  src/mupnp/servicestate_table.c
  src/mupnp/statevariable.c
  src/mupnp/upnp_function.c
  src/mupnp/upnp_status.c
  src/mupnp/event/eventlistener_list.c
  src/mupnp/event/notify_request.c
  src/mupnp/event/property.c
  src/mupnp/event/property_list.c
  src/mupnp/event/subscriber.c
  src/mupnp/event/subscriber_list.c
  src/mupnp/event/subscription.c
  src/mupnp/event/subscription_request.c
  src/mupnp/event/subscription_response.c
  src/mupnp/ssdp/httpmu_socket.c
  src/mupnp/ssdp/httpu_socket.c
  src/mupnp/ssdp/ssdp.c
  src/mupnp/ssdp/ssdp_packet.c
  src/mupnp/ssdp/ssdp_request.c
  src/mupnp/ssdp/ssdp_response.c
  src/mupnp/ssdp/ssdp_response_server.c
  src/mupnp/ssdp/ssdp_response_server_list.c
  src/mupnp/ssdp/ssdp_server.c
  src/mupnp/ssdp/ssdp_server_list.c
  src/mupnp/ssdp/ssdp_socket.c
  src/mupnp/util/cond.c
  src/mupnp/util/dictionary.c
  src/mupnp/util/dictionary_elem.c
  src/mupnp/util/list.c
  src/mupnp/util/log.c
  src/mupnp/util/mutex.c
  src/mupnp/util/string.c
  src/mupnp/util/string_function.c
  src/mupnp/util/string_tokenizer.c
  src/mupnp/util/thread.c
  src/mupnp/util/thread_list.c
  src/mupnp/util/time.c
  src/mupnp/xml/xml_attribute.c
  src/mupnp/xml/xml_attribute_list.c
  src/mupnp/xml/xml_function.c
  src/mupnp/xml/xml_node.c
  src/mupnp/xml/xml_node_list.c
  src/mupnp/xml/xml_parser.c
  src/mupnp/xml/xml_parser_expat.c
  src/mupnp/xml/xml_parser_libxml2.c
  )

include_directories(include)
add_library(MUPNP STATIC ${MUPNP_SRCS})

# TODO: add_subdirectory(sample)

#
# Dependencies
#

if(NOT WIN32)
  find_library(LIBUUID_LIBRARY uuid)
  find_path(LIBUUID_INCLUDE_DIR uuid/uuid.h)
  if(LIBUUID_LIBRARY AND LIBUUID_INCLUDE_DIR)
    # TODO: try_compile() for HAVE_UUID_UNPARSE_LOWER
    add_definitions(
      -DHAVE_LIBUUID
      -DHAVE_UUID_UNPARSE_LOWER
      -DHAVE_UUID_UUID_H
      )
    include_directories(${LIBUUID_INCLUDE_DIR})
    target_link_libraries(MUPNP ${LIBUUID_LIBRARY})
    message(STATUS "Found libuuid: ${LIBUUID_LIBRARY}")
  else()
    message(WARNING "libuuid not found. Install uuid-dev package for UUID support.")
  endif()
endif(NOT WIN32)

if(CG_HTTP_CURL)
  find_package(CURL REQUIRED)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries(MUPNP ${CURL_LIBRARIES})
endif(CG_HTTP_CURL)

if(CG_USE_OPENSSL)
  find_package(OpenSSL REQUIRED)
  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(MUPNP ${OPENSSL_LIBRARIES})
endif(CG_USE_OPENSSL)

if(CG_XMLPARSER_LIBXML2)
  find_package(LibXml2 REQUIRED)
  add_definitions(${LIBXML2_DEFINITIONS})
  include_directories(${LIBXML2_INCLUDE_DIR})
  target_link_libraries(MUPNP ${LIBXML2_LIBRARIES})
else(CG_XMLPARSER_LIBXML2)
  find_library(LIBEXPAT_LIBRARY expat)
  find_path(LIBEXPAT_INCLUDE_DIR expat.h PATH_SUFFIXES expat)
  include_directories(${LIBEXPAT_INCLUDE_DIR})
  target_link_libraries(MUPNP ${LIBEXPAT_LIBRARY})
endif(CG_XMLPARSER_LIBXML2)

#
# Installation
#

install(DIRECTORY include/mupnp/ DESTINATION include/cybergarage
  FILES_MATCHING PATTERN "*.h")
install(TARGETS MUPNP ARCHIVE DESTINATION lib LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
