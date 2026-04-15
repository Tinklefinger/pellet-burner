# Pellet Burner — Feature Specification

## 1. Web Interface Security

- HTTP Basic Auth on all web UI routes
- Hardcoded username and password (configurable via `config.h`)
- Unauthenticated requests return 401

---

## 2. Power Levels

Three power levels controlling fan speed, feeder ON time, and feeder OFF time.

| Level | Description | Activation condition |
|-------|-------------|----------------------|
| Power 1 | Weakest | Water temp within 5 °C of target |
| Power 2 | Medium | Water temp 5–15 °C below target |
| Power 3 | Strongest | Water temp more than 15 °C below target |

Each power level has independently configurable settings:

- Fan speed (%)
- Feeder ON duration (seconds)
- Feeder OFF duration (seconds)

### Thresholds (configurable)
- `power3_threshold` — default 15 °C below target → Power 3
- `power1_threshold` — default 5 °C below target → Power 1
- Between the two → Power 2

---

## 3. Operation Modes

### Standby
- Burner is off
- Pump follows pump start temperature rule (see §5)
- No feeding, no ignition, fan off

### Automatic
- Burner runs continuously
- Follows Power Management (§2) at all times
- Includes Economy Mode logic (§4)

### Timer
- Burner active only within configured time intervals
- Up to **3 time presets**, each with a start and end time (HH:MM)
- Only one preset active at a time (selectable in settings)
- Outside the active interval → behaves as Standby
- Inside the active interval → follows Power Management (§2) and Economy Mode (§4)

#### Timer preset example
| Preset | Start | End |
|--------|-------|-----|
| A | 18:00 | 09:00 |
| B | 07:00 | 22:00 |
| C | 06:30 | 08:00 |

---

## 4. Economy Mode

Goal: avoid unnecessary fuel burn when the boiler is already hot enough.

- If water temperature has been **at or above target** for a configurable hold time → enter Economy Mode
  - Turn off feeder and fan
  - Keep pump running per pump rules (§5)
- Exit Economy Mode when water temperature drops below `target - economy_resume_delta`
  - Resume normal power level operation

### Configurable settings
- `economy_hold_time` — how long temp must stay at target before shutting down (seconds, default 300)
- `economy_resume_delta` — how many °C below target triggers resume (default 5 °C)

---

## 5. Water Pump Control

- Pump does **not** start until water temperature reaches `pump_start_temp`
- Pump stays on while burner is running or water temp is above `pump_start_temp`
- Pump turns off when water temp drops below `pump_start_temp` and burner is in Standby/Economy

### Configurable settings
- `pump_start_temp` — minimum water temperature to activate pump (default 45 °C)

---

## 6. Configurable Settings Summary

All values below are adjustable via the web UI and persisted to NVS flash.

### General
| Setting | Description | Default |
|---------|-------------|---------|
| `target_temp` | Desired water temperature (°C) | 75 |
| `operation_mode` | Standby / Automatic / Timer | Standby |
| `active_timer_preset` | Which timer preset is active (A/B/C) | A |

### Power Levels
| Setting | Description | Default |
|---------|-------------|---------|
| `p1_fan_speed` | Power 1 fan speed (%) | 30 |
| `p1_feeder_on` | Power 1 feeder ON time (s) | 5 |
| `p1_feeder_off` | Power 1 feeder OFF time (s) | 60 |
| `p2_fan_speed` | Power 2 fan speed (%) | 60 |
| `p2_feeder_on` | Power 2 feeder ON time (s) | 8 |
| `p2_feeder_off` | Power 2 feeder OFF time (s) | 45 |
| `p3_fan_speed` | Power 3 fan speed (%) | 100 |
| `p3_feeder_on` | Power 3 feeder ON time (s) | 12 |
| `p3_feeder_off` | Power 3 feeder OFF time (s) | 30 |
| `power1_threshold` | °C below target for Power 1 | 5 |
| `power3_threshold` | °C below target for Power 3 | 15 |

### Economy Mode
| Setting | Description | Default |
|---------|-------------|---------|
| `economy_hold_time` | Seconds at target before economy (s) | 300 |
| `economy_resume_delta` | °C drop below target to resume (°C) | 5 |

### Pump
| Setting | Description | Default |
|---------|-------------|---------|
| `pump_start_temp` | Min water temp to activate pump (°C) | 45 |

### Timer Presets
| Setting | Description | Default |
|---------|-------------|---------|
| `timer_a_start` | Preset A start time (HH:MM) | 18:00 |
| `timer_a_end` | Preset A end time (HH:MM) | 09:00 |
| `timer_b_start` | Preset B start time (HH:MM) | 07:00 |
| `timer_b_end` | Preset B end time (HH:MM) | 22:00 |
| `timer_c_start` | Preset C start time (HH:MM) | 06:30 |
| `timer_c_end` | Preset C end time (HH:MM) | 08:00 |

### WiFi
| Setting | Description |
|---------|-------------|
| `wifi_ssid` | Network name |
| `wifi_pass` | Network password |
