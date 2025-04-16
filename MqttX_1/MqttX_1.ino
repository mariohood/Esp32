
/*********
  Rui Santos
  https://www.youtube.com/watch?v=qPYe9YF5A80&t=5s
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <esp_eap_client.h>

// Replace the next variables with your SSID/Password combination

// const char* ssid = "IFRS-POA";
// const char* username = "87646668087";
// const char* password = "*mariohood1";

#define SSID "IFRS-POA" // SSID da rede
#define USERNAME "87646668087" // Identidade (usuário)
#define PASSWORD "*mariohood1" // Senha

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.15.77";
//const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char Contador = 0;

// LED Pin
const int ledPin = 2;

void setup() {
  Serial.begin(115200);

  // Configura o modo Wi-Fi
WiFi.mode(WIFI_STA);
  
// Configura WPA2-Enterprise (usando as novas funções)
esp_eap_client_set_identity((uint8_t *)USERNAME, strlen(USERNAME));
esp_eap_client_set_username((uint8_t *)USERNAME, strlen(USERNAME));
esp_eap_client_set_password((uint8_t *)PASSWORD, strlen(PASSWORD));
// Conecta ao Wi-Fi
WiFi.begin(SSID);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
    if (String(topic) == "Mario/Teste") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
  //  if (client.connect("ESP8266Client")) {
    if (client.connect("mqttx_6583ad00")) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32/output");
      client.subscribe("Mario/Teste");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    Contador++;

    // Convert the value to a char array
    char tempString[8];
    dtostrf(Contador, 1, 2, tempString);
    Serial.print("Contador: ");    Serial.println(tempString);
    client.publish("Mario/Contador", tempString);
  }
}