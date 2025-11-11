#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "moto g(8) power 5139";
const char* password = "12345678";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.2.229";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1884);

}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconectar(){
  while(!client.connected()){
    Serial.print("Aguardando conexão...");
      if (client.connect("ESP32Client")) {
        Serial.println("conectado");
        client.subscribe("esp32/data");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

}

void loop() {
  if(!client.connected()){
    reconectar();
  }
  client.loop();

  long now = millis();

  if(now - lastMsg > 5000){
    lastMsg = now;
    client.publish("esp32/data","esp32");
  }

}




