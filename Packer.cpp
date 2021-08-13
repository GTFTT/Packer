#include "Packer.h"

Packer::Packer()
{
    outln("Library is initialized!");
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

/* For user simplicity, just takes length of an arrary and decreases by one to calculate packs count */
int Packer::countPacks(char arr[], int size)
{
    return countPacks(size - 1);
}

packsContainer Packer::generatePacks(char message[], int size)
{
    int dataLength = size - 1; //Remove empty character from generation
    packsContainer packsCont;
    const int packsCount = countPacks(dataLength);
    const int currentPackNumber = getPackNumber();

    if(packsCount > MAX_PACKS_COUNT) {
        outln((String)"[ ERROR ] - MAX PACKS COUNT REACHED(" + packsCount + "/" + MAX_PACKS_COUNT + ")");
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

pack Packer::generatePack(char message[], int size, unsigned char packNumber, unsigned char packId) {
    int dataLength = size - 1; //Remove empty character from generation
    const int packsCount = countPacks(dataLength);
    pack p;

    if(packsCount > MAX_PACKS_COUNT) {
        outerr((String)"MAX PACKS COUNT REACHED(" + packsCount + "/" + MAX_PACKS_COUNT + ")");
        return p;
    }

    if(packId >= packsCount || packId < 0) {
        outerr((String) "Invalid packId provided");
        return;
    }

    //Calculate payload available size
    int payloadSize = dataLength - (MAX_DATA_SIZE * packId);
    if (payloadSize > MAX_DATA_SIZE)
        payloadSize = MAX_DATA_SIZE;

    //Define package type
    if (packId == 0 && (packId + 1) != packsCount)
        p.type = 1;
    if (packId != 0 && (packId + 1) < packsCount)
        p.type = 2;
    if (packId > 0 && (packId + 1) == packsCount)
        p.type = 3;
    if (packId == 0 && (packId + 1) == packsCount)
        p.type = 4;

    p.number = packNumber;
    p.id = packId;
    p.payloadSize = payloadSize;

    for (int i = 0; i < payloadSize; i++)
        p.payload[i] = message[i + (MAX_DATA_SIZE * packId)];

    return p;
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
    outln("-------------------------------");
    out((String)"Type: " + (int)p.type + " | ");
    out((String)"Number: " + (int)p.number + " | ");
    out((String)"ID: " + (int)p.id + " | ");
    outln((String)"Payload size: " + (int)p.payloadSize);
    out("Payload: ");
    out("+= ");
    for (int i = 0; i < p.payloadSize; i++)
        out((String)(char)p.payload[i]);
    outln(" =+");
}

void Packer::printPack(builtPack p)
{
    outln("-------------------------------");
    out("Pack: ");
    for (int i = 0; i < p.size; i++)
        out((String)(char)p.body[i]);
    outln();

    out("Bin: ");
    for (int i = 0; i < p.size; i++){
        out(" ");
        out((String)(int)p.body[i]);
    }
    outln();
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
        outln("Throw error here!");
    }
    return buffer;
}

void Packer::setDebug(bool value) {
    USE_DEBUG = value;
}

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

//----- Private ---------------------------------------------------------------------------------------------

void Packer::out(String message) {
    if(!USE_DEBUG) return;

    Serial.print(message);
}

void Packer::outerr(String errorMessage) {
    if(!USE_DEBUG) return;

    Serial.print((String)"[ ERROR ]: " + errorMessage);
}

void Packer::outln(String message) {
    if(!USE_DEBUG) return;

    Serial.println(message);
}

void Packer::outln(void) {
    if(!USE_DEBUG) return;

    Serial.println();
}