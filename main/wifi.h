#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"

class WiFiManager {
public:
    static constexpr EventBits_t CONNECTED_BIT = BIT0;
    static constexpr EventBits_t FAIL_BIT      = BIT1;

    void connect(const char *ssid, const char *password);
    bool isConnected() const;

private:
    static void eventHandler(void *arg, esp_event_base_t base,
                             int32_t event_id, void *event_data);

    EventGroupHandle_t _eventGroup = nullptr;
    int                _retries    = 0;

    static constexpr int MAX_RETRIES = 5;
};
