#include "config.h"
#include "settings.h"
#include "wifi.h"
#include "webserver.h"
#include "sensors.h"

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

static DS18B20 g_ds18b20(PIN_DS18B20);
static MAX6675 g_max6675(PIN_MAX6675_CS, PIN_MAX6675_SCK, PIN_MAX6675_MISO);

// ── GPIO init ─────────────────────────────────────────────────────────────────
static void gpioInit()
{
    // Relay outputs — active LOW, start deactivated (HIGH)
    const gpio_config_t relayCfg = {
        .pin_bit_mask = (1ULL << PIN_RELAY_FEEDER) |
                        (1ULL << PIN_RELAY_PUMP),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&relayCfg);
    gpio_set_level(PIN_RELAY_FEEDER, 1);
    gpio_set_level(PIN_RELAY_PUMP,   1);

    // AC dimmer outputs (DIM1 blower, DIM2 igniter) — start at 0 (off)
    const gpio_config_t dimmerOutCfg = {
        .pin_bit_mask = (1ULL << PIN_DIM_BLOWER) |
                        (1ULL << PIN_DIM_IGNITER),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&dimmerOutCfg);
    gpio_set_level(PIN_DIM_BLOWER,  0);
    gpio_set_level(PIN_DIM_IGNITER, 0);

    // Zero-cross input — input only, no pull (signal driven by dimmer module)
    const gpio_config_t zcCfg = {
        .pin_bit_mask = (1ULL << PIN_ZERO_CROSS),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&zcCfg);

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

// ── Sensor task ───────────────────────────────────────────────────────────────
// DS18B20::read() blocks ~750 ms for the conversion, so the loop runs at
// roughly 1 s per cycle (750 ms conversion + 250 ms delay).
static void sensorTask(void *)
{
    float t;
    while (true) {
        // Water temperature — DS18B20 (OneWire)
        if (g_ds18b20.read(t))
            g_status.water_temp = t;

        // Flame temperature — MAX6675 (K-type thermocouple)
        // MAX6675 needs ≥220 ms between reads; DS18B20 conversion above provides this.
        if (g_max6675.read(t)) {
            g_status.flame_temp = t;
            g_status.flame_on   = (t > 100.0f);  // flame present above 100 °C
        } else {
            g_status.flame_on = false;            // open thermocouple → no flame
        }

        vTaskDelay(pdMS_TO_TICKS(250));
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
