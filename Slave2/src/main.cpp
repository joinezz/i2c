#include <Wire.h>
 
#define LED_PIN_1 PB3
#define LED_PIN_2 PA10
 
#define DEBUG_I2C true // Setze auf true, um Debugging zu aktivieren
 
 
 
bool led1Status = false;
bool led2Status = false;
 
uint8_t a = 0; // Variable für Status beider LED's

 
void loop() {
  delay(100);
}
 
 
void receiveEvent(int numBytes) {
  Serial.println("Daten Empfangen");
  while (Wire.available()) { // Solange Daten verfügbar sind
    int c = Wire.read();    // Lies ein Byte
    if (c == 11) {
      digitalWrite(LED_PIN_1, HIGH); // Schalte LED 1 ein
      led1Status = true;
    } else if (c == 10) {
      digitalWrite(LED_PIN_1, LOW);  // Schalte LED 1 aus
      led1Status = false;
    } else if (c == 21) {
      digitalWrite(LED_PIN_2, HIGH); // Schalte LED 2 ein
      led2Status = true;
    } else if (c == 20) {
      digitalWrite(LED_PIN_2, LOW);  // Schalte LED 2 aus
      led2Status = false;
    } else if (c == 30 && led1Status && led2Status) { // Beide LED's an
        a = 3;
        Wire.write(a); // Status der LED's zuruecksenden
    } else if (c == 30 && led1Status == 0 && led2Status) { // LED 1 aus LED 2 an
        a = 1;
        Wire.write(a);
    } else if (c == 30 && led1Status == 0 && led2Status == 0) { // Beide LED's aus
        a = 0;
        Wire.write(a);
    } else if (c == 30 && led1Status && led2Status == 0) { // LED 1 an und LED 2 aus
        a = 2;
        Wire.write(a);
    } else if (c == 40) {
      digitalWrite(LED_PIN_2, HIGH); // Schalte LED 2 ein
      led2Status = true;
      digitalWrite(LED_PIN_1, HIGH); // Schalte LED 1 ein
      led1Status = true;
    } else if (c == 50) {
      digitalWrite(LED_PIN_2, LOW); // Schalte LED 2 aus
      led2Status = false;
      digitalWrite(LED_PIN_1, LOW); // Schalte LED 1 aus
      led1Status = false;
    }
}
}
 
void requestEvent() {
  Serial.println("Anfrage Empfangen");
 
  while (Wire.available()){
    int c = Wire.read();
  }
  Wire.write(a); // Status der LED 1 senden
  Serial.println(a);
}
 
void receiveEvent_2(int numBytes) {
  while (Wire.available()) { // Solange Daten verfügbar sind
    char c = Wire.read();    // Einzelnes empfangenes Byte lesen
    Serial.print("Empfangene Daten: ");
    Serial.println(c); // Empfangene Daten im Seriellen Monitor anzeigen
  }
 
}
 
void setup() {
 
  Wire.setSCL(PB8);
  Wire.setSDA(PB9);
  Serial.begin(115200);
  Serial.println("Test");
  Wire.begin(0x42);               // Initialisiere die Wire-Bibliothek als Slave mit Adresse 42 (Slave 2)
  //Wire.onReceive(receiveEvent); // Wenn Daten empfangen werden, rufe die receiveEvent-Funktion auf
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent); // Wenn Anfrage empfangen wird, rufe die requestEvent-Funktion auf
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
 
}
 