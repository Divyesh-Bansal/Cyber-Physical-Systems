Smart Room Monitoring and Automation System
Divyesh Bansal (23112036)


==============================
1. Project Overview
==============================
This project monitors and automates a room’s environment using a Raspberry Pi 3. It tracks:
- Temperature and Humidity (DHT11)
- Light Levels (LDR)
- Door/Window Status (Magnetic Reed Switch)

The system controls appliances such as fans and lights automatically based on sensor readings. An RGB LED indicates the status:
- Fan ON = Green
- Dark = Blue
- Door Open = Red

An LCD display shows real-time environmental data.

==============================
2. Hardware Components
==============================
- Raspberry Pi 3
- Sensors:
  - DHT11 (Temperature and Humidity)
  - LDR (Light Detection)
  - Magnetic Reed Switch (Door/Window)
- Actuators:
  - RGB LED (Status Indicator)
  - Relay Module (Controls Fan/Light)
  - LCD Display (I2C Interface)

==============================
3. Software Setup
==============================
1. Connect to Raspberry Pi via SSH:
   - Find Pi IP using `arp -a`
   - Connect using PowerShell or terminal:
     ssh pi@<Pi_IP>
2. Install necessary libraries:
   sudo apt update
   sudo apt install python3-pip
   pip3 install RPi.GPIO adafruit-circuitpython-dht RPLCD

==============================
4. Hardware Connections
==============================
- DHT11 DATA → GPIO4
- LDR → GPIO18
- Reed Switch → GPIO17
- Relay IN → GPIO27
- RGB LED:
  - RED → GPIO23
  - GREEN → GPIO24
  - BLUE → GPIO25
- LCD (I2C) → SDA, SCL pins
- Power all sensors with 3.3V/5V and connect GND accordingly

==============================
5. Working Logic
==============================
- Temperature Monitoring:
  - Fan turns ON if temperature > 35°C
  - RGB LED turns Green
- Light Detection:
  - RGB LED turns Blue if dark
- Door Monitoring:
  - RGB LED turns Red if the door is open
- LCD Display:
  - Continuously shows temperature, humidity, light, and door status

==============================
6. Running the Code
==============================
1. Save the Python code to a file, e.g., `smart.py`
2. Run the script:
   python3 smart.py
3. Press Ctrl+C to stop the program safely. LCD will clear, and GPIO pins will reset.



