# Pellet Burner — Wiring Guide

## Power Distribution

```
AC Mains (220V)
  └─ HLK-PM01
       ├─ +Vo (5V) ──┬──→ ESP32 VIN
       │             └──→ Relay module VCC (×3)
       └─ -Vo (GND) ──→ DC GND bus ──→ all component GNDs

ESP32 3.3V pin ──┬──→ OLED VCC
                 ├──→ DS18B20 VCC
                 └──→ MAX6675 VCC
```

**Rules:**
- Every DC GND wire terminates at a common GND bus (terminal strip).
- The HLK-PM01 `-Vo` is the source of that bus.
- AC side (L/N) and DC side are fully isolated — do not bridge them.
- Use a screw terminal strip for the GND bus, not a solder blob.

---

## SH1106 OLED Display (I2C)

| OLED pin | ESP32 pin | Note |
|----------|-----------|------|
| VCC | 3.3V | |
| GND | GND bus | |
| SDA | GPIO 21 | |
| SCL | GPIO 22 | |

No external pull-up resistors needed at typical cable lengths.
If you get I2C errors with long cables, add 4.7 kΩ from SDA and SCL to 3.3V.
I2C address: **0x3C** (some boards use 0x3D — run an I2C scanner if display is blank).

---

## DS18B20 Water Temperature Sensor (OneWire)

| DS18B20 wire | ESP32 pin | Note |
|-------------|-----------|------|
| VCC (red) | 3.3V | |
| GND (black) | GND bus | |
| DATA (yellow) | GPIO 4 | |
| — | — | 4.7 kΩ resistor between DATA and 3.3V — **required** |

The pull-up resistor is mandatory. Without it the bus floats and you get CRC errors.
Use the waterproof stainless steel probe version. Route cable away from AC wiring.

---

## MAX6675 Thermocouple Module (SPI)

| MAX6675 pin | ESP32 pin | Note |
|------------|-----------|------|
| VCC | 3.3V | |
| GND | GND bus | |
| CS | GPIO 5 | |
| SCK | GPIO 18 | |
| SO (MISO) | GPIO 19 | |

The K-type thermocouple connects to the two screw terminals on the MAX6675 module.
Polarity matters — check +/− markings on your thermocouple.
Use a K-type thermocouple rated to at least 900 °C.

---

## Relay Modules — Feeder / Igniter / Pump (active LOW)

| Signal | ESP32 pin | Relay | Load |
|--------|-----------|-------|------|
| Feeder motor | GPIO 25 | IN → relay 1 | Pellet auger motor |
| Igniter | GPIO 26 | IN → relay 2 | Electric igniter element |
| Water pump | GPIO 27 | IN → relay 3 | Circulation pump |

All three relay modules share the same power:

| Relay module pin | Connects to |
|-----------------|-------------|
| VCC | 5V (HLK-PM01 +Vo) |
| GND | GND bus |
| COM | AC Live |
| NO | Load |

**Active LOW:** GPIO HIGH = relay OFF, GPIO LOW = relay ON.
All GPIOs are initialised HIGH at boot — relays stay off during startup and reset.

---

## TRIAC Blower Control (phase-angle dimmer)

| Signal | ESP32 pin | Note |
|--------|-----------|------|
| TRIAC gate | GPIO 33 | → 330 Ω → MOC3021 input LED (+) |
| Zero-cross detect | GPIO 35 | Input-only pin — falling edge ISR |

**Circuit (low-voltage side):**
```
GPIO33 → 330Ω → MOC3021 pin 1 (+)
                 MOC3021 pin 2 (−) → GND bus
                 MOC3021 pin 4 → TRIAC gate
                 MOC3021 pin 6 → MT1 (via snubber)
```

**Snubber across TRIAC:** 100 Ω + 100 nF / 400 V in series.
**Zero-cross detector:** 4N35 optocoupler or dedicated zero-cross module, output to GPIO35.

> ⚠️ The TRIAC circuit operates at 230V AC on the load side.
> Keep AC and DC wiring physically separated.
> Fuse the AC circuit (5–10A slow-blow).
> Do not wire AC until the state machine (Phase 5) is complete and tested at low voltage.

---

## Physical Buttons (active LOW, internal pull-up)

| Button | ESP32 pin | Function |
|--------|-----------|----------|
| On / Off | GPIO 13 | Toggle Standby ↔ previous mode |
| Emergency Stop | GPIO 14 | All outputs OFF immediately → STANDBY |

Wiring: one terminal → GPIO pin, other terminal → GND bus.
No external resistors — ESP32 internal pull-ups are enabled in firmware.
Optional: 100 nF capacitor from GPIO pin to GND for hardware debounce.

---

## Complete Pin Reference

| Component | GPIO | Direction | Voltage | Protocol |
|-----------|------|-----------|---------|----------|
| OLED SDA | 21 | out | 3.3V | I2C |
| OLED SCL | 22 | out | 3.3V | I2C |
| DS18B20 DATA | 4 | in/out | 3.3V | 1-Wire |
| MAX6675 CS | 5 | out | 3.3V | SPI |
| MAX6675 SCK | 18 | out | 3.3V | SPI |
| MAX6675 MISO | 19 | in | 3.3V | SPI |
| Relay Feeder | 25 | out | 3.3V | GPIO active LOW |
| Relay Igniter | 26 | out | 3.3V | GPIO active LOW |
| Relay Pump | 27 | out | 3.3V | GPIO active LOW |
| TRIAC gate | 33 | out | 3.3V | GPIO (330Ω + MOC3021) |
| Zero-cross | 35 | in | 3.3V | GPIO input-only |
| Button On/Off | 13 | in | 3.3V | GPIO pull-up |
| Button E-Stop | 14 | in | 3.3V | GPIO pull-up |

---

## Bring-up Order (recommended)

1. **USB only** — flash firmware, check serial log boots cleanly.
2. **Add OLED** — verify I2C display initialises (Phase 6 needed for content).
3. **Add DS18B20** — verify water temp reads in serial log and web UI.
4. **Add MAX6675** — verify flame temp reads ~20 °C at room temperature.
5. **Add relay modules** — test each relay clicks via a future web UI control.
6. **Add TRIAC + blower** — last, only after state machine is complete.
7. **AC mains wiring** — very last step, after all logic is verified at low voltage.
