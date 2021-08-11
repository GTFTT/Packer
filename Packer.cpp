#include "Packer.h"

Packer::Packer()
{
    Serial.println("Library is initialized!");
}

//----- Public ----------------------------------------------------------------------------------------------

/* Count of packs that can be generated from data of provided length.
Data does not include last empty characters for an array. */
int Packer::countPacks(int dataLength)
{
    int count = dataLength / MAX_DATA_SIZE;
    if ((dataLength % MAX_DATA_SIZE) != 0) count++;
    return count;
}

packsContainer Packer::generatePacks(char message[], int size)
{
    int dataLength = size - 1; //Remove empty character from generation
    packsContainer packsCont;
    const int packsCount = countPacks(dataLength);
    const int currentPackNumber = getPackNumber();

    pack packs[packsCount];

    for (int packId = 0; packId < packsCount; packId++)
    {
        //Calculate payload available size
        int payloadSize = dataLength - (MAX_DATA_SIZE * packId); //Minus one to prevent empty character at the end of an array
        if (payloadSize > MAX_DATA_SIZE)
            payloadSize = MAX_DATA_SIZE;

        pack p;

        if (packId == 0 && (packId + 1) != packsCount)
            p.type = 1;
        if (packId != 0 && (packId + 1) < packsCount)
            p.type = 2;
        if (packId > 0 && (packId + 1) == packsCount)
            p.type = 3;
        if (packId == 0 && (packId + 1) == packsCount)
            p.type = 4;

        p.number = currentPackNumber;
        p.id = packId;
        p.payloadSize = payloadSize;

        for (int i = 0; i < payloadSize; i++)
            p.payload[i] = message[i + (MAX_DATA_SIZE * packId)];

        packs[packId] = p;
    }
    
    packsCont.count = sizeof(packs)/sizeof(packs[0]);
    packsCont.packs = packs;

    return packsCont;
}

builtPack Packer::buildPack(pack p)
{
    builtPack buffer;

    //Build pack
    int index = 0;
    buffer.body[index] = PACK_DIVIDE_SIGN; //Start symbol
    index++;

    buffer.body[index] = p.type;
    index++;

    buffer.body[index] = p.number;
    index++;

    buffer.body[index] = p.id;
    index++;

    buffer.body[index] = p.payloadSize;
    index++;

    buffer.body[index] = PACK_DIVIDE_SIGN;
    index++;

    for (int i = 0; i < MAX_DATA_SIZE; i++)
    {
        buffer.body[index] = p.payload[i];
        index++;
    }

    return buffer;
}

void Packer::printPack(pack p)
{
    Serial.println("-------------------------------");
    Serial.print((String)"Type: " + (int)p.type + " | ");
    Serial.print((String)"Number: " + (int)p.number + " | ");
    Serial.print((String)"ID: " + (int)p.id + " | ");
    Serial.println((String)"Payload size: " + (int)p.payloadSize);
    Serial.print("Payload: ");
    Serial.print("+= ");
    Serial.print(p.payload);
    Serial.println(" =+");
}

void Packer::printPack(builtPack p)
{
    Serial.println("-------------------------------");
    Serial.print("Pack: ");
    for (int i = 0; i < p.size; i++)
        Serial.print((char)p.body[i]);
    Serial.println();

    Serial.print("Bin: ");
    for (int i = 0; i < p.size; i++){
        Serial.print(" ");
        Serial.print( (int)p.body[i]);
    }
    Serial.println();
}

pack Packer::restorePack(builtPack p)
{
    pack buffer;

    if (p.body[0] == PACK_DIVIDE_SIGN && p.body[5] == PACK_DIVIDE_SIGN)
    {
        buffer.type = p.body[1];
        buffer.number = p.body[2];
        buffer.id = p.body[3];
        buffer.payloadSize = p.body[4];

        for (int i = 0; i < buffer.payloadSize; i++)
        {
            buffer.payload[i] = p.body[i + 6];
        }
    }
    else
    {
        Serial.println("Throw error here!");
    }
    return buffer;
}

/*
Last character of an array is counted too. "Hello" has 6 characters.
JSON variant of type string has two more symbols at the begining and at the end: ' " '.
serializeJson - Standard JSON
serializeMsgPack - Really serialized and compressed binary object
I can use serial in my library if it is defined some where!
*/
String Packer::test(char arr[], int size)
{

    char message[size]; //Create copy of an input array
    for (int i = 0; i < size; i++)
    {
        message[i] = arr[i];
    }

    // allocate the memory for the document
    DynamicJsonDocument doc(1024);

    doc["test"] = 234123;
    // JsonVariant variant = doc.to<JsonVariant>();
    // variant.set(message);

    int jsonSize = measureJson(doc);
    char buffJSON[jsonSize];

    int msgPackSize = measureMsgPack(doc);
    char buffMsgPack[msgPackSize];

    int writenBytesJSON = serializeJson(doc, buffJSON, jsonSize);
    int writenBytesMsgPack = serializeMsgPack(doc, buffMsgPack, msgPackSize);

    String res = "";

    res += "Size pass: " + (String)size + "\n\n";

    res += "jsonSize: " + (String)jsonSize + "\n";
    res += "msgPackSize: " + (String)msgPackSize + "\n\n";

    res += "writenBytesJSON: " + (String)writenBytesJSON + "\n";
    res += "writenBytesMsgPack: " + (String)writenBytesMsgPack + "\n\n";

    res += "Passed: ";
    for (int i = 0; i < sizeof(message); i++)
        res += message[i];
    res += "\n\n";

    res += "Serialized JSON: ";
    for (int i = 0; i < sizeof(buffJSON); i++)
        res += buffJSON[i];
    res += "\n";

    res += "Serialized MsgPack: ";

    Serial.println(res);

    for (int i = 0; i < sizeof(buffMsgPack); i++)
        Serial.print(buffMsgPack[i]);
    Serial.println();

    return res;
}

//----- Private ---------------------------------------------------------------------------------------------

/* Automatically increases pack number after each call */
unsigned char Packer::getPackNumber(void)
{
    return packNo++;
}