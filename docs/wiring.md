# Pellet Burner — Wiring Guide

---

## Parts to Buy — Passive Components

### Required — won't work without these

| Component | Value / Rating | Qty | Used for |
|-----------|---------------|-----|----------|
| Resistor | 4.7 kΩ, **1/4W**, through-hole axial | 1 | DS18B20 DATA pull-up |

That's it for required discrete passives. The AC dimmer module has its gate resistor, optocouplers, and snubber **built in**.

### Recommended — prevents instability and protects hardware

| Component | Value / Rating | Qty | Form factor | Used for |
|-----------|---------------|-----|-------------|----------|
| Electrolytic capacitor | 100 µF / 10V | 1 | Through-hole radial | HLK-PM01 output bulk decoupling |
| Ceramic capacitor | 100 nF / 50V | 3 | Through-hole or SMD | One at each relay module VCC + GND, one at ESP32 VIN |
| Ceramic capacitor | 100 nF / 50V | 2 | Through-hole | Button debounce — one per button, GPIO to GND |

### Optional — good practice for permanent install

| Component | Value / Rating | Qty | Form factor | Used for |
|-----------|---------------|-----|-------------|----------|
| MOV varistor | 275V AC | 1 | Through-hole radial | HLK-PM01 AC input surge protection (L ↔ N) |
| Resistor | 4.7 kΩ, 1/4W | 2 | Through-hole axial | I2C pull-ups — only if OLED cable is longer than ~20 cm |

### Resistor wattage guide

| Resistor | Wattage | Why |
|----------|---------|-----|
| 4.7 kΩ pull-up | **1/4W (0.25W)** | Pulls signal line to 3.3V — only ~0.7 mA flows |
| 4.7 kΩ I2C pull-up | **1/4W (0.25W)** | Same — signal line only |
| Any other signal resistor | **1/4W (0.25W)** | Standard through-hole axial, 5mm pitch |

> All remaining resistors in this build are on signal lines at 3.3V — standard 1/4W
> carbon film through-hole resistors are fine for all of them.
> The old 100 Ω snubber and 330 Ω gate resistors are no longer needed — they are
> built into the 2-channel AC dimmer module.

---

## Power Distribution

