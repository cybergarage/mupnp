# ESP-IDF Project CMakeLists for mUPnP Device Example
cmake_minimum_required(VERSION 3.16)

# Set the components to include (mupnp component from parent directory)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mupnp_device)
