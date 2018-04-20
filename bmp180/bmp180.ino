#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  Wire.pins(0, 2);
  Wire.begin(0, 2);
  if (!bmp.begin()) {
    Serial.println("No BMP180 / BMP085");// we dont wait for this
    while (1) {}
  }
}
float temp[10];
float pres[10];
float alti[10];
float avgTemp;
float avgPres;
float avgAlti;
float average (float * array, int len)  // assuming array is int.
{
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < len ; i++)
    sum += array [i] ;
  return  ((float) sum) / len ;  // average will be fractional, so float may be appropriate.
}
void loop() {
  for (int i = 0; i < 10; i++) {
    temp[i] = bmp.readTemperature();
    pres[i] = bmp.readPressure();
    alti[i] = bmp.readAltitude(102300);
  }
  avgTemp = average(temp, 10);
  avgPres = average(pres, 10);
  avgAlti = average(alti, 10);
  String t = "T=" + String(avgTemp) + " *C";
  String  p = "P=" + String(avgPres) + " Pa";
  String a = "A=" + String(avgAlti) + " m";// insert pressure at sea level

  Serial.println(t);
  Serial.println(p);
  Serial.println(a);
  delay(2000);
}
