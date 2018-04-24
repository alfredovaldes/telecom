#define RLOAD 22.0
// Calibration resistance at atmospheric CO2 level
#define RZERO 630.13 
#include "MQ135.h" 
MQ135 gasSensor = MQ135(A0); 
int val; 
int sensorPin = A0; 
int sensorValue = 0; 
void setup() { 
  Serial.begin(9600);
  pinMode(sensorPin, INPUT); 
} 
 
void loop() { 
  val = analogRead(A0); 
  Serial.print ("raw = "); 
  Serial.println (val); 
  float zero = gasSensor.getRZero(); 
  Serial.print ("rzero: "); 
  Serial.println (zero); 
  float ppm = gasSensor.getPPM(); 
  Serial.print ("ppm: "); 
  Serial.println (ppm); 
  delay(5000); 
} 
