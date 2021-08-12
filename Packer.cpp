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

    if(packsCount > MAX_PACKS_COUNT) {
        Serial.println((String)"[ ERROR ] - MAX PACKS COUNT REACHED(" + packsCount + "/" + MAX_PACKS_COUNT + ")");
        return packsCont;
    } 

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

    for(int k = 0; k < packsCount; k++) {
        packsCont.packs[k] = packs[k];
    }

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
    for (int i = 0; i < p.payloadSize; i++)
        Serial.print((char)p.payload[i]);
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

//----- Private ---------------------------------------------------------------------------------------------

/*
Automatically increases pack number after each call.
If pack number is too big it will reset it and start counting again.
*/
unsigned char Packer::getPackNumber(void)
{
    packNo++;
    if(packNo > 255) packNo = 0;

    return packNo;
}