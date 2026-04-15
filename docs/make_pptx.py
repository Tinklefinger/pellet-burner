"""
Generates docs/Pellet_Burner_Components.pptx
Run with: python3 make_pptx.py   (from inside docs/)
"""
import os
from pptx import Presentation
from pptx.util import Inches, Pt, Emu
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.util import Inches, Pt

# ── Colour palette ────────────────────────────────────────────────────────────
BG        = RGBColor(0x1a, 0x1a, 0x2e)   # dark navy  (body bg)
CARD      = RGBColor(0x16, 0x21, 0x3e)   # slightly lighter navy
ACCENT    = RGBColor(0xe9, 0x45, 0x60)   # red-pink   (headings)
TEAL      = RGBColor(0xa8, 0xda, 0xdc)   # teal       (sub-headings)
WHITE     = RGBColor(0xff, 0xff, 0xff)
GREY      = RGBColor(0xaa, 0xaa, 0xaa)
HIGHLIGHT = RGBColor(0x4c, 0xaf, 0x50)   # green

SLIDE_W = Inches(13.33)
SLIDE_H = Inches(7.5)

IMG_DIR = os.path.join(os.path.dirname(__file__), "img")

prs = Presentation()
prs.slide_width  = SLIDE_W
prs.slide_height = SLIDE_H

BLANK = prs.slide_layouts[6]   # completely blank


# ── Helpers ───────────────────────────────────────────────────────────────────
def bg_rect(slide):
    """Full-slide background rectangle."""
    sh = slide.shapes.add_shape(1, 0, 0, SLIDE_W, SLIDE_H)
    sh.fill.solid()
    sh.fill.fore_color.rgb = BG
    sh.line.fill.background()
    return sh


def add_rect(slide, x, y, w, h, fill=CARD, line_color=None):
    sh = slide.shapes.add_shape(1, x, y, w, h)
    sh.fill.solid()
    sh.fill.fore_color.rgb = fill
    if line_color:
        sh.line.color.rgb = line_color
        sh.line.width = Pt(1)
    else:
        sh.line.fill.background()
    return sh


def add_text(slide, text, x, y, w, h,
             size=18, bold=False, color=WHITE, align=PP_ALIGN.LEFT, wrap=True):
    txb = slide.shapes.add_textbox(x, y, w, h)
    tf  = txb.text_frame
    tf.word_wrap = wrap
    p   = tf.paragraphs[0]
    p.alignment = align
    run = p.add_run()
    run.text = text
    run.font.size  = Pt(size)
    run.font.bold  = bold
    run.font.color.rgb = color
    return txb


def add_bullet_box(slide, lines, x, y, w, h, size=14, title=None):
    """Card with optional title and bullet lines."""
    add_rect(slide, x, y, w, h, fill=CARD)
    cur_y = y + Inches(0.12)
    if title:
        add_text(slide, title, x + Inches(0.15), cur_y,
                 w - Inches(0.3), Inches(0.35),
                 size=13, bold=True, color=TEAL)
        cur_y += Inches(0.38)
    for line in lines:
        is_head = line.startswith("##")
        is_val  = line.startswith(">>")
        txt = line.lstrip("#>").strip()
        col = TEAL if is_head else (HIGHLIGHT if is_val else WHITE)
        sz  = 12 if is_head else (size - 1 if is_val else size)
        add_text(slide, ("• " if not is_head and not is_val else "") + txt,
                 x + Inches(0.18), cur_y, w - Inches(0.36), Inches(0.28),
                 size=sz, bold=is_head, color=col)
        cur_y += Inches(0.27)


def place_image(slide, path, x, y, w, h, label=None):
    """Place image if file exists, otherwise a labelled placeholder."""
    if os.path.exists(path):
        slide.shapes.add_picture(path, x, y, width=w, height=h)
    else:
        sh = add_rect(slide, x, y, w, h, fill=RGBColor(0x0f, 0x34, 0x60),
                      line_color=TEAL)
        lbl = label or os.path.basename(path)
        add_text(slide, lbl, x, y + h//2 - Inches(0.2), w, Inches(0.4),
                 size=12, color=TEAL, align=PP_ALIGN.CENTER)


def divider(slide, y):
    sh = slide.shapes.add_shape(1, Inches(0.4), y, SLIDE_W - Inches(0.8), Pt(1.5))
    sh.fill.solid()
    sh.fill.fore_color.rgb = ACCENT
    sh.line.fill.background()


