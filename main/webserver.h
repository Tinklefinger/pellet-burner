#pragma once

#include <cstdint>
#include "esp_http_server.h"
#include "settings.h"

struct BurnerStatus {
    float       water_temp    = 0.0f;   // DS18B20 (°C)
    float       flame_temp    = 0.0f;   // MAX6675  (°C)
    bool        flame_on      = false;
    bool        pump_on       = false;
    bool        feeder_on     = false;
    bool        igniter_on    = false;
    uint8_t     blower_speed  = 0;      // 0–100 %
    const char *state         = "IDLE";
};

class WebServer {
public:
    void start();

private:
    static esp_err_t handleRoot(httpd_req_t *req);
    static esp_err_t handleStatusGet(httpd_req_t *req);
    static esp_err_t handleSettingsGet(httpd_req_t *req);
    static esp_err_t handleSettingsPost(httpd_req_t *req);

    httpd_handle_t _server = nullptr;
};

// Shared globals — written by sensor/control tasks, read by web server
extern BurnerStatus g_status;
extern Settings     g_settings;
