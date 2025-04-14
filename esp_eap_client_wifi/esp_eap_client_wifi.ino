/*********
  Mario Ibanez
  Conectar Rede WiFi (SSID) com Usuario e Senha, usando a biblioteca
  esp_eap_client.h  
*********/

#include <WiFi.h>
#include "esp_eap_client.h" // Substitui esp_wpa2.h

// Configurações da rede WPA2-Enterprise
#define SSID "IFRS-POA" // SSID da rede (ex: "Empresa_XYZ")
#define USERNAME "87646668087" // Identidade (usuário)
#define PASSWORD "*mariohood1" // Senha

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

  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Seu código aqui
}