add_executable(upnpbinarylight
  unix/binarylight_main.c
  binarylight_device.c
  ../common/kb_util.c
)

target_include_directories(upnpbinarylight PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(upnpbinarylight PRIVATE MUPNP)
