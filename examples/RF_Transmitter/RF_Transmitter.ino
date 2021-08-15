/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
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

//Init data
// const char text[] = "Hello World12345678912345678901";
char text[] = "{test: 12.24, sensorX: 1023, sensorY: 255, sensorZ: \"Hello\"}";
// char text[] = "123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|";

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openWritingPipe(address);
  radio.stopListening();
  Serial.println((String)F("Used RAM Setup: ") + memoryUsage());
}

unsigned long startTime = 0;
unsigned long endTime = 0;

void loop() {
  Serial.println((String) F("Data size: ") + sizeof(text));
  startTime = millis();
  packsContainer pc = packer.generatePacks(text, sizeof(text));
  for(int i = 0; i < pc.count; i++) {
    builtPack built = packer.buildPack(pc.packs[i]);
    radio.write(&built.body, built.size+1);    
  }
  // radio.write(&text, sizeof(text));
  endTime = millis();
  Serial.println((String) F("Generating time: ") + (endTime-startTime));

  Serial.println((String)F("Used RAM Loop: ") + memoryUsage());

  while(true);
}
