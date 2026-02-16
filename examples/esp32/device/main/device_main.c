/******************************************************************
 *
 * mUPnP for C - ESP32 Device Example
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
#include "esp_mac.h"

#include <mupnp/upnp.h>

/* WiFi configuration */
#ifndef CONFIG_ESP_WIFI_SSID
#define CONFIG_ESP_WIFI_SSID      "your-ssid"
#endif

#ifndef CONFIG_ESP_WIFI_PASSWORD
#define CONFIG_ESP_WIFI_PASSWORD  "your-password"
#endif

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "upnp_device";
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static mUpnpDevice* device = NULL;

/* Device description XML */
static const char* DEVICE_DESCRIPTION = 
"<?xml version=\"1.0\"?>\n"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
"  <specVersion>\n"
"    <major>1</major>\n"
"    <minor>0</minor>\n"
"  </specVersion>\n"
"  <device>\n"
"    <deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>\n"
"    <friendlyName>ESP32 mUPnP Device</friendlyName>\n"
"    <manufacturer>ESP32</manufacturer>\n"
"    <manufacturerURL>http://www.espressif.com</manufacturerURL>\n"
"    <modelDescription>mUPnP Basic Device on ESP32</modelDescription>\n"
"    <modelName>ESP32 UPnP Device</modelName>\n"
"    <modelNumber>1.0</modelNumber>\n"
"    <modelURL>http://www.espressif.com</modelURL>\n"
"    <serialNumber>12345678</serialNumber>\n"
"    <UDN>uuid:esp32-upnp-device-12345678</UDN>\n"
"    <serviceList>\n"
"      <service>\n"
"        <serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>\n"
"        <serviceId>urn:upnp-org:serviceId:SwitchPower1</serviceId>\n"
"        <SCPDURL>/service/switchpower/description.xml</SCPDURL>\n"
"        <controlURL>/service/switchpower/control</controlURL>\n"
"        <eventSubURL>/service/switchpower/eventSub</eventSubURL>\n"
"      </service>\n"
"    </serviceList>\n"
"  </device>\n"
"</root>\n";

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

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s", CONFIG_ESP_WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", CONFIG_ESP_WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/****************************************
 * UPnP Action Request Handler
 ****************************************/

static bool action_received(mUpnpAction* action)
{
    char* actionName = (char*)mupnp_action_getname(action);
    
    ESP_LOGI(TAG, "Action received: %s", actionName ? actionName : "(null)");
    
    /* Handle SetTarget action */
    if (mupnp_streq(actionName, "SetTarget")) {
        mUpnpArgument* arg = mupnp_action_getargumentbyname(action, "newTargetValue");
        if (arg) {
            char* value = mupnp_argument_getvalue(arg);
            ESP_LOGI(TAG, "SetTarget value: %s", value ? value : "(null)");
            
            /* Update the state variable */
            mUpnpService* service = mupnp_action_getservice(action);
            if (service) {
                mUpnpStateVariable* stateVar = mupnp_service_getstatevariablebyname(service, "Target");
                if (stateVar) {
                    mupnp_statevariable_setvalue(stateVar, value);
                }
            }
        }
        return true;
    }
    
    /* Handle GetTarget action */
    if (mupnp_streq(actionName, "GetTarget")) {
        mUpnpService* service = mupnp_action_getservice(action);
        if (service) {
            mUpnpStateVariable* stateVar = mupnp_service_getstatevariablebyname(service, "Target");
            if (stateVar) {
                char* value = mupnp_statevariable_getvalue(stateVar);
                mUpnpArgument* retArg = mupnp_action_getargumentbyname(action, "RetTargetValue");
                if (retArg) {
                    mupnp_argument_setvalue(retArg, value ? value : "0");
                }
            }
        }
        return true;
    }
    
    return false;
}

/****************************************
 * UPnP Device Task
 ****************************************/

static void upnp_device_task(void* pvParameters)
{
    ESP_LOGI(TAG, "Starting UPnP Device...");
    
    /* Create device from description */
    device = mupnp_device_new();
    if (!device) {
        ESP_LOGE(TAG, "Failed to create device");
        vTaskDelete(NULL);
        return;
    }
    
    /* Load device description */
    if (!mupnp_device_parsedescription(device, DEVICE_DESCRIPTION, strlen(DEVICE_DESCRIPTION))) {
        ESP_LOGE(TAG, "Failed to parse device description");
        mupnp_device_delete(device);
        vTaskDelete(NULL);
        return;
    }
    
    /* Set action listener */
    mupnp_device_setactionlistener(device, action_received);
    
    /* Start device */
    if (!mupnp_device_start(device)) {
        ESP_LOGE(TAG, "Failed to start device");
        mupnp_device_delete(device);
        vTaskDelete(NULL);
        return;
    }
    
    ESP_LOGI(TAG, "UPnP Device started successfully");
    ESP_LOGI(TAG, "Device location: %s", mupnp_device_getlocation(device));
    
    /* Keep running */
    while (1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Device is running...");
    }
    
    /* Cleanup (never reached) */
    mupnp_device_stop(device);
    mupnp_device_delete(device);
    vTaskDelete(NULL);
}

/****************************************
 * Main Application Entry
 ****************************************/

void app_main(void)
{
    ESP_LOGI(TAG, "mUPnP Device Example for ESP32");
    
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
    
    /* Start UPnP Device task */
    xTaskCreate(upnp_device_task, "upnp_dev", 8192, NULL, 5, NULL);
}
