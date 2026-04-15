#include "wifi.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include <cstring>

static const char *TAG = "wifi";

void WiFiManager::eventHandler(void *arg, esp_event_base_t base,
                                int32_t event_id, void *event_data)
{
    auto *self = static_cast<WiFiManager *>(arg);

    if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();

    } else if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (self->_retries < MAX_RETRIES) {
            esp_wifi_connect();
            self->_retries++;
            ESP_LOGW(TAG, "Retrying WiFi (%d/%d)...", self->_retries, MAX_RETRIES);
        } else {
            xEventGroupSetBits(self->_eventGroup, FAIL_BIT);
            ESP_LOGE(TAG, "WiFi connection failed");
        }

    } else if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        auto *event = static_cast<ip_event_got_ip_t *>(event_data);
        ESP_LOGI(TAG, "Connected — IP: " IPSTR, IP2STR(&event->ip_info.ip));
        self->_retries = 0;
        xEventGroupSetBits(self->_eventGroup, CONNECTED_BIT);
    }
}

void WiFiManager::connect(const char *ssid, const char *password)
{
    _eventGroup = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, this, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, this, nullptr));

    wifi_config_t wifi_cfg = {};
    strncpy(reinterpret_cast<char *>(wifi_cfg.sta.ssid),     ssid,     sizeof(wifi_cfg.sta.ssid) - 1);
    strncpy(reinterpret_cast<char *>(wifi_cfg.sta.password), password, sizeof(wifi_cfg.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);

    EventBits_t bits = xEventGroupWaitBits(_eventGroup,
        CONNECTED_BIT | FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi ready");
    } else {
        ESP_LOGE(TAG, "WiFi failed — running without network");
    }
}

bool WiFiManager::isConnected() const
{
    if (!_eventGroup) return false;
    return (xEventGroupGetBits(_eventGroup) & CONNECTED_BIT) != 0;
}
