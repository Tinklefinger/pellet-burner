#pragma once

#include <cstdint>
#include "driver/gpio.h"

// ── DS18B20 — OneWire temperature sensor (bit-bang) ──────────────────────────
//
// Wiring:  DATA → PIN_DS18B20 (GPIO4)  with 4.7kΩ pull-up to 3.3V
//          VCC  → 3.3V,  GND → GND
//
class DS18B20 {
public:
    explicit DS18B20(gpio_num_t pin);

    // Trigger a 12-bit conversion (blocks ~750 ms) then read the result.
    // Returns true on success; temp is set to °C.
    // Returns false on bus error or CRC mismatch — temp is unchanged.
    bool read(float &temp);

private:
    gpio_num_t _pin;

    bool    reset();
    void    writeBit(bool b);
    bool    readBit();
    void    writeByte(uint8_t byte);
    uint8_t readByte();
    uint8_t crc8(const uint8_t *data, uint8_t len);
};

// ── MAX6675 — K-type thermocouple module (software SPI, read-only) ───────────
//
// Wiring:  CS   → PIN_MAX6675_CS   (GPIO5)
//          SCK  → PIN_MAX6675_SCK  (GPIO18)
//          MISO → PIN_MAX6675_MISO (GPIO19)
//          VCC  → 3.3V,  GND → GND
//
class MAX6675 {
public:
    MAX6675(gpio_num_t cs, gpio_num_t sck, gpio_num_t miso);

    // Read thermocouple temperature.
    // Returns true on success (thermocouple present); temp is set to °C.
    // Returns false if thermocouple is open (disconnected).
    bool read(float &temp);

private:
    gpio_num_t _cs, _sck, _miso;

    uint16_t transfer16();
};
