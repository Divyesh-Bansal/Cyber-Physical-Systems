from machine import I2C, Pin, PWM
from time import sleep
import utime
from pico_i2c_lcd import I2cLcd

# Servo setup
servo_motor = PWM(Pin(2))
servo_motor.freq(50)

# Buzzer setup
alarm_buzzer = Pin(3, Pin.OUT) 

# LCD setup
i2c_bus = I2C(0, sda=Pin(0), scl=Pin(1), freq=400000)
lcd_address = i2c_bus.scan()[0]
lcd_screen = I2cLcd(i2c_bus, lcd_address, 2, 16)

# PIR sensor
pir_sensor = Pin(4, Pin.IN, Pin.PULL_DOWN)

# Passcode storage
stored_passcode = ''

# Keypad pins
row_pins = [28, 27, 26, 22]  
col_pins = [21, 20, 19, 18]  

# Key mapping
key_layout = [
    ['1', '2', '3', 'A'],
    ['4', '5', '6', 'B'],
    ['7', '8', '9', 'C'],
    ['*', '0', '#', 'D']
]

# Setup row pins
for r in row_pins:
    Pin(r, Pin.IN, Pin.PULL_UP)

# Setup column pins
for c in col_pins:
    Pin(c, Pin.OUT)

def activate_buzzer(duration=0.5):
    alarm_buzzer.value(1)
    sleep(duration)
    alarm_buzzer.value(0)

def scan_keypad():
    for col_index, c_pin in enumerate(col_pins):
        Pin(c_pin, Pin.OUT, value=0)
        for row_index, r_pin in enumerate(row_pins):
            if not Pin(r_pin).value():
                while not Pin(r_pin).value():
                    pass
                return key_layout[row_index][col_index]
        Pin(c_pin, Pin.IN)
    return None

def move_servo(start_angle, end_angle):
    start_duty = ((start_angle * 65) / 1.8) + 1500
    end_duty = ((end_angle * 65) / 1.8) + 1500
    servo_motor.duty_u16(int(start_duty))
    utime.sleep(1)
    servo_motor.duty_u16(int(end_duty))
    utime.sleep(1)

def lock_servo():
    move_servo(0, 180)

def unlock_servo():
    move_servo(180, 0)

def lcd_line_one(message):
    lcd_screen.clear()
    lcd_screen.move_to(0, 0)
    lcd_screen.putstr(message)

def lcd_two_lines(msg1, msg2, pos1=0, pos2=0):
    lcd_screen.clear()
    lcd_screen.move_to(pos1, 0)
    lcd_screen.putstr(msg1)
    lcd_screen.move_to(pos2, 1)
    lcd_screen.putstr(msg2)

def lcd_scroll_text(text, row=0):
    for i in range(len(text)):
        lcd_screen.move_to(i, row)
        lcd_screen.putstr(text[i])
        sleep(0.1)

def boot_message():
    lcd_screen.clear()
    lcd_scroll_text('   WELCOME !!')
    sleep(1)
    lcd_screen.clear()
    lcd_scroll_text('ELECTRONIC DOOR')
    lcd_scroll_text('    LOCK !!', 1)
    sleep(1)
    lcd_screen.clear()
    lcd_scroll_text('INITIALIZING...')
    lcd_scroll_text('SETUP...........', 1)
    lcd_screen.clear()
    lcd_scroll_text('SETUP..')
    lcd_scroll_text('SUCCESSFUL....', 1)

def home_screen():
    lcd_two_lines('# to unlock door', ' A to New code')

def status_progress(message, delay=0.1):
    lcd_two_lines(message, "[............] ")
    for i in range(2, 14):
        lcd_screen.move_to(i, 1)
        lcd_screen.putstr("=")
        sleep(delay)

def lock_prompt():
    lcd_line_one(' # to lock door')

def passcode_prompt():
    lcd_two_lines('Enter Your code: ', '[____]', 0, 5)

def access_denied():
    for _ in range(3):
        activate_buzzer(0.2)
        sleep(0.1)
    lcd_two_lines('code mismatch !!', 'Access denied !!')
    sleep(1)
    status_progress('Access Denied !!', 0.8)
    lcd_line_one(' Door locked!!')
    sleep(1)

def motion_alert():
    lcd_two_lines('Motion detected!!', '  Check Area !!')
    activate_buzzer(0.2)
    sleep(0.5)
    timeout_limit = utime.ticks_add(utime.ticks_ms(), 5000)
    while pir_sensor.value():
        if utime.ticks_diff(timeout_limit, utime.ticks_ms()) <= 0:
            break
        sleep(0.1)
    home_screen()

def get_passcode_input():
    entered_code = ''
    while len(entered_code) != 4:
        key = scan_keypad()
        if key:
            entered_code += key
            lcd_screen.move_to(5 + (len(entered_code)), 1)
            lcd_screen.putchar(entered_code[-1])
            sleep(0.1)
            lcd_screen.move_to(5 + (len(entered_code)), 1)
            lcd_screen.putchar('*')
    return entered_code

def set_new_passcode():
    global stored_passcode
    lcd_two_lines("Enter New code:", '[____]', 0, 5)
    stored_passcode = get_passcode_input()
    lcd_two_lines('Confirm New Code', '[____]', 0, 5)
    sleep(0.5)
    is_same = stored_passcode == get_passcode_input()
    status_progress('Authenticating !', 0.07)
    return is_same

def initial_lock():
    lcd_line_one(' code matched !!')
    sleep(0.5)
    status_progress('    LOCKING !!')
    lock_servo()
    lcd_two_lines('      DOOR', "     LOCKED !!")
    sleep(1)

def lock_door_wait():
    lock_prompt()
    while True:
        key = scan_keypad()
        if key == '#':
            lock_servo()
            lcd_two_lines('      DOOR', "     LOCKED !!")
            sleep(1)
            home_screen()
            return

def unlock_door():
    lcd_line_one('code matched !!')
    sleep(0.5)
    status_progress('  UNLOCKING !!')
    unlock_servo()
    lcd_two_lines('      DOOR', "   UNLOCKED !!")
    sleep(1)
    lock_door_wait()

def verify_passcode():
    code_input = get_passcode_input()
    status_progress('Authenticating !', 0.07)
    return code_input == stored_passcode

def setup_initial_code():
    lock_prompt()
    while True:
        key = scan_keypad()
        if key == '#':
            if set_new_passcode():
                initial_lock()
                return
            else:
                lcd_two_lines('code mismatch !!', 'door not locked!')
                sleep(1)
                lock_prompt()

def main_loop():
    home_screen()
    while True:
        key = scan_keypad()
        if key is None:
            if pir_sensor.value():
                motion_alert()
                continue
            else:
                sleep(0.05)
                continue
        if key == '#':
            passcode_prompt()
            if verify_passcode():
                unlock_door()
            else:
                access_denied()
                home_screen()
        elif key == 'A':
            if set_new_passcode():
                unlock_door()
            else:
                access_denied()
                home_screen()

if __name__ == "__main__":
    boot_message()
    setup_initial_code()
    main_loop()
