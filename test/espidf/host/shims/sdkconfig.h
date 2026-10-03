#ifndef MUPNP_TEST_SDKCONFIG_H
#define MUPNP_TEST_SDKCONFIG_H
/* Host sanitizers require more stack than the ESP-IDF runtime. */
#define CONFIG_MUPNP_THREAD_STACK_SIZE (1024 * 1024)
#endif
