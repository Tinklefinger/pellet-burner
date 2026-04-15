#include "settings.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG    = "settings";
static const char *NVS_NS = "pellet";

// Helper: store float as u32 bits
static inline uint32_t f2u(float f) { return *reinterpret_cast<uint32_t *>(&f); }
static inline float     u2f(uint32_t u) { return *reinterpret_cast<float *>(&u); }

void Settings::init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition erased and re-initialised");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    load();
}

void Settings::load()
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        ESP_LOGI(TAG, "No saved settings — using defaults");
        return;
    }

    uint32_t u32; uint16_t u16; uint8_t u8; size_t len;

    // General
    if (nvs_get_u32(h, "target_temp",  &u32) == ESP_OK) target_temp   = u2f(u32);
    if (nvs_get_u8 (h, "op_mode",      &u8)  == ESP_OK) op_mode       = u8;
    if (nvs_get_u8 (h, "act_preset",   &u8)  == ESP_OK) active_preset = u8;

    // Power level 1
    if (nvs_get_u8 (h, "p1_fan",       &u8)  == ESP_OK) p1.fan_speed  = u8;
    if (nvs_get_u16(h, "p1_fon",       &u16) == ESP_OK) p1.feeder_on  = u16;
    if (nvs_get_u16(h, "p1_foff",      &u16) == ESP_OK) p1.feeder_off = u16;

    // Power level 2
    if (nvs_get_u8 (h, "p2_fan",       &u8)  == ESP_OK) p2.fan_speed  = u8;
    if (nvs_get_u16(h, "p2_fon",       &u16) == ESP_OK) p2.feeder_on  = u16;
    if (nvs_get_u16(h, "p2_foff",      &u16) == ESP_OK) p2.feeder_off = u16;

    // Power level 3
    if (nvs_get_u8 (h, "p3_fan",       &u8)  == ESP_OK) p3.fan_speed  = u8;
    if (nvs_get_u16(h, "p3_fon",       &u16) == ESP_OK) p3.feeder_on  = u16;
    if (nvs_get_u16(h, "p3_foff",      &u16) == ESP_OK) p3.feeder_off = u16;

    // Thresholds
    if (nvs_get_u32(h, "p1_thresh",    &u32) == ESP_OK) p1_threshold  = u2f(u32);
    if (nvs_get_u32(h, "p3_thresh",    &u32) == ESP_OK) p3_threshold  = u2f(u32);

    // Economy
    if (nvs_get_u16(h, "eco_hold",     &u16) == ESP_OK) economy_hold_time    = u16;
    if (nvs_get_u32(h, "eco_resume",   &u32) == ESP_OK) economy_resume_delta = u2f(u32);

    // Pump
    if (nvs_get_u32(h, "pump_start",   &u32) == ESP_OK) pump_start_temp = u2f(u32);

    // Timer presets (A=0, B=1, C=2)
    for (int i = 0; i < 3; i++) {
        char key[12];
        snprintf(key, sizeof(key), "t%d_sh", i);
        if (nvs_get_u8(h, key, &u8) == ESP_OK) timer[i].start_h = u8;
        snprintf(key, sizeof(key), "t%d_sm", i);
        if (nvs_get_u8(h, key, &u8) == ESP_OK) timer[i].start_m = u8;
        snprintf(key, sizeof(key), "t%d_eh", i);
        if (nvs_get_u8(h, key, &u8) == ESP_OK) timer[i].end_h   = u8;
        snprintf(key, sizeof(key), "t%d_em", i);
        if (nvs_get_u8(h, key, &u8) == ESP_OK) timer[i].end_m   = u8;
    }

    // WiFi
    len = sizeof(wifi_ssid); nvs_get_str(h, "wifi_ssid", wifi_ssid, &len);
    len = sizeof(wifi_pass); nvs_get_str(h, "wifi_pass", wifi_pass, &len);

    nvs_close(h);
    ESP_LOGI(TAG, "Settings loaded from NVS");
}

void Settings::save() const
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS for writing");
        return;
    }

    // General
    nvs_set_u32(h, "target_temp",  f2u(target_temp));
    nvs_set_u8 (h, "op_mode",      op_mode);
    nvs_set_u8 (h, "act_preset",   active_preset);

    // Power level 1
    nvs_set_u8 (h, "p1_fan",       p1.fan_speed);
    nvs_set_u16(h, "p1_fon",       p1.feeder_on);
    nvs_set_u16(h, "p1_foff",      p1.feeder_off);

    // Power level 2
    nvs_set_u8 (h, "p2_fan",       p2.fan_speed);
    nvs_set_u16(h, "p2_fon",       p2.feeder_on);
    nvs_set_u16(h, "p2_foff",      p2.feeder_off);

    // Power level 3
    nvs_set_u8 (h, "p3_fan",       p3.fan_speed);
    nvs_set_u16(h, "p3_fon",       p3.feeder_on);
    nvs_set_u16(h, "p3_foff",      p3.feeder_off);

    // Thresholds
    nvs_set_u32(h, "p1_thresh",    f2u(p1_threshold));
    nvs_set_u32(h, "p3_thresh",    f2u(p3_threshold));

    // Economy
    nvs_set_u16(h, "eco_hold",     economy_hold_time);
    nvs_set_u32(h, "eco_resume",   f2u(economy_resume_delta));

    // Pump
    nvs_set_u32(h, "pump_start",   f2u(pump_start_temp));

    // Timer presets
    for (int i = 0; i < 3; i++) {
        char key[12];
        snprintf(key, sizeof(key), "t%d_sh", i);  nvs_set_u8(h, key, timer[i].start_h);
        snprintf(key, sizeof(key), "t%d_sm", i);  nvs_set_u8(h, key, timer[i].start_m);
        snprintf(key, sizeof(key), "t%d_eh", i);  nvs_set_u8(h, key, timer[i].end_h);
        snprintf(key, sizeof(key), "t%d_em", i);  nvs_set_u8(h, key, timer[i].end_m);
    }

    // WiFi
    nvs_set_str(h, "wifi_ssid", wifi_ssid);
    nvs_set_str(h, "wifi_pass", wifi_pass);

    nvs_commit(h);
    nvs_close(h);
    ESP_LOGI(TAG, "Settings saved to NVS");
}
