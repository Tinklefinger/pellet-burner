#include "config.h"
#include "settings.h"
#include "wifi.h"
#include "webserver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "main";

// ── Global instances ──────────────────────────────────────────────────────────
Settings     g_settings;
BurnerStatus g_status;
WiFiManager  g_wifi;
WebServer    g_webserver;

// ── GPIO init ─────────────────────────────────────────────────────────────────
static void gpioInit()
{
    // Relay outputs — active LOW, start deactivated (HIGH)
    const gpio_config_t relayCfg = {
        .pin_bit_mask = (1ULL << PIN_RELAY_FEEDER)  |
                        (1ULL << PIN_RELAY_IGNITER) |
                        (1ULL << PIN_RELAY_PUMP),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&relayCfg);
    gpio_set_level(PIN_RELAY_FEEDER,  1);
    gpio_set_level(PIN_RELAY_IGNITER, 1);
    gpio_set_level(PIN_RELAY_PUMP,    1);

    // TRIAC trigger output
    const gpio_config_t triacCfg = {
        .pin_bit_mask = (1ULL << PIN_TRIAC),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&triacCfg);
    gpio_set_level(PIN_TRIAC, 0);

    // Buttons — active LOW with internal pullup
    const gpio_config_t btnCfg = {
        .pin_bit_mask = (1ULL << PIN_BTN_ONOFF) | (1ULL << PIN_BTN_ESTOP),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&btnCfg);

    ESP_LOGI(TAG, "GPIO initialised");
}

// ── Sensor task (stub — replace with real DS18B20 + MAX6675 reads) ────────────
static void sensorTask(void *)
{
    while (true) {
        // TODO: read DS18B20  → g_status.water_temp
        // TODO: read MAX6675  → g_status.flame_temp, g_status.flame_on
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ── Control task (stub — replace with state machine) ─────────────────────────
static void controlTask(void *)
{
    while (true) {
        // TODO: IDLE → IGNITING → RUNNING → COOLING → IDLE
        // TODO: feeder timing, blower TRIAC, pump hysteresis
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ── Entry point ───────────────────────────────────────────────────────────────
extern "C" void app_main()
{
    ESP_LOGI(TAG, "Pellet burner starting — IDF %s", IDF_VER);

    g_settings.init();
    ESP_LOGI(TAG, "Target: %.1f°C | P2 fan: %u%% | P2 feeder %us/%us",
        g_settings.target_temp, g_settings.p2.fan_speed,
        g_settings.p2.feeder_on, g_settings.p2.feeder_off);

    gpioInit();

    g_wifi.connect(g_settings.wifi_ssid, g_settings.wifi_pass);

    g_webserver.start();

    xTaskCreate(sensorTask,  "sensors", 4096, nullptr, 5, nullptr);
    xTaskCreate(controlTask, "control", 4096, nullptr, 5, nullptr);

    ESP_LOGI(TAG, "System ready");
}
