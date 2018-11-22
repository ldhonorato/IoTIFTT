/************************* Inclusão das Bibliotecas *********************************/
#include "ESP8266WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
 
/************************* Conexão WiFi*********************************/

#define WIFI_SSID       "SSID"
#define WIFI_PASS       "********"

/********************* Credenciais Adafruit io *************************/
 
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "username" // Seu usuario cadastrado na plataforma da Adafruit
#define AIO_KEY         "key1235643215643264864"       // Sua key da dashboard
 
/********************** Variaveis globais *******************************/
 
WiFiClient client;
 
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

#define PINO_LED  D2
 
long previousMillis = 0;
 
/****************************** Declaração dos Feeds ***************************************/
 
/* feed responsavel por receber os dados da nossa dashboard */
Adafruit_MQTT_Subscribe _led = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledAzul", MQTT_QOS_1);
 
 /*************************** Declaração dos Prototypes ************************************/
 
void initSerial();
void initPins();
void initWiFi();
void initMQTT();
void conectar_broker();
 
/*************************** Sketch ************************************/
 
void setup() {
  initSerial();
  initPins();
  initWiFi();
  initMQTT();
}
 
void loop() {
  conectar_broker();
  mqtt.processPackets(5000);
}
 
/*************************** Implementação dos Prototypes ************************************/
 
/* Conexao Serial */
void initSerial() {
  Serial.begin(115200);
  delay(10);
}
 
/* Configuração dos pinos */
void initPins() {
  pinMode(PINO_LED, OUTPUT);
  analogWrite(PINO_LED, 0);
}
 
/* Configuração da conexão WiFi */
void initWiFi() {
  Serial.print("Conectando-se na rede "); Serial.println(WIFI_SSID);
 
  WiFi.begin(WIFI_SSID, WIFI_PASS);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
 
  Serial.println("Conectado à rede com sucesso"); Serial.println("Endereço IP: "); Serial.println(WiFi.localIP());
}
 
/* Configuração da conexão MQTT */
void initMQTT() {
  _led.setCallback(led_callback);
  mqtt.subscribe(&_led);
}
 
 
/*************************** Implementação dos Callbacks ************************************/
 
/* callback responsavel por tratar o feed do rele */
void led_callback(char *data, uint16_t len) {
  String state = data;
 
  int pwmLED = state.toInt();

  analogWrite(PINO_LED, pwmLED);
  Serial.print("Led: "); Serial.println(state);
 
}
 
/*************************** Demais implementações ************************************/
 
/* Conexão com o broker e também servirá para reestabelecer a conexão caso caia */
void conectar_broker() {
  int8_t ret;
 
  if (mqtt.connected()) {
    return;
  }
 
  Serial.println("Conectando-se ao broker mqtt...");
 
  uint8_t num_tentativas = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Falha ao se conectar. Tentando se reconectar em 5 segundos.");
    mqtt.disconnect();
    delay(5000);
    num_tentativas--;
    if (num_tentativas == 0) {
      Serial.println("Seu ESP será resetado.");
      while (1);
    }
  }
 
  Serial.println("Conectado ao broker com sucesso.");
}
