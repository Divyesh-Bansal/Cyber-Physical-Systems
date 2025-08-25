/* Machinery Safety Monitor (Arduino UNO)
   - SW-200D vibration/tilt switch -> D2 (INPUT_PULLUP)
   - Reed (magnetic) switch        -> D3 (INPUT_PULLUP)
   - DHT11 temperature/humidity    -> D4
   - RGB LED (common-cathode)      -> D9 (R), D10 (G), D11 (B) via 220Ω resistors
   - Relay module IN               -> D7  (VCC->5V, GND->GND)
   - I2C LCD (PCF8574 backpack)    -> SDA=A4, SCL=A5 ; set address below
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>


#define VIB_PIN 2
#define REED_PIN 3
#define DHT_PIN 4
#define DHT_TYPE DHT11

#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

#define RELAY_PIN 7
const bool RELAY_ACTIVE_LOW = false;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

DHT dht(DHT_PIN, DHT_TYPE);
const float TEMP_THRESHOLD = 55.0;   
const float HUM_THRESHOLD  = 100.0; 

unsigned long lastDhtRead = 0;
const unsigned long DHT_INTERVAL = 2000; 

unsigned long lastVibTime = 0;
const unsigned long VIB_DEBOUNCE_MS = 2000;

float lastTemperature = NAN;
float lastHumidity = NAN;

void setup() {
  Serial.begin(9600);
  while (!Serial) 

  pinMode(VIB_PIN, INPUT_PULLUP);
  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);

  lcd.init();
  lcd.backlight();

  dht.begin();

  setRGB(0, 255, 0);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Machinery Safety");
  lcd.setCursor(0,1);
  lcd.print("Status: INIT...");
  delay(1000);
  lcd.clear();
}

void loop() {

  bool vibTriggered = (digitalRead(VIB_PIN) == LOW); 
  bool reedTriggered = (digitalRead(REED_PIN) == LOW);

  if (vibTriggered && (millis() - lastVibTime > VIB_DEBOUNCE_MS)) {
    Serial.println("Vibration/Tilt detected (debounced)");
    lastVibTime = millis();
  }


  if (millis() - lastDhtRead > DHT_INTERVAL) {
    lastDhtRead = millis();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); 
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("DHT read failed");
    } else {
      lastTemperature = temperature;
      lastHumidity = humidity;
      Serial.print("Temp: "); Serial.print(lastTemperature,1);
      Serial.print(" C  Hum: "); Serial.print(lastHumidity,0); Serial.println(" %");
    }
  }

  bool unsafe = false;
  String reason = "";

  if (vibTriggered) {
    unsafe = true; reason = "VIBRATION/TILT!";
  } else if (reedTriggered) {
    unsafe = true; reason = "ACCESS BREACH!";
  } else if (!isnan(lastTemperature) && lastTemperature >= TEMP_THRESHOLD) {
    unsafe = true; reason = "HIGH TEMP!";
  } else if (!isnan(lastHumidity) && lastHumidity >= HUM_THRESHOLD) {
    unsafe = true; reason = "HIGH HUMIDITY!";
  }

  if (unsafe) {
    setRGB(255,0,0);
    setRelay(true);
    showLCDAlert(reason);
    Serial.print("ALERT: "); Serial.println(reason);
  } else {
    setRGB(0,255,0);
    setRelay(false);
    showLCDNormal(lastTemperature, lastHumidity);
  }

  delay(200); 
}

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void setRelay(bool on) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  }
}

void showLCDAlert(String reason) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!!! DANGER !!!");
  lcd.setCursor(0,1);
  if (reason.length() > 16) reason = reason.substring(0,16);
  lcd.print(reason);
}

void showLCDNormal(float temp, float hum) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Status: SAFE   ");
  lcd.setCursor(0,1);

  if (!isnan(temp)) {
    lcd.print("T:"); lcd.print(temp,1); lcd.print("C ");
  } else {
    lcd.print("T:--C ");
  }

  if (!isnan(hum)) {
    lcd.print("H:"); lcd.print(hum,0); lcd.print("%");
  } else {
    lcd.print("H:--%");
  }
}
