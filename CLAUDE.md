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
| Water temp sensor | DS18B20 | OneWire, GPIO4, 4.7kΩ pull-up to 3.3V |
| Flame temp sensor | MAX6675 + K-type thermocouple | SPI (CS=GPIO5, SCK=GPIO18, MISO=GPIO19) |
| Relays | 2× 5V relay module | Feeder=GPIO25, Pump=GPIO27 — active LOW, VCC=5V |
| Blower + Igniter control | Hipzeepo 2-ch AC dimmer module (3.3V logic) | Phase-angle PWM: blower=GPIO33 (DIM1), igniter=GPIO26 (DIM2), zero-cross=GPIO35 (ZC). Snubber + optocouplers built in. |
| Power supply | HLK-PM01 (AC 220V → DC 5V 1A) | Powers ESP32 (VIN) and relay coils. -Vo = DC common GND. ESP32 3.3V reg powers sensors. [Amazon DE](https://www.amazon.de/HLK-PM01-Haushalt-Netzteilmodul-Power-Supply-2-x/dp/B073QH1XT8) |

## VSCode Extensions (Development Tooling)

| Extension | ID | How it helps |
|-----------|-----|--------------|
| ESP-IDF | `espressif.esp-idf-extension` | Build / Flash / Monitor buttons; built-in serial monitor for live ESP32 logs; auto-detects COM port; fixes IntelliSense by pointing VSCode at ESP-IDF headers (run **ESP-IDF: Configure ESP-IDF Extension** once to set up) |
| C/C++ IntelliSense | `ms-vscode.cpptools` | Jump-to-definition, hover docs, error squiggles in `.cpp`/`.h` files |
| Wokwi Simulator | `wokwi.wokwi-vscode` | Simulate ESP32 firmware without real hardware — useful for testing state machine logic (Phase 5+) before full wiring is complete. See `hello display` reference project for example setup. |
| CMake Tools | `ms-vscode.cmake-tools`, `twxs.cmake` | CMake syntax highlighting and project awareness for `CMakeLists.txt` files |

**Division of labour:** Claude writes and edits code. The ESP-IDF extension handles build, flash, and serial monitoring — those require direct hardware access that Claude cannot do.

## Architecture Decisions

- **Settings UI:** Web UI served directly from ESP32 (no cloud, no subscription). ESP32 hosts an HTML page over WiFi, settings saved to NVS flash.
- **Physical controls:** Minimal buttons for safety-critical actions only (on/off, emergency stop). All configuration via browser.
- **Display role:** OLED shows live status (temp, flame, pump, state) and bitmap error icons. Not used for settings navigation.
