/******************************************************************
 *
 * mUPnP for C - ESP32 Control Point Example
 *
 * Copyright (C) Satoshi Konno 2005
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include <mupnp/upnp.h>

/* WiFi configuration - override with your credentials */
#ifndef CONFIG_ESP_WIFI_SSID
#define CONFIG_ESP_WIFI_SSID      "your-ssid"
#endif

#ifndef CONFIG_ESP_WIFI_PASSWORD
#define CONFIG_ESP_WIFI_PASSWORD  "your-password"
#endif

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "upnp_cp";
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static mUpnpControlPoint* ctrlPoint = NULL;

/****************************************
 * WiFi Event Handler
 ****************************************/

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/****************************************
 * WiFi Initialization
 ****************************************/

static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/****************************************
 * UPnP Device Listener
 ****************************************/

static void device_listener(mUpnpDevice* dev)
{
    char* devName;
    char* devType;
    char* location;
    
    devName = (char*)mupnp_device_getfriendlyname(dev);
    devType = (char*)mupnp_device_getdevicetype(dev);
    location = (char*)mupnp_device_getlocation(dev);
    
    ESP_LOGI(TAG, "Found device:");
    ESP_LOGI(TAG, "  Name: %s", devName ? devName : "(null)");
    ESP_LOGI(TAG, "  Type: %s", devType ? devType : "(null)");
    ESP_LOGI(TAG, "  Location: %s", location ? location : "(null)");
}

/****************************************
 * UPnP Control Point Task
 ****************************************/

static void upnp_control_point_task(void* pvParameters)
{
    ESP_LOGI(TAG, "Starting UPnP Control Point...");
    
    /* Create control point */
    ctrlPoint = mupnp_controlpoint_new();
    if (!ctrlPoint) {
        ESP_LOGE(TAG, "Failed to create control point");
        vTaskDelete(NULL);
        return;
    }
    
    /* Start control point */
    if (!mupnp_controlpoint_start(ctrlPoint)) {
        ESP_LOGE(TAG, "Failed to start control point");
        mupnp_controlpoint_delete(ctrlPoint);
        vTaskDelete(NULL);
        return;
    }
    
    ESP_LOGI(TAG, "Control Point started successfully");
    
    /* Search for all devices periodically */
    while (1) {
        ESP_LOGI(TAG, "Searching for UPnP devices...");
        
        /* Perform M-SEARCH for all devices */
        if (mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE)) {
            /* Wait for responses */
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            
            /* Print discovered devices */
            mUpnpDevice* dev;
            int deviceCount = 0;
            
            for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; 
                 dev = mupnp_device_next(dev)) {
                device_listener(dev);
                deviceCount++;
            }
            
            ESP_LOGI(TAG, "Total devices found: %d", deviceCount);
        } else {
            ESP_LOGE(TAG, "M-SEARCH failed");
        }
        
        /* Wait before next search */
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
    
    /* Cleanup (never reached in this example) */
    mupnp_controlpoint_stop(ctrlPoint);
    mupnp_controlpoint_delete(ctrlPoint);
    vTaskDelete(NULL);
}

/****************************************
 * Main Application Entry
 ****************************************/

void app_main(void)
{
    ESP_LOGI(TAG, "mUPnP Control Point Example for ESP32");
    
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    /* Initialize WiFi */
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init_sta();
    
    /* Start UPnP Control Point task */
    xTaskCreate(upnp_control_point_task, "upnp_cp", 8192, NULL, 5, NULL);
}
