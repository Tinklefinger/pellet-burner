#pragma once

#include "driver/gpio.h"

// ── Relay outputs (active LOW) ────────────────────────────────────────────────
#define PIN_RELAY_FEEDER    GPIO_NUM_25   // Feeder reductor motor
#define PIN_RELAY_PUMP      GPIO_NUM_27   // Water pump

// ── 2-channel AC dimmer module (3.3V logic, PWM input) ───────────────────────
// Module: Hipzeepo 2-ch AC Light Dimmer, 3.3V/5V logic, 110/220V AC
#define PIN_ZERO_CROSS      GPIO_NUM_35   // Shared zero-cross output → ZC pin (input only)
#define PIN_DIM_BLOWER      GPIO_NUM_33   // Channel 1 → DIM1  — blower motor speed
#define PIN_DIM_IGNITER     GPIO_NUM_26   // Channel 2 → DIM2  — igniter soft-start (ramp 0→100% over IGNITER_SOFTSTART_MS)

// ── Temperature sensors ───────────────────────────────────────────────────────
#define PIN_DS18B20         GPIO_NUM_4    // OneWire — water temperature
#define PIN_MAX6675_CS      GPIO_NUM_5    // SPI CS  — flame thermocouple
#define PIN_MAX6675_SCK     GPIO_NUM_18   // SPI CLK
#define PIN_MAX6675_MISO    GPIO_NUM_19   // SPI MISO

// ── OLED SH1106 (I2C) ─────────────────────────────────────────────────────────
#define PIN_I2C_SDA         GPIO_NUM_21
#define PIN_I2C_SCL         GPIO_NUM_22

// ── Physical buttons (active LOW, internal pullup) ────────────────────────────
#define PIN_BTN_ONOFF       GPIO_NUM_13   // On / Off
#define PIN_BTN_ESTOP       GPIO_NUM_14   // Emergency stop

// ── Web UI auth ───────────────────────────────────────────────────────────────
#define WEB_USERNAME            "admin"
#define WEB_PASSWORD            "pellet123"

// ── Operation modes ───────────────────────────────────────────────────────────
#define MODE_STANDBY            0
#define MODE_AUTOMATIC          1
#define MODE_TIMER              2

// ── Default general settings ──────────────────────────────────────────────────
#define DEFAULT_TARGET_TEMP         75.0f   // °C
#define DEFAULT_OP_MODE             MODE_STANDBY
#define DEFAULT_ACTIVE_PRESET       0       // 0=A, 1=B, 2=C

// ── Default power level settings ─────────────────────────────────────────────
#define DEFAULT_P1_FAN_SPEED        30      // %
#define DEFAULT_P1_FEEDER_ON        5       // sec
#define DEFAULT_P1_FEEDER_OFF       60      // sec

#define DEFAULT_P2_FAN_SPEED        60      // %
#define DEFAULT_P2_FEEDER_ON        8       // sec
#define DEFAULT_P2_FEEDER_OFF       45      // sec

#define DEFAULT_P3_FAN_SPEED        100     // %
#define DEFAULT_P3_FEEDER_ON        12      // sec
#define DEFAULT_P3_FEEDER_OFF       30      // sec

// ── Default power thresholds ──────────────────────────────────────────────────
#define DEFAULT_P1_THRESHOLD        5.0f    // °C below target → Power 1
#define DEFAULT_P3_THRESHOLD        15.0f   // °C below target → Power 3

// ── Igniter soft-start ───────────────────────────────────────────────────────
#define IGNITER_SOFTSTART_MS        4000    // ms — ramp 0→100% duration at IGNITING entry

// ── Default economy mode ──────────────────────────────────────────────────────
#define DEFAULT_ECONOMY_HOLD_TIME   300     // sec at target before shutdown
#define DEFAULT_ECONOMY_RESUME_DELTA 5.0f   // °C drop below target to resume

// ── Default pump settings ─────────────────────────────────────────────────────
#define DEFAULT_PUMP_START_TEMP     45.0f   // °C min water temp to run pump

// ── Default timer presets (HH and MM stored separately) ──────────────────────
#define DEFAULT_TIMER_A_START_H     18
#define DEFAULT_TIMER_A_START_M     0
#define DEFAULT_TIMER_A_END_H       9
#define DEFAULT_TIMER_A_END_M       0

#define DEFAULT_TIMER_B_START_H     7
#define DEFAULT_TIMER_B_START_M     0
#define DEFAULT_TIMER_B_END_H       22
#define DEFAULT_TIMER_B_END_M       0

#define DEFAULT_TIMER_C_START_H     6
#define DEFAULT_TIMER_C_START_M     30
#define DEFAULT_TIMER_C_END_H       8
#define DEFAULT_TIMER_C_END_M       0
