#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_eap_client.h> // Biblioteca para EAP

// Configurações Wi-Fi EAP
#define SSID "IFRS-POA"
#define USERNAME "87646668087" // Seu usuário (identity)
#define PASSWORD "*mariohood1" // Sua senha

// Configurações MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/led_control";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(SSID);
  // Configuração EAP (WPA2-Enterprise)
  //WiFi.disconnect(true); // Remove configurações anteriores
  WiFi.mode(WIFI_STA);  
  // Configura WPA2-Enterprise (usando as novas funções)
  esp_eap_client_set_identity((uint8_t *)USERNAME, strlen(USERNAME));
  esp_eap_client_set_username((uint8_t *)USERNAME, strlen(USERNAME));
  esp_eap_client_set_password((uint8_t *)PASSWORD, strlen(PASSWORD));
  
  WiFi.begin(SSID); // Sem senha, pois a autenticação é via EAP

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado (EAP)!");
  Serial.println("IP: " + WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida: ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "ON") {
    digitalWrite(LED_BUILTIN, LOW); // Liga (ativo em LOW)
    Serial.println("LED LIGADO");
  } else if (message == "OFF") {
    digitalWrite(LED_BUILTIN, HIGH); // Desliga
    Serial.println("LED DESLIGADO");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    String clientId = "ESP32-EAP-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Inicia desligado
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  delay (2000);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}