# ══════════════════════════════════════════════════════════════════════════════
# Slide 1 — Title
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

# Accent bar top
sh = slide.shapes.add_shape(1, 0, 0, SLIDE_W, Inches(0.08))
sh.fill.solid(); sh.fill.fore_color.rgb = ACCENT; sh.line.fill.background()

# Accent bar bottom
sh = slide.shapes.add_shape(1, 0, SLIDE_H - Inches(0.08), SLIDE_W, Inches(0.08))
sh.fill.solid(); sh.fill.fore_color.rgb = ACCENT; sh.line.fill.background()

add_text(slide, "Pellet Burner Controller",
         Inches(1), Inches(2.2), Inches(11.33), Inches(1.2),
         size=48, bold=True, color=ACCENT, align=PP_ALIGN.CENTER)

add_text(slide, "Hardware Components & Wiring Guide",
         Inches(1), Inches(3.5), Inches(11.33), Inches(0.7),
         size=26, color=TEAL, align=PP_ALIGN.CENTER)

add_text(slide, "ESP32  ·  SH1106 OLED  ·  DS18B20  ·  MAX6675  ·  Relays  ·  TRIAC  ·  Buttons",
         Inches(1), Inches(4.35), Inches(11.33), Inches(0.5),
         size=15, color=GREY, align=PP_ALIGN.CENTER)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 2 — ESP32 DevKit V1
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "ESP32 DevKit V1", Inches(0.4), Inches(0.15),
         Inches(8), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

