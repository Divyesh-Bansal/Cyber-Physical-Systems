import machine
import utime
import dht
from machine import Pin, ADC, I2C
import ssd1306


i2c = I2C(0, scl=Pin(1), sda=Pin(0))
oled = ssd1306.SSD1306_I2C(128, 64, i2c)


dht_sensor = dht.DHT22(Pin(15))

pot = ADC(26)

log_data = []


def read_wind_speed():
    raw = pot.read_u16() 
    wind_speed = int((raw / 65535) * 100)  
    return wind_speed

while True:
    try:
       
        dht_sensor.measure()
        temperature = dht_sensor.temperature()
        humidity = dht_sensor.humidity()
    
        wind_speed = read_wind_speed()

        oled.fill(0) 
        oled.text("Live Weather", 0, 0)
        oled.text("Temp: {} C".format(temperature), 0, 16)
        oled.text("Hum : {} %".format(humidity), 0, 32)
        oled.text("Wind: {} %".format(wind_speed), 0, 48)
        oled.show()


        log_data.append((temperature, humidity, wind_speed))
        if len(log_data) > 5:
            log_data.pop(0)
            
        print("Temp: {} C, Hum: {} %, Wind: {} %".format(
            temperature, humidity, wind_speed
        ))

    except Exception as e:
        print("Error reading sensor:", e)

    utime.sleep(2)
