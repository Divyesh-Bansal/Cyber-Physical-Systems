Vibration Detection for Machinery Safety System
Divyesh Bansal (23112036)
IIT Roorkee

==============================
1. Project Overview
==============================
This project monitors machinery for unsafe conditions using an Arduino UNO. It detects:
- Abnormal vibrations or tilt (SW-200D sensor)
- Unauthorized access (Reed switch)
- Environmental conditions: temperature and humidity (DHT11)

The system provides real-time alerts through an RGB LED, an LCD display, and activates safety mechanisms using a relay.

==============================
2. Hardware Components
==============================
- Arduino UNO
- SW-200D Vibration/Tilt Sensor (D2)
- Reed Switch for access detection (D3)
- DHT11 Temperature and Humidity Sensor (D4)
- RGB LED with resistors (RED=D9, GREEN=D10, BLUE=D11)
- Relay Module to control safety mechanisms (D7)
- 16x2 I2C LCD (SDA=A4, SCL=A5)

==============================
3. Circuit Design
==============================
- Vibration and reed switches use internal pull-ups.
- RGB LED indicates status:
  - Green = Safe
  - Red = Unsafe
- Relay activates external safety mechanisms when unsafe conditions are detected.
- LCD displays current status and sensor readings.
- Ensure proper power connections: 5V for relay and sensors, GND shared.

==============================
4. Working Principle
==============================
1. Arduino continuously monitors vibration and door/access status.
2. Temperature and humidity are read periodically.
3. Unsafe conditions (vibration, unauthorized access, high temperature >55°C, humidity >100%) trigger:
   - Red LED
   - Relay activation
   - Alert message on LCD
4. Normal conditions display live sensor values and green LED.

==============================
5. Software Setup
==============================
1. Install Arduino IDE on your computer.
2. Connect Arduino UNO via USB.
3. Include required libraries:
   - Wire.h
   - LiquidCrystal_I2C.h
   - DHT.h
4. Upload the provided Arduino code to the board.

==============================
6. Arduino Code Overview
==============================
- Initializes all sensors and output devices.
- Reads vibration, reed switch, temperature, and humidity.
- Implements safety thresholds.
- Updates RGB LED, relay, and LCD accordingly.
- Reads sensors every 2 seconds.

==============================
7. Results
==============================
- RGB LED glows green under normal conditions.
- Alerts triggered immediately on vibration or tilt.
- Red LED and relay activate on unauthorized access.
- High temperature (>55°C) or high humidity (>100%) triggers warnings.
- Real-time sensor values displayed on LCD.

