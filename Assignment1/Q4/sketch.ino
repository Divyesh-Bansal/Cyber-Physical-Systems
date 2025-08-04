#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define PIR_PIN 3
#define MQ2_PIN A0
#define BUZZER_PIN 4
#define FAN_LED_PIN 5

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_LED_PIN, OUTPUT);
}

void loop() {
  float temp = dht.readTemperature();
  int gasLevel = analogRead(MQ2_PIN);
  bool motionDetected = digitalRead(PIR_PIN);

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" C | Gas: "); Serial.print(gasLevel);
  Serial.print(" | Motion: "); Serial.println(motionDetected);

  if (gasLevel > 400 || temp > 35 || motionDetected) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(FAN_LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(FAN_LED_PIN, LOW);
  }

  delay(1000);
}
