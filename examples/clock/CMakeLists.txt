add_executable(upnpclock
  unix/clock_main.c
  clock.c
  clock_device.c
  ../common/kb_util.c
)

target_include_directories(upnpclock PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(upnpclock PRIVATE MUPNP)
