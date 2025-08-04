
#include <Servo.h>

// ---------------- Pin Definitions ----------------
#define PIN_IR      2
#define PIN_TRIG    5
#define PIN_ECHO    6
#define PIN_SERVO   9
#define PIN_LED     3
#define PIN_LDR     A0

// ---------------- Constants ----------------
const int GATE_CLOSED_ANGLE = 0;
const int GATE_OPEN_ANGLE   = 90;
const int OPEN_CLOSE_SPEED  = 5;     // deg per step
const int STEP_DELAY_MS     = 10;    // smoothness
const long DIST_OPEN_CM     = 20;
const long DIST_CLEAR_CM    = 35;
const unsigned long OPEN_HOLD_MS = 1000;

// ---------------- Enum and Global Variables ----------------
enum GateState { CLOSED, OPEN, MOVING };
GateState state = CLOSED;

Servo gate;
unsigned long stateChangeMs = 0;
int ledStatus = 0; // 0 = OFF, 1 = ON

// ---------------- Function Prototypes ----------------
void switchState(GateState s);
long readDistanceCm();
void moveServoSmooth(int fromDeg, int toDeg);
void setLedByLdr(bool parkingFree);

// ---------------- Setup ----------------
void setup() {
  pinMode(PIN_IR,   INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED,  OUTPUT);
  pinMode(PIN_LDR,  INPUT);

  gate.attach(PIN_SERVO);
  gate.write(GATE_CLOSED_ANGLE);

  Serial.begin(9600);
  Serial.println("Smart Parking Gate");
}

// ---------------- Main Loop ----------------
void loop() {
  bool irDetected = (digitalRead(PIN_IR) == HIGH); // Change to LOW if inverted
  long dist = readDistanceCm();

  bool occupied = (dist < DIST_CLEAR_CM);
  setLedByLdr(!occupied); // sets LED according to parking status

  switch (state) {
    case CLOSED:
      if (irDetected && dist <= DIST_OPEN_CM) {
        moveServoSmooth(GATE_CLOSED_ANGLE, GATE_OPEN_ANGLE);
        switchState(OPEN);
      }
      break;

    case OPEN:
      if (millis() - stateChangeMs >= OPEN_HOLD_MS) {
        if (dist > DIST_CLEAR_CM) {
          moveServoSmooth(GATE_OPEN_ANGLE, GATE_CLOSED_ANGLE);
          switchState(CLOSED);
        }
      }
      break;

    default:
      break;
  }

  // Debug prints
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print("IR: "); Serial.print(irDetected);
    Serial.print("  Dist(cm): "); Serial.print(dist);
    Serial.print("  State: "); Serial.print(state == CLOSED ? "CLOSED" : (state == OPEN ? "OPEN" : "MOVING"));
    Serial.print("  LDR: "); Serial.print(analogRead(PIN_LDR));

    // Added LED status print
    Serial.print("  LED: ");
    Serial.println(ledStatus ? "ON" : "OFF");
  }

  delay(10);
}

// ---------------- Helper Functions ----------------
void switchState(GateState s) {
  state = s;
  stateChangeMs = millis();
}

long readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 30000UL);
  if (duration == 0) return 500;
  return duration / 58;
}

void moveServoSmooth(int fromDeg, int toDeg) {
  int step = (toDeg > fromDeg) ? OPEN_CLOSE_SPEED : -OPEN_CLOSE_SPEED;
  for (int p = fromDeg; (step > 0) ? p <= toDeg : p >= toDeg; p += step) {
    gate.write(p);
    delay(STEP_DELAY_MS);
  }
  gate.write(toDeg);
}

void setLedByLdr(bool parkingFree) {
  if (!parkingFree) {
    analogWrite(PIN_LED, 0);
    ledStatus = 0; // LED OFF
    return;
  }
  int ldrRaw = analogRead(PIN_LDR); // higher in bright light
  int brightness = map(ldrRaw, 0, 1023, 50, 255);
  brightness = constrain(brightness, 0, 255);
  analogWrite(PIN_LED, brightness);
  ledStatus = 1; // LED ON
}
/*
Stage 1 — Idle State
No car near sensor
Gate is closed
LED ON (parking space available)

Stage 2 — Car Approaches
Reduce the distance
Ensure IR sensor detects (toggle its state in simulation by moving object in front).
Gate opens .
LED OFF (parking occupied).

Stage 3 — Car Passes / Clears
Increase ultrasonic distance > 35 cm.
Gate closes automatically after a short delay.
LED ON again (space free).

Stage 4 — Light Sensitivity
Use light slider on the LDR in Tinkercad.
In bright light → LED shines brighter.
In darkness → LED dims softly.
*/