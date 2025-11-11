// RDM6300 RDM630 RFID Reader interfacing with ESP32
// https://microcontrollerslab.com/rdm6300-rdm630-rfid-reader-esp32-tutorial/
// para informar minha extensão Tactiq: https://tactiq.io/r/transcribe 

#include <SoftwareSerial.h>
SoftwareSerial RFID(5,4); // RX and TX

String text;
String CardNumber;

void setup()
{
  Serial.begin(115200);
  RFID.begin(9600);
  Serial.println("Aproxime seu cartão RFID ...");
  CardNumber = "5E002AB522";
  
}
char c;

void loop() {
  while (RFID.available() > 0) {
    delay(5);
    c = RFID.read();
    text += c;
  }
  if (text.length() > 20)
  check();
  text = "";
}

void check() {
  text = text.substring(1, 11);
  Serial.println("Card ID : " + text);
  Serial.println("Access ID : " + CardNumber);

  if (CardNumber.indexOf(text) >= 0) {
    Serial.println("Acesso aprovado");
  }
  else {
    Serial.println("Acesso negado");
  }
  delay(2000);
  Serial.println(" ");
  Serial.println("Aproxime seu cartão RFID …");
}


