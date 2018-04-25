//Lux
double Light (int RawADC0){
double Vout=RawADC0*0.0048828125;
int lux=(2400/Vout-330)/9.97;
return lux;
}
void setup() {
Serial.begin(115200);
}
void loop() {
Serial.println(int(Light(analogRead(A0))));
delay(1000);
}
