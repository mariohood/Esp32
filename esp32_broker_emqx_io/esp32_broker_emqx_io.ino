#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_eap_client.h"

// Configurações da rede WPA2-Enterprise
#define SSID "IFRS-POA"
#define USERNAME "87646668087"
#define PASSWORD "*mariohood1"

// Configurações MQTT
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/led_control";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.begin(115200);
  
  // Configura o modo Wi-Fi
  WiFi.mode(WIFI_STA);
  
  // Configura WPA2-Enterprise
  esp_eap_client_set_identity((uint8_t *)USERNAME, strlen(USERNAME));
  esp_eap_client_set_username((uint8_t *)USERNAME, strlen(USERNAME));
  esp_eap_client_set_password((uint8_t *)PASSWORD, strlen(PASSWORD));
  
  // Conecta ao Wi-Fi
  WiFi.begin(SSID);

  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload para String
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensagem: ");
  Serial.println(message);

  // Controle do LED_BUILTIN (ativo em LOW na maioria dos ESP32)
  if (message == "ON") {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED LIGADO!");
  } else if (message == "OFF") {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED DESLIGADO!");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    
    // Cria um ID de cliente aleatório
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      // Subscreve no tópico
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Inicia com LED desligado
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}