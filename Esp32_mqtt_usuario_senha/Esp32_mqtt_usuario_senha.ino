#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "moto g(8) power 5139";
const char* password = "12345678";

// Configurações MQTT
const char* mqtt_server = "test.mosquitto.org"; // Broker público
const int mqtt_port = 1883; // Porta padrão MQTT
const char* mqtt_topic = "esp32/led"; // Tópico para controlar o LED

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe(mqtt_topic); // Assina o tópico para receber comandos
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// Função chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Controle do LED (ON/OFF)
  if (message == "ON") {
    digitalWrite(LED_BUILTIN, LOW); // LED acende (LOW para ESP32)
    Serial.println("LED ligado!");
  } else if (message == "OFF") {
    digitalWrite(LED_BUILTIN, HIGH); // LED apaga (HIGH para ESP32)
    Serial.println("LED desligado!");
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Inicia desligado
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Define a função de callback
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); // Mantém a conexão MQTT ativa
}