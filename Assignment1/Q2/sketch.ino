#include <DHT.h>

#define DHTPIN    2
#define DHTTYPE   DHT22
#define PIR_PIN   7
#define LDR_PIN   A0
#define BUZZER_PIN 8
#define LED_PIN    9

// --- Adjustable Parameters ---
int  ldrDarkThreshold = 500;     
int  alarmToneHz      = 1000;    
int  alarmHoldMs      = 2000;    
unsigned long envPrintIntervalMs = 2000; 

// --- Globals ---
DHT dht(DHTPIN, DHTTYPE);
bool alarmActive = false;
unsigned long alarmStartedAt = 0;
unsigned long lastPrintAt = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println(F("======================================"));
  Serial.println(F(" HOME INTRUSION + ENVIRONMENT MONITOR "));
  Serial.println(F("======================================"));
  Serial.println(F("Sensors: PIR (D7), LDR (A0), DHT22 (D2)"));
  Serial.println(F("Actuators: LED (D9), Buzzer (D8)"));
  Serial.println(F("--------------------------------------"));
  delay(2000); // small delay for PIR stabilization
}

void loop() {
  unsigned long now = millis();

  // --- Read Sensors ---
  int ldrValue = analogRead(LDR_PIN);
  bool isDark = (ldrValue < ldrDarkThreshold);
  bool motionDetected = digitalRead(PIR_PIN);

  // --- Print Environment Data Periodically ---
  if (now - lastPrintAt >= envPrintIntervalMs) {
    lastPrintAt = now;
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    Serial.println(F("\n--------------------------------------"));
    Serial.println(F("Environment Data"));
    Serial.print(F("  LDR Value     : ")); Serial.println(ldrValue);
    Serial.print(F("  Light Status  : ")); Serial.println(isDark ? "Dark" : "Bright");
    Serial.print(F("  PIR Motion    : ")); Serial.println(motionDetected ? "Detected" : "None");

    if (!isnan(temp) && !isnan(hum)) {
      Serial.print(F("  Temperature   : ")); Serial.print(temp, 1); Serial.println(F(" °C"));
      Serial.print(F("  Humidity      : ")); Serial.print(hum, 1); Serial.println(F(" %"));
    } else {
      Serial.println(F("  DHT22 Error   : Failed to read data"));
    }
  }

  // --- Intrusion Logic (only at night) ---
  if (motionDetected && isDark) {
    if (!alarmActive) {
      alarmActive = true;
      alarmStartedAt = now;
      tone(BUZZER_PIN, alarmToneHz);
      digitalWrite(LED_PIN, HIGH);
      Serial.println(F("\nALERT: Motion detected in dark! Alarm ON"));
    } else {
      alarmStartedAt = now; 
    }
  }

  // --- Stop Alarm After Timeout ---
  if (alarmActive && (now - alarmStartedAt >= (unsigned long)alarmHoldMs)) {
    alarmActive = false;
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    Serial.println(F("Alarm cleared. System back to monitoring."));
  }

  delay(50); // small delay to stabilize loop
}
