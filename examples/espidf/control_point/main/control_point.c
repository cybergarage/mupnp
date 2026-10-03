/* ESP-IDF IPv4 Wi-Fi control point. Licensed under the repository's COPYING. */

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <mupnp/upnp.h>

#define WIFI_HAS_IP BIT0
#define NETWORK_CHANGED BIT1

static const char* TAG = "mupnp_example";
static EventGroupHandle_t network_state;

static const char* safe_string(const char* text)
{
  return text ? text : "";
}

/* Listeners run in library worker threads. Keep them short and never stop or
 * delete the control point here: those operations join the same workers. */
static void device_changed(mUpnpControlPoint* cp, const char* udn, mUpnpDeviceStatus status)
{
  const char* change = "removed";
  (void)cp;
  if (status == mUpnpDeviceStatusAdded)
    change = "added";
  else if (status == mUpnpDeviceStatusUpdated)
    change = "updated";
  ESP_LOGI(TAG, "Device %s: %s", change, safe_string(udn));
}

static void ssdp_response(mUpnpSSDPPacket* packet)
{
  ESP_LOGI(TAG, "SSDP response: ST=%s USN=%s LOCATION=%s", safe_string(mupnp_ssdp_packet_getst(packet)), safe_string(mupnp_ssdp_packet_getusn(packet)), safe_string(mupnp_ssdp_packet_getlocation(packet)));
}

static void control_point_worker(void* arg)
{
  mUpnpControlPoint* cp = NULL;
  TickType_t wait_ticks = portMAX_DELAY;
  (void)arg;

  /* ESP-IDF pthread_join uses task notifications internally, so use an event
   * group for network changes rather than this task's notification slot.
   * This is the only task that creates, starts, searches, or destroys cp.
   * The change bit is retained even during slow startup/teardown. A quick
   * disconnect/reconnect therefore still recreates all interface bindings. */
  for (;;) {
    EventBits_t events = xEventGroupWaitBits(network_state, NETWORK_CHANGED, pdTRUE, pdFALSE, wait_ticks);
    if ((events & NETWORK_CHANGED) && cp) {
      ESP_LOGI(TAG, "Network changed; stopping control point");
      mupnp_controlpoint_delete(cp);
      cp = NULL;
    }

    if (!(xEventGroupGetBits(network_state) & WIFI_HAS_IP)) {
      wait_ticks = portMAX_DELAY;
      continue;
    }

    if (!cp) {
      cp = mupnp_controlpoint_new();
      if (cp) {
        mupnp_controlpoint_setdevicelistener(cp, device_changed);
        mupnp_controlpoint_setssdpresponselistener(cp, ssdp_response);
        mupnp_controlpoint_setssdpsearchmx(cp, 3);
        if (!mupnp_controlpoint_start(cp)) {
          ESP_LOGE(TAG, "Control point startup failed; retrying in 5 seconds");
          mupnp_controlpoint_delete(cp);
          cp = NULL;
        }
      }
      if (!cp) {
        wait_ticks = pdMS_TO_TICKS(5000);
        continue;
      }
      ESP_LOGI(TAG, "Control point started");
    }

    /* If the network changed during startup, let the next iteration tear
     * down stale bindings before issuing another search. */
    EventBits_t current = xEventGroupGetBits(network_state);
    if (!(current & WIFI_HAS_IP) || (current & NETWORK_CHANGED)) {
      wait_ticks = 0;
      continue;
    }
    if (!mupnp_controlpoint_search(cp, "upnp:rootdevice"))
      ESP_LOGW(TAG, "M-SEARCH failed");
    else
      ESP_LOGI(TAG, "M-SEARCH sent for upnp:rootdevice");
    wait_ticks = pdMS_TO_TICKS(CONFIG_MUPNP_EXAMPLE_SEARCH_INTERVAL * 1000);
  }
}

static void network_event(void* arg, esp_event_base_t base, int32_t id, void* data)
{
  (void)arg;
  if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
    ESP_ERROR_CHECK(esp_wifi_connect());
  }
  else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    xEventGroupClearBits(network_state, WIFI_HAS_IP);
    xEventGroupSetBits(network_state, NETWORK_CHANGED);
    ESP_LOGW(TAG, "Wi-Fi disconnected; reconnecting");
    ESP_ERROR_CHECK(esp_wifi_connect());
  }
  else if (base == IP_EVENT && id == IP_EVENT_STA_LOST_IP) {
    xEventGroupClearBits(network_state, WIFI_HAS_IP);
    xEventGroupSetBits(network_state, NETWORK_CHANGED);
  }
  else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = data;
    ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&event->ip_info.ip));
    xEventGroupSetBits(network_state, WIFI_HAS_IP | NETWORK_CHANGED);
  }
}

void app_main(void)
{
  if (strlen(CONFIG_MUPNP_EXAMPLE_WIFI_SSID) == 0) {
    ESP_LOGE(TAG, "Set Wi-Fi credentials in menuconfig: mUPnP control point example");
    return;
  }
  if (strlen(CONFIG_MUPNP_EXAMPLE_WIFI_SSID) > 32
      || strlen(CONFIG_MUPNP_EXAMPLE_WIFI_PASSWORD) > 64) {
    ESP_LOGE(TAG, "Wi-Fi SSID/password exceeds the ESP-IDF field size");
    return;
  }

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t* sta = esp_netif_create_default_wifi_sta();
  assert(sta);

  network_state = xEventGroupCreate();
  assert(network_state);
  BaseType_t created = xTaskCreate(control_point_worker, "mupnp_app", 8192, NULL, 5, NULL);
  assert(created == pdPASS);

  wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&init));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, network_event, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, network_event, NULL));

  wifi_config_t wifi = { 0 };
  memcpy(wifi.sta.ssid, CONFIG_MUPNP_EXAMPLE_WIFI_SSID, strlen(CONFIG_MUPNP_EXAMPLE_WIFI_SSID));
  memcpy(wifi.sta.password, CONFIG_MUPNP_EXAMPLE_WIFI_PASSWORD, strlen(CONFIG_MUPNP_EXAMPLE_WIFI_PASSWORD));
  wifi.sta.threshold.authmode = strlen(CONFIG_MUPNP_EXAMPLE_WIFI_PASSWORD)
      ? WIFI_AUTH_WPA2_PSK
      : WIFI_AUTH_OPEN;
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi));
  ESP_ERROR_CHECK(esp_wifi_start());
  /* Keep the demonstration responsive to multicast; evaluate power saving
   * separately for battery-operated products. */
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
}
