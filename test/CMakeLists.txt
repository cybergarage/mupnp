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
