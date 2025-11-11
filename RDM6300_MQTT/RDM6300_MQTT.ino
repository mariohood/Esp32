//RDM6300 com MQTT
// RDM6300 RDM630 RFID Reader interfacing with ESP32
// https://microcontrollerslab.com/rdm6300-rdm630-rfid-reader-esp32-tutorial/
// para informar minha extensão Tactiq: https://tactiq.io/r/transcribe 

#include <WiFi.h>
#include <PubSubClient.h>
//#include <SoftwareSerial.h>

// =======================
// 🔧 Configurações Wi-Fi
// =======================
const char* ssid     = "moto g(8) power 5139";
const char* password = "12345678";

// =======================
// 🔧 Configurações MQTT
// =======================
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

// Cria o cliente WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// =======================
// 🔧 Configurações RFID
// =======================
//SoftwareSerial RFID(5, 4); // RX, TX
//String text;
//String CardNumber = "5E002AB522";  // Tag autorizada


// =======================
// 🔧 Configurações RFID (RDM6300)
// =======================
// Usando UART2 do ESP32
#define RXD2 16  // pino de recepção (conecta ao TX do RDM6300)
#define TXD2 17  // pino de transmissão (conecta ao RX do RDM6300)
#define RELE_PIN 5 //GPIO para o rele
HardwareSerial RFID(2); // usa UART2

String text;
//String CardNumber = "5E002AB522";  // Tag autorizada


// =======================
// 🚀 Funções auxiliares
// =======================
void wifiConnect() {
  Serial.print("Conectando a Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("esp32_rfid_client")) {
      Serial.println("Conectado!");
      // Se quiser escutar algo do broker:
      client.subscribe("ifrs/sala/POAlab/cmd"); //escuta comandos
    } else {
      Serial.print("Falha (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// =======================
// ⚙️ Setup
// =======================
void setup() {
  Serial.begin(115200);
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);
  //RFID.begin(9600);
  RFID.begin(9600, SERIAL_8N1, RXD2, TXD2); // Inicializa UART2 (pinos 16/17)

  wifiConnect();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  

  Serial.println("Aproxime seu cartão RFID ...");
}

// =======================
// 🔄 Loop principal
// =======================
char c;

void loop() {
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();

  while (RFID.available() > 0) {
    delay(5);
    c = RFID.read();
    text += c;
  }

  if (text.length() > 20) {
    checkCard();
    text = "";
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("Comando recebido: ");
  Serial.println(msg);

  if (msg == "1") {
    Serial.println("✅ Acesso liberado!");
    digitalWrite(RELE_PIN, HIGH);
    delay(2000);
    digitalWrite(RELE_PIN, LOW);
  }
}

void checkCard() {
  text = text.substring(1, 11);
  Serial.println("Card ID : " + text);
  //Serial.println("Access ID : " + CardNumber);

  String message;
  if (text) {
    Serial.println("Acesso aprovado ✅");
    message = text;
  }
  // else {
  // Serial.println("Acesso negado ❌");
  //message = "Negado: " + text;
  // }

  // 📨 Publica no tópico MQTT
  client.publish("ifrs/sala/POAlab", message.c_str());

  delay(2000);
  Serial.println("\nAproxime seu cartão RFID …");
}
