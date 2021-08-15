#include <Packer.h>

Packer packer;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  packer.setDebug(true);
  packer.onMessageReady(eventHandler);
  Serial.println((String)"Used RAM1: " + memoryUsage());

  // char message[] = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789X";
  // char message[] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
  // char message[] = "12345678901234567890123456789012";
  char message[] = "{\"test\": \"Hello\",\"temp\": 123.9032,\"sensorX\":7832.3829}123456789012345678901234";
  // char message[] = "{test: \"Hello\",temp: 123.9032}";
  Serial.println((String)"Data size: " + sizeof(message));
  
  // Serial.println((String)"Used RAM2: " + memoryUsage());

  // packsContainer packsCont = packer.generatePacks(message, sizeof(message));
  // Serial.println((String)"\n\nPacks count: " +  packsCont.count);
  // for(int i = 0; i < packsCont.count; i++) {
  //   packer.printPack(packsCont.packs[i]);
  //   builtPack built = packer.buildPack(packsCont.packs[i]);
  //   Serial.println((String)"Built pack size: " + built.size);
  //   // pack restoredPack = packer.restorePack(packer.getBuiltPack(built.body, built.size));
  //   Serial.println("--------");
  //   // packer.printPack(restoredPack);
  // //   Serial.println("\n\n");
  // //   Serial.println((String)"Used RAM X: " + memoryUsage());
  // }

  Serial.println((String)"Used RAM3: " + memoryUsage());
  unsigned char packNo = packer.getPackNumber();
  // --------------------------------------------
  for(int i = 0; i < packer.countPacks(message, sizeof(message)); i++) {
    pack generatedPack = packer.generatePack(message, sizeof(message), packNo, i);
    Serial.println((String) "See pack: ");    
    packer.printPack(generatedPack);
    Serial.println((String)"Used RAMX: " + memoryUsage());
    builtPack built = packer.buildPack(generatedPack);
    Serial.println((String) "Built pack: ");
    packer.printPack(packer.getBuiltPack(built.body, built.size+1));
    // Serial.println((String)"Built pack size: " + built.size);
    pack restoredPack = packer.restorePack(packer.getBuiltPack(built.body, built.size + 1)); //Note: in real case you not always want to increase size by one(only for literal char arrays with last null character)
    // Serial.println("--------");
    packer.printPack(restoredPack);
    packer.pushPack(restoredPack);
    Serial.println("\n");
    Serial.println((String)"Used RAM X: " + memoryUsage());
  }  

  Serial.println((String)"Used RAM4: " + memoryUsage());
}
String globOutput = "";

void loop() {
  Serial.println((String) "Global: " + globOutput);
  while(true);
}

void eventHandler(char arr[], int size) {
  Serial.println("-----------------------------------\nData received!!!");
  Serial.println((String) "Size of data: " + size);
  Serial.print("Data: ");
  globOutput = "";
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    globOutput += (String) arr[i];
  }
  
  Serial.println();
}

int freeRAM() {
  int value = 0;
  int result = 0;
  extern int *__brkval;
  extern int __heap_start;
  result = (int)&value - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval);
  return result;
}

/* How many RAM was used */
String memoryUsage() {
  float free = freeRAM();
  return (String)(100.0-(free/2048.0)*100.0) + "% | Used bytes: " + (int)(2048-free) + " of 2048";
}


