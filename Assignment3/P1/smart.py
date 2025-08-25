import time
import RPi.GPIO as GPIO
import board
import adafruit_dht
from RPLCD.i2c import CharLCD

DHT_PIN = board.D4          # DHT11 DATA → GPIO4
LDR_PIN = 18                # LDR → GPIO18
REED_PIN = 17               # Reed switch → GPIO17
RELAY_PIN = 27              # Relay IN → GPIO27
RED_PIN = 23                # RGB LED RED → GPIO23
GREEN_PIN = 24              # RGB LED GREEN → GPIO24
BLUE_PIN = 25               # RGB LED BLUE → GPIO25

GPIO.setmode(GPIO.BCM)
GPIO.setup(LDR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(REED_PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(RELAY_PIN, GPIO.OUT)
GPIO.setup(RED_PIN, GPIO.OUT)
GPIO.setup(GREEN_PIN, GPIO.OUT)
GPIO.setup(BLUE_PIN, GPIO.OUT)

lcd = CharLCD('PCF8574', 0x27) 
lcd.clear()

dhtDevice = adafruit_dht.DHT11(DHT_PIN)


temperature_prev = None
humidity_prev = None

def set_rgb(r, g, b):
    GPIO.output(RED_PIN, r)
    GPIO.output(GREEN_PIN, g)
    GPIO.output(BLUE_PIN, b)


try:
    while True:

        try:
            temperature = dhtDevice.temperature
            humidity = dhtDevice.humidity

            if temperature is not None:
                temperature_prev = temperature
            if humidity is not None:
                humidity_prev = humidity
        except RuntimeError:

            temperature = temperature_prev
            humidity = humidity_prev

        light = GPIO.input(LDR_PIN)       # 1 = bright, 0 = dark
        door = GPIO.input(REED_PIN)       # 1 = closed, 0 = open
  
   
        lcd.clear()
        if temperature is not None and humidity is not None:
            lcd.write_string(f"T:{temperature:.1f}C H:{humidity:.1f}%")
        else:
            lcd.write_string("DHT11 Reading...")

        lcd.cursor_pos = (1, 0)
        lcd.write_string(f"L:{'BRIGHT' if light else 'DARK'} D:{'CLOSE' if door else 'OPEN'}")

        if temperature is not None:
            if temperature > 35:
                GPIO.output(RELAY_PIN, GPIO.HIGH)   # Fan ON
                set_rgb(0, 1, 0)  
            else:
                GPIO.output(RELAY_PIN, GPIO.LOW)    # Fan OFF
                set_rgb(0, 0, 0)  

        if light == 0:  # Dark
            set_rgb(0, 0, 1)  # BLUE = dark

        if door == 0:   # Door open
            set_rgb(1, 0, 0)  

        time.sleep(2)  

except KeyboardInterrupt:
    print("Exiting program...")

finally:
    lcd.clear()
    GPIO.cleanup()