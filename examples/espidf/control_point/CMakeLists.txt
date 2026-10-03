cmake_minimum_required(VERSION 3.16)

# Build the repository root as an ESP-IDF component.
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../..")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mupnp_control_point)
