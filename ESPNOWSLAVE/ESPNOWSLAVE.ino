#include <ESP8266WiFi.h> 
extern "C" {
  #include <espnow.h>
}

//***ESTRUCTURA DE LOS DATOS TRANSMITIDOS MAESTRO/ESCLAVO***//
//Se de establecer IGUAL en el par maestro
    struct ESTRUCTURA_DATOS {
    uint16_t potenciometro = 0;
    uint32_t tiempo = 0;
};

//***PIN de conexión del LED a regular con el potenciometro del ESP MAESTRO***//
int PinLED = 5;    //Pin D1

void setup() {

  //***INICIALIZACIÓN DEL PUERTO SERIE***//
  Serial.begin(115200); Serial.println();

  //***INICIALIZACIÓN DEL PROTOCOLO ESP-NOW***//   
  if (esp_now_init()!=0) {
    Serial.println("Protocolo ESP-NOW no inicializado...");
    ESP.restart();
    delay(1);
  }
  else{
    Serial.println("ESPNOW OK");
  }

  //***DATOS DE LAS MAC (Access Point y Station) del ESP***//
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  //***DECLARACIÓN DEL PAPEL DEL DISPOSITIVO ESP EN LA COMUNICACIÓN***//
  //0=OCIOSO, 1=MAESTRO, 2=ESCLAVO y 3=MAESTRO+ESCLAVO 
  esp_now_set_self_role(2);   

  //***DECLARACIÓN del PinLED como SALIDA***//
  pinMode(PinLED, OUTPUT);
}

void loop() {

  //***RECEPCIÓN DE LA COMUNICACIÓN ESP-NOW***//
   esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {

    char MACmaestro[6];
    sprintf(MACmaestro, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    Serial.print("Recepcion desde ESP MAC: "); Serial.print(MACmaestro);

    ESTRUCTURA_DATOS ED;
    memcpy(&ED, data, sizeof(ED));

    Serial.print(". Dato potenciometro: "); Serial.print(ED.potenciometro);
    Serial.print(". Dato tiempo: "); Serial.print(ED.tiempo);
    Serial.println(".");

  analogWrite(PinLED,ED.potenciometro);
  });

}
