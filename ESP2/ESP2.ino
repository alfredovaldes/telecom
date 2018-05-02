#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

//***ESTRUCTURA DE LOS DATOS TRANSMITIDOS MAESTRO/ESCLAVO***//
//Se de establecer IGUAL en el par esclavo
struct ESTRUCTURA_DATOS {
  char payload[1000];
};

void setup() {

  //***INICIALIZACIÓN DEL PUERTO SERIE***//
  Serial.begin(115200);

  Wire.pins(D1, D2);
  Wire.begin(D1, D2);
  if (!bmp.begin()) {
    Serial.println("No BMP180 / BMP085");// we dont wait for this
    //while (1) {}
  }

  //***INICIALIZACIÓN DEL PROTOCOLO ESP-NOW***//
  if (esp_now_init() != 0) {
    Serial.println("*** ESP_Now init failed");
    ESP.restart();
    delay(1);
  }

  //***DATOS DE LAS MAC (Access Point y Station) del ESP***//
  Serial.print("Access Point MAC de este ESP: ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("Station MAC de este ESP: ");
  Serial.println(WiFi.macAddress());

  //***DECLARACIÓN DEL PAPEL DEL DISPOSITIVO ESP EN LA COMUNICACIÓN***//
  //0=OCIOSO, 1=MAESTRO, 2=ESCLAVO y 3=MAESTRO+ESCLAVO
  esp_now_set_self_role(1);

  //***EMPAREJAMIENTO CON EL ESCLAVO***//
  // Dirección MAC del ESP con el que se empareja (esclavo)
  // Se debe introducir la STA MAC correspondiente
  uint8_t mac_addr[6] = {0x5c, 0xcf, 0x7f, 0x86, 0x08, 0x54};
  uint8_t role = 2;
  uint8_t channel = 3;
  uint8_t key[0] = {}; //no hay clave
  //uint8_t key[16] = {0,255,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  uint8_t key_len = sizeof(key);
  Serial.print("Tamaño de *key:  ");
  Serial.println(key_len);
  esp_now_add_peer(mac_addr, role, channel, key, key_len);
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
StaticJsonBuffer<300> JSONbuffer;   //Declaramos objeto JSON
JsonObject& JSONencoder = JSONbuffer.createObject();
JsonObject& values = JSONencoder.createNestedObject("ESP2");
char JSONmessageBuffer[1000];

void loop() {
  sensorData();
  //JSON DE
  //Agregamos datos al JSON
  values["lux"] = Light(analogRead(A0));
  values["presBar"] = avgPres;
  values["altitud"] = avgAlti;
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  //***DATOS A ENVIAR***//
  ESTRUCTURA_DATOS ED;
  // ED.payload = JSONmessageBuffer;
  std::copy(std::begin(JSONmessageBuffer), std::end(JSONmessageBuffer), std::begin(ED.payload));
  //***ENVÍO DE LOS DATOS***//
  //uint8_t *da=NULL;   //NULL envía los datos a todos los ESP con los que está emparejado
  uint8_t da[6] = {0x5E, 0xCF, 0x7F, 0x86, 0x08, 0x54};
  uint8_t data[sizeof(ED)]; memcpy(data, &ED, sizeof(ED));
  uint8_t len = sizeof(data);
  esp_now_send(da, data, len);

  delay(1); //Si se pierden datos en la recepción se debe subir este valor

  //***VERIFICACIÓN DE LA RECEPCIÓN CORRECTA DE LOS DATOS POR EL ESCLAVO***//
  esp_now_register_send_cb([](uint8_t* mac, uint8_t status) {
    char MACesclavo[6];
    sprintf(MACesclavo, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(". Enviado a ESP MAC: ");
    Serial.print(MACesclavo);
    Serial.print(". Recepcion (0=0K - 1=ERROR): ");
    Serial.println(status);
  });
  delay(10000);
}

double Light (int RawADC0) {
  double Vout = RawADC0 * 0.0048828125;
  int lux = (2400 / Vout - 330) / 9.97;
  return lux;
}
void sensorData() {
  for (int i = 0; i < 10; i++) {
    temp[i] = bmp.readTemperature();
    pres[i] = bmp.readPressure();
    alti[i] = bmp.readAltitude(102300);
  }
  avgTemp = average(temp, 10);
  avgPres = average(pres, 10);
  avgAlti = average(alti, 10);
}

