/*********
  Mario Ibanez
  Conectar o ESP32 ao WiFi e ao Mosquitto Broker (MQTT)
  https://www.youtube.com/watch?v=b0qEgjdCUc4&list=PLuzyBdM_bvSxeQMJGUJu5X4NKVoYZgpoD&index=17 
*********/

#include <WiFi.h>
#include <PubSubClient.h>

// Configurações para acesso a rede e ao MQTT
const char* ssid = "moto g(8) power 5139";
const char* password = "12345678";
const char* mqtt_server = "test.mosquitto.org"; // Broker público

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay (10);
  Serial.print("Conectando ao ...");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");      
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(15, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}
void Conectado_WiFi()
{
  if(WiFi.status())
  digitalWrite(2,HIGH);
  else
  digitalWrite(2,LOW);
}
void Conectado_Mosquitto()
{
  if(client.connected())
  digitalWrite(15,HIGH);
  else
  digitalWrite(15,LOW);
}

void loop() {
  reconnect_mqtt();
  Conectado_WiFi();
  Conectado_Mosquitto();
}
