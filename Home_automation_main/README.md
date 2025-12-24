# Web-Based Home Automation

## Introduction
**SmartSense** is an IoT-based home automation project designed to make life easier and safer.It allows users to control home appliances remotely via a web interface while actively monitoring the environment for safety hazards like gas leaks or fire.Built using the **Raspberry Pi Pico W**.
## Key Features
* **Remote Control:** Turn bulbs ON/OFF and control fan speed wirelessly via a web page.
* **Smart Geyser Logic:** Automatically turns the geyser ON if the room temperature drops below **27°C** and OFF when it gets warm.
* **Safety Alerts:** Detects Gas (LPG/Methane) and Smoke using the MQ2 sensor and triggers immediate alerts.
* **Real-Time Monitoring:** Displays live Temperature and Humidity data on the dashboard.
* **Responsive Interface:** Works on smartphones, tablets, and laptop.

## Hardware Required
* **Microcontroller:** Raspberry Pi Pico W 
* **Sensors:**
    * DHT11 (Temperature & Humidity)
    * MQ2 (Gas & Smoke)
* **Actuators:**
    * Relay Module (for Bulb control)
    * L298N Motor Driver (for Fan speed control)
    * 12V DC Fan
    * AC LED Bulb
* **Power:** 5V/12V Power Supply
