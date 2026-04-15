# Pellet Burner — Implementation Plan

Track feature implementation progress here. Update status as work progresses.
Status: [ ] = pending, [~] = in progress, [x] = done

---

## Phase 1 — Project Skeleton
- [x] Project structure created (CMakeLists, sdkconfig.defaults, .gitignore)
- [x] Converted to C++
- [x] `Settings` class with NVS load/save
- [x] `WiFiManager` class with reconnect logic
- [x] `WebServer` class with embedded HTML UI
- [x] `BurnerStatus` struct (shared live state)
- [x] GPIO init for relays, TRIAC, buttons
- [x] Stub sensor task (DS18B20 + MAX6675 placeholders)
- [x] Stub control task (state machine placeholder)
- [x] FEATURES.md written

---

## Phase 2 — Settings Expansion
- [x] Expand `Settings` class to include all fields from FEATURES.md:
  - [x] Per power-level: fan speed, feeder on/off (P1/P2/P3)
  - [x] Power thresholds (power1_threshold, power3_threshold)
  - [x] Economy mode: hold time, resume delta
  - [x] Pump start temperature
  - [x] Operation mode (Standby / Automatic / Timer)
  - [x] Timer presets A/B/C (start + end time HH:MM)
  - [x] Active timer preset selector
- [x] Update NVS save/load for all new fields
- [x] Update web UI settings form for all new fields

---

## Phase 3 — Web Interface
- [x] HTTP Basic Auth (hardcoded user/pass from config.h)
- [x] Settings page: all fields from Phase 2 rendered as form inputs
- [x] Status page: live refresh every 2s (already scaffolded)
- [x] Operation mode selector (Standby / Automatic / Timer)
- [x] Timer preset editor (3 presets, start/end time)
- [x] Active preset selector

---

## Phase 4 — Sensor Drivers
- [x] DS18B20 OneWire driver → `g_status.water_temp`
- [x] MAX6675 SPI driver → `g_status.flame_temp`, `g_status.flame_on`

---

---

## ✦ Hardware Bring-up Checkpoint (after Phase 4, before Phase 5)

This is the right point to wire up the real hardware and verify each
subsystem works before the state machine depends on it.

**Minimum hardware needed to start:**
- [ ] ESP32 DevKit powered via USB
- [ ] SH1106 OLED wired (VCC→3.3V, GND, SDA→GPIO21, SCL→GPIO22)
- [ ] DS18B20 wired (DATA→GPIO4, 4.7kΩ pull-up to 3.3V)
- [ ] MAX6675 + K-type thermocouple wired (CS→GPIO5, SCK→GPIO18, MISO→GPIO19)
- [ ] WiFi credentials set in `Kconfig.projbuild` / NVS

**Verification steps:**
- [ ] Flash firmware (`idf.py build flash`)
- [ ] Open serial monitor — confirm DS18B20 reads a plausible water temp
- [ ] Confirm MAX6675 reads ambient temp (~20 °C) when thermocouple is cold
- [ ] Open web UI in browser — Live Status shows real temperature values
- [ ] Confirm `flame_on = false` at room temperature

**Add relays + TRIAC only when Phase 5 (state machine) is ready.**
AC wiring should be the last step — do all low-voltage testing first.

---

## Phase 5 — State Machine & Control Logic
- [ ] Burner states: STANDBY → IGNITING → RUNNING → ECONOMY → COOLING → STANDBY
- [ ] Power level selection logic (P1/P2/P3 based on temp delta)
- [ ] Feeder timing task (on/off intervals per active power level)
- [ ] AC dimmer CH1 blower control (fan speed % per active power level)
- [ ] AC dimmer CH2 igniter soft-start: ramp PWM 0→100% over ~1–2 s at IGNITING entry, then hold 100%; 0% on exit
- [ ] Pump relay control (temp-gated by pump_start_temp)
- [ ] Economy mode: detect hold at target → shutdown feeder+fan → resume on delta drop
- [ ] Operation mode enforcement:
  - [ ] Standby: keep everything off
  - [ ] Automatic: run state machine continuously
  - [ ] Timer: check RTC time against active preset, gate state machine

---

## Phase 6 — OLED Display (SH1106 via U8g2)
- [ ] U8g2 library integration
- [ ] Status screen: water temp, flame temp, pump, feeder, blower, state
- [ ] Error screen: custom bitmap icons for fault conditions
- [ ] Mode indicator: Standby / Auto / Timer + active preset

---

## Phase 7 — Physical Buttons
- [ ] On/Off button: toggle Standby ↔ previous mode
- [ ] Emergency stop button: immediate STANDBY + all outputs off

---

## Phase 8 — Time Sync (for Timer mode)
- [ ] SNTP client to sync RTC over WiFi
- [ ] Fallback: manual time set via web UI if no internet

---

## Phase 9 — Hardening & Polish
- [ ] Watchdog timer (TWDT) on control task
- [ ] WiFi reconnect loop (already partially done in WiFiManager)
- [ ] NVS corruption recovery
- [ ] OTA firmware update via web UI
- [ ] First-boot AP mode if no WiFi credentials saved

---

## Open Questions (resolve before implementing)
- [ ] Economy → resume: does burner re-ignite or does flame stay alive (starved)?
- [ ] Standby: fully extinguish flame or just idle?
- [ ] Web auth: hardcoded only, or also editable via web UI?
