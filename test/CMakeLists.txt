enable_language(CXX)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(mupnptest
  TestMain.cpp
  NetworkTest.cpp
  SubscriptionTest.cpp
  ServiceTest.cpp
  StringTest.cpp
  TestDevice.c
  DeviceTest.cpp
  XMLTest.cpp
  ControlPointTest.cpp
  DictionaryTest.cpp
  HttpTest.cpp
  StateVariableTest.cpp
  URITest.cpp
  UPnPFuncTest.cpp
  LocationChangeTest.cpp
)

target_include_directories(mupnptest PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries(mupnptest PRIVATE
  MUPNP
  ${Boost_LIBRARIES}
)

add_test(NAME mupnptest COMMAND mupnptest)

# Allocation-failure injection (GNU ld --wrap). Fails the N-th allocation made
# by mUPnP for every N a successful parse needs.
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(mupnp_alloc_failure_test XMLAllocFailureTest.c)
  target_include_directories(mupnp_alloc_failure_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(mupnp_alloc_failure_test PRIVATE MUPNP)
  target_link_options(mupnp_alloc_failure_test PRIVATE
    "LINKER:--wrap=malloc" "LINKER:--wrap=realloc")
  add_test(NAME mupnp_alloc_failure_test COMMAND mupnp_alloc_failure_test)
endif()