```
AC Mains (220V)
  [MOV across L↔N — optional surge protection]
  └─ HLK-PM01
       ├─ +Vo (5V) ──[100µF + 100nF to GND]──┬──→ ESP32 VIN
       │                                      └──→ Relay module VCC (×2: feeder, pump)
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

**Passive components:** None required for short cables.
Optional: **4.7 kΩ** from SDA → 3.3V and **4.7 kΩ** from SCL → 3.3V if cable is over ~20 cm.

I2C address: **0x3C** (some boards 0x3D — run I2C scanner if display stays blank).

---

## DS18B20 Water Temperature Sensor (OneWire)

| DS18B20 wire | Connects to | Note |
|-------------|-------------|------|
| VCC (red) | ESP32 3.3V | |
| GND (black) | GND bus | |
| DATA (yellow) | GPIO 4 | |

**Passive components:**
- **4.7 kΩ resistor (1/4W)** between DATA and 3.3V — **required.**
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

K-type thermocouple plugs into the two screw terminals on the module.
Polarity matters — check +/− markings. Use a thermocouple rated to at least 900 °C.

---

## 2-Channel AC Dimmer Module — Blower & Igniter

**Module:** Hipzeepo AC Light Dimmer 2-Channel, 3.3V/5V logic, 110/220V AC
**Built into the module:** TRIAC × 2, optocouplers, snubber circuits, zero-cross detector.
No discrete gate resistors, snubber caps, or optocouplers needed — they are all on the board.

| Dimmer module pin | Connects to | Load |
|------------------|-------------|------|
| VCC | ESP32 3.3V (or 5V) | — |
| GND | GND bus | — |
| ZC (zero-cross) | GPIO 35 | — shared output, falling edge ISR |
| DIM1 | GPIO 33 | Blower motor — variable speed (PWM) |
| DIM2 | GPIO 26 | Igniter heater — full power on/off (PWM 100% or 0%) |
| CH1 OUT (AC) | Blower motor terminals | 220V AC switched |
| CH2 OUT (AC) | Igniter heater terminals | 220V AC switched |
| AC IN | Mains L + N (via fuse) | Shared AC input for both channels |

**Passive components:** None on the DC logic side — all built into module.

**Firmware behaviour:**
- Blower: PWM duty cycle = fan speed % (0–100)
- Igniter: PWM 100% = full heat on, 0% = off — effectively on/off

> ⚠️ The AC output side operates at 230V.
> Keep AC and DC wiring physically separated.
> Fuse the AC input (5–10A slow-blow) before the module.
> Do not wire AC until the state machine (Phase 5) is complete and tested.

---

## Relay Modules — Feeder & Pump (active LOW)

> The igniter is now controlled by the AC dimmer module (channel 2).
> Only 2 relay modules are needed: feeder motor and water pump.

| Signal | ESP32 pin | Load |
|--------|-----------|------|
| Feeder motor | GPIO 25 | Pellet auger motor |
| Water pump | GPIO 27 | Circulation pump |

| Relay module pin | Connects to |
|-----------------|-------------|
| VCC | 5V (HLK-PM01 +Vo) |
| GND | GND bus |
| COM | AC Live |
| NO | Load |

**Passive components:**
- **100 nF ceramic capacitor** between VCC and GND at each relay module.
  Prevents coil switching spikes from reaching the ESP32.

**Active LOW:** GPIO HIGH = relay OFF, GPIO LOW = relay ON.
All GPIOs initialised HIGH at boot — relays stay off during startup and reset.

---

## Physical Buttons (active LOW, internal pull-up)

| Button | ESP32 pin | Function |
|--------|-----------|----------|
| On / Off | GPIO 13 | Toggle Standby ↔ previous mode |
| Emergency Stop | GPIO 14 | All outputs OFF immediately → STANDBY |

Wiring: one terminal → GPIO pin, other terminal → GND bus.
No external resistors — ESP32 internal pull-ups enabled in firmware.

**Passive components:**
- **100 nF ceramic capacitor** from each GPIO pin to GND — optional hardware debounce.

---

## HLK-PM01 Power Supply (AC 220V → DC 5V)

| HLK-PM01 terminal | Connects to |
|-------------------|-------------|
| L (AC Live) | Mains live (via fuse) |
| N (AC Neutral) | Mains neutral |
| +Vo (5V) | ESP32 VIN + relay module VCC rails |
| -Vo (GND) | DC GND bus |

**Passive components:**
- **100 µF / 10V electrolytic + 100 nF ceramic** across +Vo and -Vo at the output terminals.
- **275V AC MOV varistor** across L and N (optional) — clamps mains surges.

---

## Complete Pin Reference

| Component | GPIO | Direction | Voltage | Note |
|-----------|------|-----------|---------|------|
| OLED SDA | 21 | out | 3.3V | I2C |
| OLED SCL | 22 | out | 3.3V | I2C |
| DS18B20 DATA | 4 | in/out | 3.3V | 1-Wire, 4.7 kΩ pull-up |
| MAX6675 CS | 5 | out | 3.3V | SPI |
| MAX6675 SCK | 18 | out | 3.3V | SPI |
| MAX6675 MISO | 19 | in | 3.3V | SPI |
| Relay — Feeder | 25 | out | 3.3V | Active LOW |
| Relay — Pump | 27 | out | 3.3V | Active LOW |
| Dimmer CH1 — Blower | 33 | out | 3.3V | PWM to DIM1 |
| Dimmer CH2 — Igniter | 26 | out | 3.3V | PWM to DIM2 |
| Zero-cross detect | 35 | in | 3.3V | Input-only, falling edge ISR |
| Button On/Off | 13 | in | 3.3V | Internal pull-up |
| Button E-Stop | 14 | in | 3.3V | Internal pull-up |

---

## Bring-up Order (recommended)

1. **USB only** — flash firmware, confirm serial log boots cleanly.
2. **Add OLED** — confirm I2C display initialises (Phase 6 needed for content).
3. **Add DS18B20** — wire 4.7 kΩ pull-up, confirm water temp in serial log and web UI.
4. **Add MAX6675** — confirm flame temp reads ~20 °C at room temperature.
5. **Wire GND bus + HLK-PM01 DC side** — connect -Vo to bus, +Vo to ESP32 VIN and relay VCC. Add 100 µF + 100 nF at output. Do not connect AC yet.
6. **Add relay modules (feeder + pump)** — add 100 nF decoupling at each module, test each relay clicks.
7. **Add buttons** — add optional 100 nF debounce caps, test On/Off and E-Stop.
8. **Add AC dimmer module (DC side only)** — connect VCC, GND, ZC→GPIO35, DIM1→GPIO33, DIM2→GPIO26. Test PWM signals with a multimeter before connecting AC load.
9. **AC mains wiring** — very last step, after all logic is verified. Fuse the live wire. Connect HLK-PM01 L/N and dimmer module AC input. Optionally add MOV across L/N.
