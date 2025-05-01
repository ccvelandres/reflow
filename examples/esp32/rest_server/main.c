#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "rest_server";

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void init_softap(const char* ssid, const char *password, uint8_t channel, uint8_t max_conn)
{
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    //                                                     ESP_EVENT_ANY_ID,
    //                                                     NULL,
    //                                                     NULL,
    //                                                     NULL));

    wifi_config_t wifi_config = {
        .ap = {
            // .ssid = ssid,
            // .ssid_len = strlen(ssid),
            .channel = channel,
            // .password = psk,
            .max_connection = max_conn,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
#ifdef CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT
            .bss_max_idle_cfg = {
                .period = WIFI_AP_DEFAULT_MAX_IDLE_PERIOD,
                .protected_keep_alive = 1,
            },
#endif
        },
    };

    strncpy(wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid));
    strncpy(wifi_config.ap.password, password, sizeof(wifi_config.ap.password));
    wifi_config.ap.ssid_len = strlen(ssid);

    if (strlen(password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ssid, password, channel);
}

#include "esp_http_server.h"

esp_err_t http_handler_assets(httpd_req_t *req)
{
    extern unsigned char __binary_CMakeLists_txt_data[];
    extern unsigned __binary_CMakeLists_txt_size;

    httpd_resp_send(req, __binary_CMakeLists_txt_data, __binary_CMakeLists_txt_size);
    // httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


static const httpd_uri_t http_handler_assets_uri = 
{
    .uri = "/assets",
    .method = HTTP_GET,
    .handler = http_handler_assets,
    .user_ctx = NULL
};

httpd_handle_t init_http_server()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &http_handler_assets_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


int app_main()
{
    httpd_handle_t server = NULL;

    init_nvs();
    init_softap("esp32-rest-server", "", 1, 4);
    init_http_server();
    
    while (server) {
        sleep(5);
    }
}