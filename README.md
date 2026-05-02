# ESP32 WiFi Scanner

A lightweight ESP32 project that scans nearby WiFi networks and displays their information in real-time on an I2C LCD screen.

---

### Features

- Scan nearby WiFi networks (hidden networks included)
- Display SSID, signal strength (RSSI), and encryption type  
- Automatic refresh every 15 seconds  
- Paginated view for multiple networks  
- Serial output for debugging and logs  
- Lightweight and efficient  

---

### Dependencies and hardware required

- WiFi.h
- Wire.h
- LiquidCrystal_I2C.h

- ESP32  
- I2C LCD (16x2)

---

### Wiring (I2C)

| LCD | ESP32 |
|-----|------|
| VCC | 5V / 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

### How It Works

- ESP32 initializes WiFi in station mode  
- Scans nearby networks (including hidden)  
- Displays:
   - SSID  
   - RSSI (signal strength)  
   - Encryption type  
- Cycles through networks  
- Re-scans periodically  

---

### Configuration

- Adjust the timing behavior of the scanner by modifying the following values in the code:

```cpp
const unsigned long PAGE_MS = 2500UL;
const unsigned long SCAN_INTERVAL_MS = 15000UL;
