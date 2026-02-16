# ESP-IDF Component CMakeLists for mUPnP
#
# This file allows mUPnP to be built as an ESP-IDF component

# Collect all mupnp source files
file(GLOB_RECURSE MUPNP_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/../../src/mupnp/*.c"
)

# Remove XML parser sources that are not needed (we'll use expat)
list(FILTER MUPNP_SRCS EXCLUDE REGEX ".*xml_parser_libxml2\\.c$")

# Platform-specific sources for ESP32
set(ESP32_PLATFORM_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/thread.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/mutex.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/cond.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/time.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/log.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/socket.c"
    "${CMAKE_CURRENT_LIST_DIR}/platform/esp32/interface.c"
)

# Check if platform sources exist, if not use default ones
foreach(src ${ESP32_PLATFORM_SRCS})
    if(NOT EXISTS ${src})
        message(STATUS "Platform source not found: ${src}, using default implementation")
        # Remove from list if doesn't exist
        list(REMOVE_ITEM ESP32_PLATFORM_SRCS ${src})
    endif()
endforeach()

# Combine all sources
set(COMPONENT_SRCS ${MUPNP_SRCS} ${ESP32_PLATFORM_SRCS})

# Public include directories
set(COMPONENT_ADD_INCLUDEDIRS
    "../../include"
    "../../include/mupnp"
    "platform/esp32"
)

# Private includes for internal headers
set(COMPONENT_PRIV_INCLUDEDIRS
    "../../src"
)

# Required ESP-IDF components
set(COMPONENT_REQUIRES
    lwip
    esp_timer
    nvs_flash
    esp_netif
    esp_wifi
    expat
)

# Optional components
set(COMPONENT_PRIV_REQUIRES
    )

# Register component
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
    PRIV_INCLUDE_DIRS ${COMPONENT_PRIV_INCLUDEDIRS}
    REQUIRES ${COMPONENT_REQUIRES}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
)

# Add compile definitions for ESP32 platform
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    MUPNP_USE_OPENSSL=0
    MUPNP_NOUSE_QUERYCTRL=1
    HAVE_SOCKLEN_T=1
    HAVE_STDBOOL_H=1
    ESP32=1
)

# Set C standard
target_compile_options(${COMPONENT_LIB} PRIVATE
    -std=gnu99
    -Wno-format
    -Wno-pointer-sign
)
