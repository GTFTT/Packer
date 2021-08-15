/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Packer.h>

Packer packer;
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

void messageReceiveHandler(char arr[], int size);

void setup() {
  Serial.begin(115200);
  packer.onMessageReady(messageReceiveHandler);
  packer.setDebug(true);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

unsigned long startTime = 0;
unsigned long endTime = 0;

void loop() {
  startTime = millis();

  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    // Serial.println(text);
    endTime = millis();
    builtPack built = packer.getBuiltPack(text, sizeof(text));
    packer.printPack(built);
    packer.pushPack(packer.restorePack(built));
    Serial.println((String) "Result: " + (endTime-startTime));
  }

  // endTime = millis();
  // Serial.println((String) "Result: " + (endTime-startTime));
}

void messageReceiveHandler(char arr[], int size) {
  Serial.print("Received pack: ");
  for(int i = 0; i < size; i++) {
    Serial.print(arr[i]);
  }
  Serial.println();
}