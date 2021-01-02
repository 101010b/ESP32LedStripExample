# ESP32 APA102C (or SK9822) LED Strip driver Example

Features:
- Bluetooth LE Interface 
	Simple Android App is availlable.
	In case of Emergency, it can also be written to using Nordics nRF Connect or LightBlue (Android BLE Tools)
- MQTT 
	subscribes to configurable Topics for parameter changing and publishes the current values including ranges in JSON formatted messages.
	A Python script to read the MQTT data and create the correspodning OpenHAB2 things/items/sitemap files to access it.
- Current configuration is also stored in a local file in the SPIFFS

Hardware:
- ESP32-01 - ESP32 Module in the ESP-01 Form Factor 
- Logic-Level-Shifter to 5V
- SK9822 based LED Strip (e.g. 144 or 300 LEDs)

Software Environment to compile: Platform IO https://platformio.org/?utm_source=platformio&utm_medium=piohome

Hardware Links:
- ESP32-01: https://hackaday.io/project/171370-esp32-01
- Level Shifter and LED Strip Interface: https://hackaday.io/project/176648-apa102sk9822-led-driver-via-esp32-01

Linked Sources: https://github.com/101010b/ESP32MultiServiceIO
