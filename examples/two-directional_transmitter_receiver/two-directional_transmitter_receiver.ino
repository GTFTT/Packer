/*
* Bidirectional transmitting code
* Copy of this program is receiver and transmitter at the same time(launch the same program for two arduinos with NRF24L01+).
* Rember: The fastest way to transmit data is to supply small messages. If your message
* is small(about 25-26 characters long) you can decrease sending interval and it will increase transmition speed.
* If your messages are about two-five pack it is recommended to leave default settings.
* If your messages are very log - decrease sending interval - it will make your program more stable.
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Packer.h>
#include <Chrono.h>

#define MESSAGE_SENDING_INTERVAL 30 //Decrease this to increase speed if your message is small, else it can unpredictible count of packs to be sent(mean more unstable)
#define MESSAGE_RANDOM_MAX_EXTRACTOR -10 //Max time to extarct randomly when pack is sending
#define MESSAGE_RANDOM_MAX_ADDER 10 //Max time to add randomly when pack is sending
#define MESSAGE_READING_INTERVAL 0
#define MESSAGE_READING_TIME 0
#define SECOND_READING_DELAY 0
#define SETUP_DELAY 5
#define COUNTER_UPDATING_INTERVAL 1000

Chrono sendRFData;
Chrono readRFData;
Chrono counterUpdater;
Packer packer;
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

int RANDOMED_SENDING_INTERVAL = MESSAGE_SENDING_INTERVAL; //This value will be randomly changed after each send to prevent to arduinos work synchronously
unsigned long startTime = 0;
unsigned long endTime = 0;
int counter = 0;
int overallSize = 0;
unsigned long sendingSum = 0;
unsigned long sendCount = 0;
unsigned long sendCountFailed = 0;
unsigned long receivedFullMessages = 0;

bool RF_READING_MODE = false;

//Init data
// const char text[] = "Hello World";
char text[] = "{test: 12.24, sensorX: 1023, sensorY: 255, sensorZ: \"Hello\"}";
// char text[] = "123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|";
// char text[] = "123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|";

void setup() {
  Serial.begin(115200);
  packer.onMessageReady(messageReceiveHandler);
  packer.setDebug(false);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setRetries(2, 2);
  setupRFForWriting();
  Serial.println((String) F("Data size: ") + sizeof(text));
}

void loop() {
  emulatePayload();

  sendImportantDataLoop();
  readImportantDataLoop();

  if(counterUpdater.hasPassed(COUNTER_UPDATING_INTERVAL, true)) {
    Serial.println((String) "Packs/per/second: " + counter + "pps | " + overallSize + " bytes received");
    if(sendCount != 0) Serial.println((String) F("averageSendingTime: ") + (sendingSum/sendCount));
    Serial.println((String) F("sendCount(messages): ") + sendCount);
    Serial.println((String) F("sendCountFailed(messages): ") + sendCountFailed);
    Serial.println((String) F("receivedFullMessages: ") + receivedFullMessages);
    Serial.println();
    counter = 0;
    overallSize = 0;
    sendingSum = 0;
    sendCount = 0;
    sendCountFailed=0;
    receivedFullMessages = 0;
  }
}

//---------------------------------------------------------------------

//Emylate that arduino is doing something
void emulatePayload() {
  delay(4);
}

// Adjust for writing (0-1 ms)
void setupRFForWriting() {
  radio.openWritingPipe(address);
  radio.stopListening();
  RF_READING_MODE = false;
}

// Adjust for reading (0-1 ms)
void setupRFForReading() {
  radio.openReadingPipe(0, address);
  radio.startListening();
  RF_READING_MODE = true;
  delay(SETUP_DELAY);  //This is required to prevent data lost, somehow data is not readed without it
}

// Send data to another RF (time depends on quantity of packs)
// If something fails(at least one message is not sent - exit immidietly)
void sendImportantDataLoop() {
  if(sendRFData.hasPassed(MESSAGE_SENDING_INTERVAL, true)) {
    if(RF_READING_MODE) setupRFForWriting();
    
    bool sentSuccess = false;
    packsContainer pc = packer.generatePacks(text, sizeof(text));

    startTime = millis();
    for(int i = 0; i < pc.count; i++) {
      builtPack built = packer.buildPack(pc.packs[i]);
      sentSuccess = radio.write(&built.body, built.size+1);
      if(!sentSuccess) {
        break;
      }
    }
    endTime = millis();
    sendingSum += (endTime-startTime);
    if(!sentSuccess) {
        sendCountFailed++;
    } else {
      sendCount++;
    }
  }
}

//read all data that is transmitted to the device, is called very friquently
void readImportantDataLoop() {
  if(readRFData.hasPassed(MESSAGE_READING_INTERVAL, true)) {
    unsigned long readingStartTime = millis();
    unsigned long readingEndTime = readingStartTime;
    for(int i = 0; i < 2; i++) {
      while((readingEndTime - readingStartTime) <= MESSAGE_READING_TIME) {
        if(!RF_READING_MODE) setupRFForReading();

        RANDOMED_SENDING_INTERVAL = MESSAGE_SENDING_INTERVAL + random(MESSAGE_RANDOM_MAX_EXTRACTOR, MESSAGE_RANDOM_MAX_ADDER);

        if (radio.available()) {
          char text[32] = "";
          radio.read(&text, sizeof(text));
          builtPack built = packer.getBuiltPack(text, sizeof(text));
          pack restoredPack = packer.restorePack(built);
          packer.pushPack(restoredPack);
          counter++;
          overallSize += restoredPack.payloadSize;
        }
        readingEndTime = millis();
      }
      delay(SECOND_READING_DELAY);
    }
  }
}

//If message was received
void messageReceiveHandler(char arr[], int size) {
  receivedFullMessages++;
  // Serial.print("Received message: ");
  // for(int i = 0; i < size; i++) {
  //   Serial.print(arr[i]);
  // }
  // Serial.println();
}