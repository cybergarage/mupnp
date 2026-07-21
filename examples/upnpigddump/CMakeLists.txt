add_executable(upnpigddump
  upnpigddump.c
  ../common/kb_util.c
)

target_include_directories(upnpigddump PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(upnpigddump PRIVATE MUPNP)
