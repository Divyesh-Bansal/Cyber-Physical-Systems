const int soilMoisturePin = A0;
const int ldrPin = A1;
const int tempPin = A2;

const int relayPin = 3;       
const int growLightPin = 4;     

const int soilDryThreshold = 600;      
const int lightThreshold = 500;        

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  pinMode(growLightPin, OUTPUT);
  
  digitalWrite(relayPin, LOW);     
  digitalWrite(growLightPin, LOW); 
}

void loop() {

  int soilMoisture = analogRead(soilMoisturePin);
 
  int lightLevel = analogRead(ldrPin);


  int rawTemp = analogRead(tempPin);
  float voltage = rawTemp * (5.0 / 1023.0);       
  float temperatureC = (voltage - 0.5) * 100.0;   


  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print(" | Light Level: ");
  Serial.print(lightLevel);
  Serial.print(" | Temp: ");
  Serial.print(temperatureC);
  Serial.println(" C");


  if (soilMoisture > soilDryThreshold) {
    digitalWrite(relayPin, HIGH); 
  } else {
    digitalWrite(relayPin, LOW);   
  }


  if (lightLevel < lightThreshold) {
    digitalWrite(growLightPin, HIGH); 
  } else {
    digitalWrite(growLightPin, LOW); 
  }

  delay(2000); 
}