# Image
place_image(slide, os.path.join(IMG_DIR, "esp32.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(5.8), "ESP32 DevKit V1")

# Specs card
add_bullet_box(slide, [
    "## Key Specifications",
    "Dual-core Xtensa LX6 @ 240 MHz",
    "520 KB SRAM  |  4 MB Flash",
    "Built-in 802.11 b/g/n WiFi + Bluetooth 4.2",
    "3.3 V logic — NOT 5 V tolerant",
    "30 GPIO pins (several input-only)",
    "ADC, DAC, SPI, I2C, UART, PWM, touch",
    "## Role in this project",
    "Main controller — runs FreeRTOS tasks",
    "Hosts HTTP web server for settings UI",
    "Controls all outputs (relays, TRIAC)",
    "Reads all sensors (DS18B20, MAX6675)",
    "Drives OLED display via I2C",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(5.6), size=14)

# Power card
add_bullet_box(slide, [
    "## Power Supply",
    "USB 5V during development",
    "VIN pin: 5–12V regulated input",
    "3.3V pin: output — do NOT feed 5V here",
    "Recommended: separate 5V/3A PSU for",
    "production (relays draw surge current)",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(3.1), size=13)

# GPIO summary card
add_bullet_box(slide, [
    "## GPIO Pin Assignments",
    "GPIO4   DS18B20 OneWire data",
    "GPIO5   MAX6675 SPI CS",
    "GPIO18  MAX6675 SPI SCK",
    "GPIO19  MAX6675 SPI MISO",
    "GPIO21  OLED SDA (I2C)",
    "GPIO22  OLED SCL (I2C)",
    "GPIO25  Relay — Feeder motor",
    "GPIO26  Relay — Igniter",
    "GPIO27  Relay — Water pump",
    "GPIO33  TRIAC gate trigger",
    "GPIO35  Zero-cross detect (input)",
    "GPIO13  Button: On/Off",
    "GPIO14  Button: Emergency stop",
], Inches(5.9), Inches(4.05), Inches(3.2), Inches(2.37), size=11)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 3 — SH1106 OLED Display
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "SH1106 OLED Display  —  1.3\"  128×64", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "sh1106.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(3.5), "SH1106 OLED")

add_bullet_box(slide, [
    "## Specifications",
    "Driver IC: SH1106  (note: NOT SSD1306)",
    "Size: 1.3 inch diagonal",
    "Resolution: 128 × 64 pixels  (monochrome)",
    "Interface: I2C (default address 0x3C)",
    "Operating voltage: 3.3V – 5V",
    "Display area: ~26.8 × 15.1 mm",
    "Library: U8g2 (configured for SH1106)",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(3.6), size=14)

add_bullet_box(slide, [
    "## Wiring to ESP32",
    "VCC  →  3.3V",
    "GND  →  GND",
    "SDA  →  GPIO21",
    "SCL  →  GPIO22",
    "## Notes",
    "No additional pull-up resistors needed",
    "(ESP32 internal pull-ups sufficient at",
    "400 kHz I2C — or add 4.7kΩ externally)",
    "I2C address: 0x3C (some boards 0x3D)",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(3.6), size=13)

add_bullet_box(slide, [
    "## Role in this project",
    "At-a-glance live status without phone/browser:",
    "• Water temperature  (DS18B20)",
    "• Flame temperature  (MAX6675)",
    "• Pump / Feeder / Blower state",
    "• Current state machine state",
    "• Active operation mode",
    "• Error icons (custom bitmap) on faults",
    "## Important: SH1106 ≠ SSD1306",
    "Many cheap 1.3\" modules use SH1106.",
    "U8g2 constructor: U8G2_SH1106_128X64_NONAME",
], Inches(0.3), Inches(4.55), Inches(8.8), Inches(2.8), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 4 — DS18B20 Water Temperature Sensor
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "DS18B20  —  Water Temperature Sensor", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "ds18b20.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(3.8), "DS18B20")

add_bullet_box(slide, [
    "## Specifications",
    "Protocol: 1-Wire (OneWire)",
    "Range: –55°C to +125°C",
    "Accuracy: ±0.5°C from –10°C to +85°C",
    "Resolution: 9–12 bit (configurable)",
    "Supply: 3.0V – 5.5V (or parasitic 2-wire)",
    "Package: TO-92 or waterproof probe (DS18B20+PAR)",
    "Multiple sensors on one wire (unique 64-bit ROM)",
    "Conversion time: 94ms (9-bit) to 750ms (12-bit)",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(3.8), size=14)

add_bullet_box(slide, [
    "## Wiring to ESP32 (normal mode)",
    "VCC (red)   →  3.3V",
    "GND (black) →  GND",
    "DATA (yellow) →  GPIO4",
    "4.7 kΩ pull-up: DATA ↔ VCC",
    "## Parasitic / 2-wire mode",
    "GND (black) →  GND",
    "VCC (red)   →  GND  (tied low)",
    "DATA (yellow) →  GPIO4",
    "4.7 kΩ pull-up: DATA ↔ 3.3V",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(3.8), size=13)

add_bullet_box(slide, [
    "## Role in this project",
    "Measures water / boiler temperature",
    "Used for: pump start/stop hysteresis,",
    "  power level selection (P1/P2/P3),",
    "  economy mode logic,",
    "  target temperature control loop",
    "## Recommendation",
    "Use waterproof stainless steel probe version.",
    "Route cable away from mains wiring.",
    "12-bit resolution (0.0625°C) preferred.",
], Inches(0.3), Inches(4.75), Inches(8.8), Inches(2.6), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 5 — MAX6675 Thermocouple Module
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "MAX6675  —  Flame Thermocouple Module", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "max6675.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(3.5), "MAX6675 Module")

add_bullet_box(slide, [
    "## Specifications",
    "Sensor type: K-type thermocouple",
    "Temperature range: 0°C to +1023.75°C",
    "Resolution: 0.25°C (12-bit)",
    "Interface: SPI (read-only, 3-wire)",
    "Supply voltage: 3.0V – 5.5V",
    "Internal cold-junction compensation",
    "Open thermocouple detection (fault flag)",
    "Max SPI clock: 4.3 MHz",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(3.6), size=14)

add_bullet_box(slide, [
    "## Wiring to ESP32",
    "VCC   →  3.3V",
    "GND   →  GND",
    "CS    →  GPIO5",
    "SCK   →  GPIO18",
    "SO (MISO) →  GPIO19",
    "## K-type thermocouple",
    "Yellow (+)  →  T+ on module",
    "Red/White (–) →  T– on module",
    "Tip exposed to flame",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(3.6), size=13)

add_bullet_box(slide, [
    "## Role in this project",
    "Measures flame / combustion chamber temperature",
    "Used for: flame detection (above ~100°C = flame on),",
    "  ignition confirmation, overheat protection,",
    "  OLED display readout",
    "## Notes",
    "Module uses dedicated SPI — no sharing with other devices.",
    "Keep thermocouple wiring short and twisted-pair.",
    "MAX6675 reads bit-15=0 as fault (open thermocouple).",
    "Use alumel/chromel K-type, rated to at least 900°C.",
], Inches(0.3), Inches(4.55), Inches(8.8), Inches(2.8), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 6 — Relay Modules
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "5V Relay Modules  —  Feeder / Igniter / Pump", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "relay.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(3.5), "5V Relay Module")

add_bullet_box(slide, [
    "## Specifications",
    "Coil voltage: 5V DC  (logic-level trigger)",
    "Trigger: active LOW  (LOW = relay ON)",
    "Contact rating: 10A @ 250V AC / 10A @ 30V DC",
    "Isolation: optocoupler between ESP32 and coil",
    "Channels: 1 or 3 (one module per relay or 3-ch board)",
    "Flyback diode: included on module",
    "Indicator LED per channel",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(3.3), size=14)

# Three relay wiring columns
for i, (name, gpio, label) in enumerate([
    ("Feeder Motor", "GPIO25", "Pellet auger / screw conveyor"),
    ("Igniter",      "GPIO26", "Electric heating element"),
    ("Water Pump",   "GPIO27", "Circulation pump"),
]):
    bx = Inches(0.3) + i * Inches(2.95)
    add_bullet_box(slide, [
        f"## {name}",
        f"IN  →  {gpio}",
        "VCC →  5V",
        "GND →  GND",
        "COM →  AC Live",
        "NO  →  Load",
        label,
    ], bx, Inches(4.2), Inches(2.85), Inches(3.1), size=13)

add_bullet_box(slide, [
    "## Active LOW wiring note",
    "ESP32 GPIO initialised HIGH at boot →",
    "relay stays OFF during startup / reset",
    "Set GPIO LOW to activate (energise) relay",
    "Set GPIO HIGH to deactivate",
    "## Safety",
    "All relays default OFF on power-up",
    "Emergency stop → all GPIOs HIGH immediately",
], Inches(9.0), Inches(4.2), Inches(4.1), Inches(3.1), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 7 — TRIAC Blower Control
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "TRIAC Dimmer  —  Blower Motor Speed Control", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "triac.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(4.0), "TRIAC / AC Dimmer Module")

add_bullet_box(slide, [
    "## Method: Phase-angle control",
    "The AC waveform is 'chopped' each half-cycle.",
    "The TRIAC fires later in the cycle → less power.",
    "A zero-cross detector tells the ESP32 exactly",
    "when the AC wave crosses 0V.",
    "ESP32 waits N microseconds, then pulses the",
    "TRIAC gate → conducts for the rest of the cycle.",
    "Larger delay = lower speed.  Delay = 0 = full power.",
    "## Typical components",
    "BT136 or BTA16 TRIAC (TO-220, isolated tab)",
    "MOC3021 optocoupler (gate isolation)",
    "4N35 optocoupler (zero-cross isolation)",
    "Snubber: 100Ω + 100nF/400V across TRIAC",
    "Gate resistor: 330Ω between optocoupler and gate",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(5.7), size=13)

add_bullet_box(slide, [
    "## Zero-cross input",
    "GPIO35  ←  Zero-cross signal",
    "(GPIO35 is input-only on ESP32)",
    "Signal: 3.3V HIGH, drops to LOW at",
    "each zero crossing of the AC wave",
    "Trigger ISR on falling edge",
    "## TRIAC gate output",
    "GPIO33  →  MOC3021 input LED",
    "  via 330Ω resistor",
    "MOC3021 output → TRIAC gate",
    "## Voltage levels",
    "ESP32 side: 3.3V logic (optocoupled)",
    "AC side: 230V AC — DANGER",
    "Keep AC and DC wiring separated",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(5.7), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 8 — Physical Buttons
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "Physical Buttons  —  On/Off & Emergency Stop", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

place_image(slide, os.path.join(IMG_DIR, "button.jpg"),
            Inches(9.3), Inches(0.9), Inches(3.6), Inches(3.2), "Push Button")

add_bullet_box(slide, [
    "## Button type",
    "Momentary normally-open (NO) push button",
    "Panel-mount, rated for frequent use",
    "Recommended: latching for On/Off,",
    "  momentary (NO) for Emergency Stop",
    "## Wiring (active LOW, internal pull-up)",
    "One terminal → GPIO pin",
    "Other terminal → GND",
    "ESP32 internal pull-up enabled in firmware",
    "No external resistors required",
    "Button pressed → GPIO reads LOW",
    "Button released → GPIO reads HIGH",
], Inches(0.3), Inches(0.82), Inches(5.5), Inches(4.3), size=14)

add_bullet_box(slide, [
    "## Button 1 — On / Off",
    "GPIO13  ←  button  ←  GND",
    "Function: toggle between Standby",
    "and the previously active mode",
    "(Automatic or Timer)",
    "",
    "## Button 2 — Emergency Stop",
    "GPIO14  ←  button  ←  GND",
    "Function: immediate shutdown —",
    "ALL outputs OFF, state → STANDBY",
    "Does NOT save op_mode to NVS",
    "(avoids writing flash on every estop)",
], Inches(5.9), Inches(0.82), Inches(3.2), Inches(4.3), size=13)

add_bullet_box(slide, [
    "## Debounce strategy",
    "Hardware: 100nF capacitor from GPIO pin to GND (optional).",
    "Software: ignore transitions for 50ms after first edge.",
    "Emergency stop: no debounce delay — act immediately on first LOW edge.",
], Inches(0.3), Inches(5.25), Inches(8.8), Inches(1.5), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 9 — Full Wiring Summary
# ══════════════════════════════════════════════════════════════════════════════
slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "Complete Wiring Summary", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

COL1 = Inches(0.3)
COL2 = Inches(4.55)
COL3 = Inches(8.8)
ROW1 = Inches(0.82)
ROW2 = Inches(3.85)

add_bullet_box(slide, [
    "## I2C Bus  (SDA=GPIO21, SCL=GPIO22)",
    "SH1106 OLED  —  addr 0x3C",
    "VCC → 3.3V   GND → GND",
    "SDA → GPIO21   SCL → GPIO22",
    "## OneWire Bus  (GPIO4)",
    "DS18B20 water temp sensor",
    "DATA → GPIO4  (4.7kΩ to 3.3V)",
    "VCC → 3.3V   GND → GND",
], COL1, ROW1, Inches(4.1), Inches(2.85), size=13)

add_bullet_box(slide, [
    "## SPI Bus  (dedicated)",
    "MAX6675 thermocouple module",
    "CS   → GPIO5",
    "SCK  → GPIO18",
    "MISO → GPIO19",
    "VCC  → 3.3V   GND → GND",
    "## Zero-cross detect",
    "Signal → GPIO35 (input only)",
    "3.3V logic, falling edge ISR",
], COL2, ROW1, Inches(4.1), Inches(2.85), size=13)

add_bullet_box(slide, [
    "## Relay outputs  (active LOW)",
    "GPIO25 → Relay IN  — Feeder motor",
    "GPIO26 → Relay IN  — Igniter",
    "GPIO27 → Relay IN  — Pump",
    "Relay VCC → 5V   GND → GND",
    "## TRIAC gate",
    "GPIO33 → 330Ω → MOC3021 LED(+)",
    "MOC3021 output → TRIAC gate",
], COL3, ROW1, Inches(4.1), Inches(2.85), size=13)

add_bullet_box(slide, [
    "## Buttons  (active LOW, internal pull-up)",
    "GPIO13 ← On/Off button ← GND",
    "GPIO14 ← Emergency Stop button ← GND",
], COL1, ROW2, Inches(4.1), Inches(1.4), size=13)

add_bullet_box(slide, [
    "## Power rails summary",
    "3.3V: ESP32 onboard reg, OLED, DS18B20, MAX6675",
    "5V: Relay module VCC (from USB or regulator)",
    "AC 230V: TRIAC load side only — fully isolated",
], COL2, ROW2, Inches(4.1), Inches(1.4), size=13)

add_bullet_box(slide, [
    "## Safety reminders",
    "Never connect 5V to ESP32 3.3V rail",
    "Optocouplers required on TRIAC side",
    "Relay contacts rated for your AC load",
    "Fuse the AC circuit (5–10A slow blow)",
], COL3, ROW2, Inches(4.1), Inches(1.4), size=13)


# ══════════════════════════════════════════════════════════════════════════════
# Slide 10 — Pin Reference Table
# ══════════════════════════════════════════════════════════════════════════════
from pptx.oxml.ns import qn
from lxml import etree

slide = prs.slides.add_slide(BLANK)
bg_rect(slide)

add_text(slide, "Pin Reference Table", Inches(0.4), Inches(0.15),
         Inches(9), Inches(0.55), size=28, bold=True, color=ACCENT)
divider(slide, Inches(0.72))

headers = ["Component", "GPIO", "Protocol", "Voltage", "Notes"]
rows = [
    ["SH1106 OLED — SDA",   "GPIO21", "I2C",    "3.3 V", "I2C address 0x3C"],
    ["SH1106 OLED — SCL",   "GPIO22", "I2C",    "3.3 V", "400 kHz fast-mode"],
    ["DS18B20 DATA",        "GPIO4",  "1-Wire", "3.3 V", "4.7 kΩ pull-up to 3.3 V"],
    ["MAX6675 CS",          "GPIO5",  "SPI",    "3.3 V", "dedicated SPI bus"],
    ["MAX6675 SCK",         "GPIO18", "SPI",    "3.3 V", "max 4.3 MHz"],
    ["MAX6675 MISO (SO)",   "GPIO19", "SPI",    "3.3 V", "read-only from module"],
    ["Relay — Feeder IN",   "GPIO25", "GPIO",   "3.3 V", "active LOW, relay VCC = 5 V"],
    ["Relay — Igniter IN",  "GPIO26", "GPIO",   "3.3 V", "active LOW, relay VCC = 5 V"],
    ["Relay — Pump IN",     "GPIO27", "GPIO",   "3.3 V", "active LOW, relay VCC = 5 V"],
    ["TRIAC gate",          "GPIO33", "GPIO",   "3.3 V", "330 Ω → MOC3021 → TRIAC gate"],
    ["Zero-cross detect",   "GPIO35", "GPIO",   "3.3 V", "input-only pin, falling edge ISR"],
    ["Button — On/Off",     "GPIO13", "GPIO",   "3.3 V", "internal pull-up, active LOW"],
    ["Button — E-Stop",     "GPIO14", "GPIO",   "3.3 V", "internal pull-up, active LOW"],
]

NUM_ROWS = 1 + len(rows)   # header + data
NUM_COLS = len(headers)

TBL_LEFT = Inches(0.3)
TBL_TOP  = Inches(0.9)
TBL_W    = SLIDE_W - Inches(0.6)
TBL_H    = SLIDE_H - Inches(1.1)

tbl_shape = slide.shapes.add_table(NUM_ROWS, NUM_COLS, TBL_LEFT, TBL_TOP, int(TBL_W), int(TBL_H))
tbl = tbl_shape.table

# Column widths (proportional)
col_fracs = [0.26, 0.10, 0.10, 0.09, 0.45]
for ci, frac in enumerate(col_fracs):
    tbl.columns[ci].width = int(TBL_W * frac)

# Row heights — header taller, data rows equal
HDR_H  = Inches(0.46)
DATA_H = int((TBL_H - HDR_H) / len(rows))
tbl.rows[0].height = int(HDR_H)
for ri in range(1, NUM_ROWS):
    tbl.rows[ri].height = DATA_H


def set_cell(tbl, ri, ci, text,
             bold=False, font_size=13,
             fg=WHITE, bg_color=None, align=PP_ALIGN.LEFT):
    cell = tbl.cell(ri, ci)
    cell.text = ""
    tf = cell.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.alignment = align
    run = p.add_run()
    run.text = text
    run.font.size  = Pt(font_size)
    run.font.bold  = bold
    run.font.color.rgb = fg
    # Cell fill
    tc = cell._tc
    tcPr = tc.get_or_add_tcPr()
    solidFill = etree.SubElement(tcPr, qn('a:solidFill'))
    srgbClr   = etree.SubElement(solidFill, qn('a:srgbClr'))
    c = bg_color if bg_color else BG
    srgbClr.set('val', f'{int(c[0]):02X}{int(c[1]):02X}{int(c[2]):02X}')
    # Remove border lines
    for side in ('lnL', 'lnR', 'lnT', 'lnB'):
        ln = etree.SubElement(tcPr, qn(f'a:{side}'))
        noFill = etree.SubElement(ln, qn('a:noFill'))


# Header row
for ci, hdr in enumerate(headers):
    set_cell(tbl, 0, ci, hdr, bold=True, font_size=14,
             fg=WHITE, bg_color=ACCENT, align=PP_ALIGN.CENTER)

# Data rows
ROW_EVEN = RGBColor(0x0f, 0x34, 0x60)
ROW_ODD  = CARD

for ri, row in enumerate(rows):
    bg = ROW_EVEN if ri % 2 == 0 else ROW_ODD
    for ci, val in enumerate(row):
        if ci == 1:          fg = TEAL       # GPIO column
        elif ci == 4:        fg = GREY       # Notes column
        else:                fg = WHITE
        set_cell(tbl, ri + 1, ci, val, font_size=13, fg=fg, bg_color=bg)


# ── Save ──────────────────────────────────────────────────────────────────────
out = os.path.join(os.path.dirname(__file__), "Pellet_Burner_Components.pptx")
prs.save(out)
print(f"Saved: {out}")
