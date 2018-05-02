#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}
#include <ArduinoJson.h>

//***ESTRUCTURA DE LOS DATOS TRANSMITIDOS MAESTRO/ESCLAVO***//
//Se de establecer IGUAL en el par maestro
struct ESTRUCTURA_DATOS {
  char payload[1000];
};
void setup() {

  //***INICIALIZACIÓN DEL PUERTO SERIE***//
  Serial.begin(115200); Serial.println();

  //***INICIALIZACIÓN DEL PROTOCOLO ESP-NOW***//
  if (esp_now_init() != 0) {
    Serial.println("Protocolo ESP-NOW no inicializado...");
    ESP.restart();
    delay(1);
  }
  else {
    Serial.println("ESPNOW OK");
  }

  //***DATOS DE LAS MAC (Access Point y Station) del ESP***//
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  //***DECLARACIÓN DEL PAPEL DEL DISPOSITIVO ESP EN LA COMUNICACIÓN***//
  //0=OCIOSO, 1=MAESTRO, 2=ESCLAVO y 3=MAESTRO+ESCLAVO
  esp_now_set_self_role(2);
}
StaticJsonBuffer<300> JSONbuffer;   //Declaramos objeto JSON
JsonObject& JSONencoder = JSONbuffer.createObject();
JsonObject& values = JSONencoder.createNestedObject("Jardin1");
char JSONmessageBuffer[2000];
int temp, hum, lux, lluvia, gndHum = 0;
float hin, presBar, altitud, gasCo = 0.0;
void loop() {
  //***RECEPCIÓN DE LA COMUNICACIÓN ESP-NOW***//
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {

    char MACmaestro[6];
    sprintf(MACmaestro, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    //Serial.print("Recepcion desde ESP MAC: "); Serial.print(MACmaestro);
    ESTRUCTURA_DATOS ED;
    memcpy(&ED, data, sizeof(ED));
    //Serial.print(". Datos recibidos: "); Serial.print(ED.payload);
    //Serial.println(".");
    if (strcmp(MACmaestro, "5E:CF:7F:81:31:0A") == 0) {
      temp = atoi(&ED.payload[28]);
      hum = atoi(&ED.payload[47]);
      hin = atof(&ED.payload[66]);
      /*Serial.println("ESP1");
        Serial.print("Temperatura: ");
        Serial.println(temp);
        Serial.print("Humedad: ");
        Serial.println(hum);
        Serial.print("HeatIndex: ");
        Serial.println(hin);*/
    }
    if (strcmp(MACmaestro, "1A:FE:34:D6:4B:BF") == 0) {
      lux = atoi(&ED.payload[27]);
      presBar = atof(&ED.payload[46]);
      altitud = atof(&ED.payload[72]);
      /*Serial.println("ESP2");
        Serial.print("lux: ");
        Serial.println(lux);
        Serial.print("presBar: ");
        Serial.println(presBar);
        Serial.print("altitud: ");
        Serial.println(altitud);*/
    }
    if (strcmp(MACmaestro, "1A:FE:34:D1:D0:BB") == 0) {
      gasCo = atof(&ED.payload[30]);
      /*Serial.println("ESP3");
        Serial.print("CO2 PPM: ");
        Serial.println(gasCo);*/
    }
    if (strcmp(MACmaestro, "1A:FE:34:D7:38:0A") == 0) {
      lluvia = atoi(&ED.payload[30]);
      gndHum = atoi(&ED.payload[48]);
      /*Serial.println("ESP4");
        Serial.print("Lluvia: ");
        Serial.println(lluvia);
        Serial.print("gndHum: ");
        Serial.print(gndHum);
        Serial.println("%");*/
    }
  });
  values["temp"] = temp;
  values["hum"] = hum;
  values["hin"] = hin;
  values["lux"] = lux;
  values["presBar"] = presBar;
  values["altitud"] = altitud;
  values["gasCO2"] = gasCo;
  values["lluvia"] = lluvia;
  values["gndHum"] = gndHum;
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
}
