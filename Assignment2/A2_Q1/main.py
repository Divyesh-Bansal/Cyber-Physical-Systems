from machine import Pin, I2C
from time import sleep
import dht
import ssd1306


dht_sensor = dht.DHT22(Pin(15))
relay = Pin(16, Pin.OUT)
led = Pin(17, Pin.OUT)

i2c = I2C(0, scl=Pin(1), sda=Pin(0))
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

TEMP_THRESHOLD = 30  
HUMIDITY_THRESHOLD = 70 

while True:
    try:
        dht_sensor.measure()
        temp = dht_sensor.temperature()
        hum = dht_sensor.humidity()

        print("Temp:", temp, "Humidity:", hum)

        oled.fill(0)
        oled.text("Smart Env Ctrl", 0, 0)
        oled.text(f"Temp: {temp} C", 0, 20)
        oled.text(f"Humidity: {hum}%", 0, 35)

        if temp > TEMP_THRESHOLD or hum > HUMIDITY_THRESHOLD:
            relay.value(1) 
            led.value(1)
            oled.text("Fan/Heater: ON", 0, 50)
        else:
            relay.value(0)
            led.value(0)
            oled.text("Fan/Heater: OFF", 0, 50)

        oled.show()

    except Exception as e:
        print("Sensor error:", e)

    sleep(2)
