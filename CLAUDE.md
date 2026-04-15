# Pellet Burner Controller

## Reference Projects

| Name | Path | Notes |
|------|------|-------|
| wifi example | `e:/Projects/Automation/Examples/esp32-idf-hello-wifi` | ESP32-IDF WiFi + HTTP server reference |
| hello display | `e:/Projects/Automation/Examples/esp-hello-display` | ESP32 display example with VSCode + Wokwi integration |

## Hardware

| Component | Model | Notes |
|-----------|-------|-------|
| Microcontroller | ESP32 | Built-in WiFi used for web UI |
| Display | 1.3" OLED SH1106 128×64 | Monochrome, I2C — at-a-glance status + custom bitmap icons for errors. Use U8g2 library. |
| Buttons | 2× physical | On/off and emergency stop only — settings via web UI |

## Architecture Decisions

- **Settings UI:** Web UI served directly from ESP32 (no cloud, no subscription). ESP32 hosts an HTML page over WiFi, settings saved to NVS flash.
- **Physical controls:** Minimal buttons for safety-critical actions only (on/off, emergency stop). All configuration via browser.
- **Display role:** OLED shows live status (temp, flame, pump, state) and bitmap error icons. Not used for settings navigation.
