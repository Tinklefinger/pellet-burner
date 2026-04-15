# Pellet Burner — Wiring Guide

---

## Parts to Buy — Passive Components

Everything you need before you start wiring. Organised by priority.

### Required — won't work without these

| Component | Value / Rating | Qty | Used for |
|-----------|---------------|-----|----------|
| Resistor | 4.7 kΩ | 1 | DS18B20 DATA pull-up |
| Resistor | 330 Ω | 1 | TRIAC gate current limiter (GPIO33 → MOC3021) |
| Resistor | 100 Ω | 1 | TRIAC snubber (in series with snubber cap) |
| Capacitor | 100 nF / **400V** (film or ceramic) | 1 | TRIAC snubber — **must be rated for AC voltage** |

### Recommended — prevents instability and protects hardware

| Component | Value / Rating | Qty | Used for |
|-----------|---------------|-----|----------|
| Electrolytic capacitor | 100 µF / 10V | 1 | HLK-PM01 output bulk decoupling |
| Ceramic capacitor | 100 nF / 50V | 3 | One near each relay module VCC, one at ESP32 VIN |
| Ceramic capacitor | 100 nF / 50V | 2 | Button debounce (one per button, GPIO → GND) |

### Optional — good practice for permanent install

| Component | Value / Rating | Qty | Used for |
|-----------|---------------|-----|----------|
| MOV varistor | 275V AC | 1 | HLK-PM01 AC input surge protection (L ↔ N) |
| Resistor | 4.7 kΩ | 2 | I2C pull-ups — only if OLED cable is longer than ~20 cm |

---

## Power Distribution

```
AC Mains (220V)
  [MOV across L↔N — optional surge protection]
  └─ HLK-PM01
       ├─ +Vo (5V) ──[100µF + 100nF to GND]──┬──→ ESP32 VIN
       │                                      └──→ Relay module VCC (×3)
       │                                           [100nF to GND at each relay module]
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

| OLED pin | Connects to | Note |
|----------|-------------|------|
| VCC | ESP32 3.3V | |
| GND | GND bus | |
| SDA | GPIO 21 | |
| SCL | GPIO 22 | |

**Passive components:**
- No pull-ups needed for short cables (under ~20 cm).
- If you get I2C errors: add **4.7 kΩ** from SDA → 3.3V and **4.7 kΩ** from SCL → 3.3V.

I2C address: **0x3C** (some boards use 0x3D — run an I2C scanner if display stays blank).

---

## DS18B20 Water Temperature Sensor (OneWire)

| DS18B20 wire | Connects to | Note |
|-------------|-------------|------|
| VCC (red) | ESP32 3.3V | |
| GND (black) | GND bus | |
| DATA (yellow) | GPIO 4 | |

**Passive components:**
- **4.7 kΩ resistor** between DATA and 3.3V — **required, do not skip.**
  Without it the bus floats and you get CRC errors on every read.

Use the waterproof stainless steel probe version. Route cable away from AC wiring.

---

## MAX6675 Thermocouple Module (SPI)

| MAX6675 pin | Connects to | Note |
|------------|-------------|------|
| VCC | ESP32 3.3V | |
| GND | GND bus | |
| CS | GPIO 5 | |
| SCK | GPIO 18 | |
| SO (MISO) | GPIO 19 | |

**Passive components:** None — the module has everything on board.

The K-type thermocouple plugs into the two screw terminals on the MAX6675 module.
Polarity matters — check +/− markings on your thermocouple.
Use a K-type thermocouple rated to at least 900 °C.

---

## Relay Modules — Feeder / Igniter / Pump (active LOW)

| Signal | ESP32 pin | Load |
|--------|-----------|------|
| Feeder motor | GPIO 25 | Pellet auger motor |
| Igniter | GPIO 26 | Electric igniter element |
| Water pump | GPIO 27 | Circulation pump |

| Relay module pin | Connects to |
|-----------------|-------------|
| VCC | 5V (HLK-PM01 +Vo) |
| GND | GND bus |
| COM | AC Live |
| NO | Load |

**Passive components:**
- **100 nF ceramic capacitor** between VCC and GND **at each relay module** — prevents
  voltage spikes from coil switching from reaching the ESP32.

**Active LOW:** GPIO HIGH = relay OFF, GPIO LOW = relay ON.
All GPIOs are initialised HIGH at boot — relays stay off during startup and reset.

---

## TRIAC Blower Control (phase-angle dimmer)

| Signal | ESP32 pin | Note |
|--------|-----------|------|
| TRIAC gate | GPIO 33 | via 330 Ω → MOC3021 |
| Zero-cross detect | GPIO 35 | input-only pin, falling edge ISR |

**Circuit — low-voltage side (3.3V logic):**
```
GPIO33 ──[330Ω]──→ MOC3021 pin 1 (+)
                   MOC3021 pin 2 (−) → GND bus
                   MOC3021 pin 4     → TRIAC gate
                   MOC3021 pin 6     → TRIAC MT1
```

**Circuit — high-voltage side (across TRIAC):**
```
TRIAC MT1 ──[100Ω]──[100nF/400V]──→ TRIAC MT2   (snubber)
```

**Passive components:**
- **330 Ω resistor** — GPIO33 to MOC3021 LED (+). Required to limit current into the optocoupler LED.
- **100 Ω resistor + 100 nF / 400V capacitor in series** across TRIAC MT1 ↔ MT2.
  Prevents false triggering from AC line noise and protects TRIAC from voltage spikes.
  The capacitor **must be rated for AC/mains voltage (≥400V)** — standard 50V ceramics will fail.

**Zero-cross detector:** use a 4N35 optocoupler or a ready-made zero-cross module → GPIO35.

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
No external resistors needed — ESP32 internal pull-ups are enabled in firmware.

**Passive components:**
- **100 nF ceramic capacitor** from each GPIO pin to GND — optional hardware debounce.
  Prevents false triggers from electrical noise or bouncy button contacts.

---

## HLK-PM01 Power Supply

| HLK-PM01 terminal | Connects to |
|-------------------|-------------|
| L (AC Live) | Mains live wire (via fuse) |
| N (AC Neutral) | Mains neutral |
| +Vo (5V) | ESP32 VIN + relay VCC rails |
| -Vo (GND) | DC GND bus |

**Passive components:**
- **100 µF / 10V electrolytic + 100 nF ceramic** across +Vo and -Vo (at the output terminals).
  The electrolytic absorbs current spikes when relay coils switch; the ceramic handles high-frequency noise.
- **275V AC MOV varistor** across L and N (optional) — clamps mains voltage surges before they reach the module.

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

1. **USB only** — flash firmware, confirm serial log boots cleanly.
2. **Add OLED** — confirm I2C display initialises (Phase 6 needed for content).
3. **Add DS18B20** — wire 4.7 kΩ pull-up, confirm water temp in serial log and web UI.
4. **Add MAX6675** — confirm flame temp reads ~20 °C at room temperature.
5. **Wire GND bus + HLK-PM01 DC side** — connect -Vo to bus, +Vo to ESP32 VIN and relay VCC. Add 100 µF + 100 nF at output. Do not connect AC yet.
6. **Add relay modules** — add 100 nF decoupling at each module, test each relay clicks.
7. **Add buttons** — add 100 nF debounce caps, test On/Off and E-Stop behaviour.
8. **Add TRIAC + blower** — wire 330 Ω gate resistor and 100Ω + 100nF/400V snubber. Test phase-angle control at low voltage first if possible.
9. **AC mains wiring** — very last step, after all logic is verified. Fuse the live wire. Optionally add MOV across L/N.
