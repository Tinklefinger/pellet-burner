#pragma once

#include <cstdint>
#include <cstring>
#include "config.h"

struct PowerLevel {
    uint8_t  fan_speed;     // %
    uint16_t feeder_on;     // seconds
    uint16_t feeder_off;    // seconds
};

struct TimerPreset {
    uint8_t start_h;
    uint8_t start_m;
    uint8_t end_h;
    uint8_t end_m;
};

class Settings {
public:
    // ── General ──────────────────────────────────────────────────────────────
    float   target_temp     = DEFAULT_TARGET_TEMP;
    uint8_t op_mode         = DEFAULT_OP_MODE;      // MODE_STANDBY/AUTO/TIMER
    uint8_t active_preset   = DEFAULT_ACTIVE_PRESET; // 0=A, 1=B, 2=C

    // ── Power levels ─────────────────────────────────────────────────────────
    PowerLevel p1 = { DEFAULT_P1_FAN_SPEED, DEFAULT_P1_FEEDER_ON, DEFAULT_P1_FEEDER_OFF };
    PowerLevel p2 = { DEFAULT_P2_FAN_SPEED, DEFAULT_P2_FEEDER_ON, DEFAULT_P2_FEEDER_OFF };
    PowerLevel p3 = { DEFAULT_P3_FAN_SPEED, DEFAULT_P3_FEEDER_ON, DEFAULT_P3_FEEDER_OFF };

    // ── Power thresholds ──────────────────────────────────────────────────────
    float p1_threshold = DEFAULT_P1_THRESHOLD;  // °C below target → P1
    float p3_threshold = DEFAULT_P3_THRESHOLD;  // °C below target → P3

    // ── Economy mode ──────────────────────────────────────────────────────────
    uint16_t economy_hold_time    = DEFAULT_ECONOMY_HOLD_TIME;
    float    economy_resume_delta = DEFAULT_ECONOMY_RESUME_DELTA;

    // ── Pump ─────────────────────────────────────────────────────────────────
    float pump_start_temp = DEFAULT_PUMP_START_TEMP;

    // ── Timer presets ─────────────────────────────────────────────────────────
    TimerPreset timer[3] = {
        { DEFAULT_TIMER_A_START_H, DEFAULT_TIMER_A_START_M, DEFAULT_TIMER_A_END_H, DEFAULT_TIMER_A_END_M },
        { DEFAULT_TIMER_B_START_H, DEFAULT_TIMER_B_START_M, DEFAULT_TIMER_B_END_H, DEFAULT_TIMER_B_END_M },
        { DEFAULT_TIMER_C_START_H, DEFAULT_TIMER_C_START_M, DEFAULT_TIMER_C_END_H, DEFAULT_TIMER_C_END_M },
    };

    // ── WiFi ─────────────────────────────────────────────────────────────────
    char wifi_ssid[32] = {};
    char wifi_pass[64] = {};

    Settings() {
        strncpy(wifi_ssid, CONFIG_ESP_WIFI_SSID,    sizeof(wifi_ssid) - 1);
        strncpy(wifi_pass, CONFIG_ESP_WIFI_PASSWORD, sizeof(wifi_pass) - 1);
    }

    void init();        // NVS flash init + load
    void load();
    void save() const;
};
