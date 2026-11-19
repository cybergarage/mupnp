add_executable(upnpavchk upnpavchk.c)
target_link_libraries(upnpavchk PRIVATE MUPNP)

if(MUPNP_ENABLE_PLATFORM_SMOKE)
  add_test(NAME avchk_help COMMAND upnpavchk --help)
  add_test(NAME avchk_invalid_arguments COMMAND upnpavchk --invalid-option)
  set_tests_properties(avchk_invalid_arguments PROPERTIES WILL_FAIL TRUE)
  set_tests_properties(avchk_help avchk_invalid_arguments PROPERTIES TIMEOUT 10)
endif()
