#include "sensors.h"

#include "esp_rom_sys.h"       // esp_rom_delay_us
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "sensors";

// ── DS18B20 OneWire commands ──────────────────────────────────────────────────
static constexpr uint8_t OW_SKIP_ROM        = 0xCC;
static constexpr uint8_t OW_CONVERT_T       = 0x44;
static constexpr uint8_t OW_READ_SCRATCHPAD = 0xBE;

// ════════════════════════════════════════════════════════════════════════════
// DS18B20 — bit-bang OneWire
// ════════════════════════════════════════════════════════════════════════════

DS18B20::DS18B20(gpio_num_t pin) : _pin(pin)
{
    // Open-drain mode: write 0 to drive LOW, write 1 to release (high-Z).
    // External 4.7 kΩ pull-up holds the bus HIGH when released.
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << pin,
        .mode         = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    gpio_set_level(_pin, 1);  // release bus
}

// Send reset pulse; return true if sensor replies with a presence pulse.
bool DS18B20::reset()
{
    portDISABLE_INTERRUPTS();
    gpio_set_level(_pin, 0);
    esp_rom_delay_us(480);          // reset pulse ≥480 µs
    gpio_set_level(_pin, 1);        // release
    esp_rom_delay_us(70);           // wait for presence pulse to start
    bool present = (gpio_get_level(_pin) == 0);
    portENABLE_INTERRUPTS();
    esp_rom_delay_us(410);          // let presence pulse finish
    return present;
}

void DS18B20::writeBit(bool b)
{
    portDISABLE_INTERRUPTS();
    gpio_set_level(_pin, 0);
    esp_rom_delay_us(b ? 1 : 60);  // write-1: 1 µs LOW; write-0: 60 µs LOW
    gpio_set_level(_pin, 1);
    esp_rom_delay_us(b ? 59 : 1);  // complete 60 µs slot
    portENABLE_INTERRUPTS();
}

bool DS18B20::readBit()
{
    portDISABLE_INTERRUPTS();
    gpio_set_level(_pin, 0);
    esp_rom_delay_us(1);            // initiate read slot
    gpio_set_level(_pin, 1);        // release
    esp_rom_delay_us(14);           // sample at 15 µs from start
    bool bit = (gpio_get_level(_pin) == 1);
    portENABLE_INTERRUPTS();
    esp_rom_delay_us(45);           // complete 60 µs slot
    return bit;
}

void DS18B20::writeByte(uint8_t byte)
{
    for (int i = 0; i < 8; ++i)
        writeBit((byte >> i) & 1);  // LSB first
}

uint8_t DS18B20::readByte()
{
    uint8_t val = 0;
    for (int i = 0; i < 8; ++i)
        if (readBit()) val |= (1 << i);
    return val;
}

// Dallas/Maxim CRC-8 (polynomial 0x31, reflected as 0x8C)
uint8_t DS18B20::crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; ++i) {
        uint8_t b = data[i];
        for (int j = 0; j < 8; ++j) {
            crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
            b >>= 1;
        }
    }
    return crc;
}

bool DS18B20::read(float &temp)
{
    // Step 1 — trigger conversion
    if (!reset()) {
        ESP_LOGW(TAG, "DS18B20: no presence pulse on convert");
        return false;
    }
    writeByte(OW_SKIP_ROM);
    writeByte(OW_CONVERT_T);

    // Step 2 — wait for 12-bit conversion (750 ms max)
    vTaskDelay(pdMS_TO_TICKS(750));

    // Step 3 — read scratchpad
    if (!reset()) {
        ESP_LOGW(TAG, "DS18B20: no presence pulse on read");
        return false;
    }
    writeByte(OW_SKIP_ROM);
    writeByte(OW_READ_SCRATCHPAD);

    uint8_t buf[9];
    for (auto &b : buf) b = readByte();

    // Step 4 — verify CRC (byte 8 is CRC of bytes 0–7)
    if (crc8(buf, 8) != buf[8]) {
        ESP_LOGW(TAG, "DS18B20: CRC error");
        return false;
    }

    // Step 5 — decode temperature (two's complement, LSB = 1/16 °C)
    int16_t raw = static_cast<int16_t>((buf[1] << 8) | buf[0]);
    temp = raw / 16.0f;
    return true;
}


// ════════════════════════════════════════════════════════════════════════════
// MAX6675 — software SPI (CPOL=0, CPHA=0, MSB first, read-only)
// ════════════════════════════════════════════════════════════════════════════

MAX6675::MAX6675(gpio_num_t cs, gpio_num_t sck, gpio_num_t miso)
    : _cs(cs), _sck(sck), _miso(miso)
{
    const gpio_config_t outCfg = {
        .pin_bit_mask = (1ULL << cs) | (1ULL << sck),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&outCfg);

    const gpio_config_t inCfg = {
        .pin_bit_mask = 1ULL << miso,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&inCfg);

    gpio_set_level(_cs,  1);  // deselect
    gpio_set_level(_sck, 0);  // clock idle low
}

// Clock out 16 bits MSB-first from MISO while CS is held LOW.
uint16_t MAX6675::transfer16()
{
    uint16_t val = 0;
    gpio_set_level(_cs, 0);
    esp_rom_delay_us(1);

    for (int i = 15; i >= 0; --i) {
        gpio_set_level(_sck, 1);
        esp_rom_delay_us(1);
        if (gpio_get_level(_miso))
            val |= (1u << i);
        gpio_set_level(_sck, 0);
        esp_rom_delay_us(1);
    }

    gpio_set_level(_cs, 1);
    return val;
}

bool MAX6675::read(float &temp)
{
    uint16_t raw = transfer16();

    // Bit 1: fault flag — set when thermocouple is open (disconnected)
    if (raw & 0x0004) {
        ESP_LOGW(TAG, "MAX6675: open thermocouple (fault bit set)");
        return false;
    }

    // Bits 14–3: 12-bit temperature value, 0.25 °C per LSB
    temp = static_cast<float>(raw >> 3) * 0.25f;
    return true;
}
