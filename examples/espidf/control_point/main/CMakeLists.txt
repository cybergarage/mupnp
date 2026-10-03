# EXTRA_COMPONENT_DIRS uses the checkout directory's basename as its name.
get_filename_component(MUPNP_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
get_filename_component(MUPNP_COMPONENT_NAME "${MUPNP_ROOT}" NAME)
idf_component_register(SRCS "control_point.c"
  INCLUDE_DIRS "."
  REQUIRES ${MUPNP_COMPONENT_NAME} esp_wifi esp_event esp_netif nvs_flash)
