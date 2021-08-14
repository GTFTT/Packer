#include "Packer.h"

//Initialize callback for event
void (*Packer::user_onMessageReady)(char arr[], int size);

Packer::Packer()
{
    outln(F("Library is initialized!"));
}

//----- Public ----------------------------------------------------------------------------------------------

/* Count of packs that can be generated from data of provided length.
Data does not include last empty characters for an array. */
unsigned char Packer::countPacks(int dataLength)
{
    unsigned char count = dataLength / MAX_DATA_SIZE;
    if ((dataLength % MAX_DATA_SIZE) != 0) count++;
    return count;
}

/* For user simplicity, just takes length of an arrary and decreases by one to calculate packs count */
unsigned char Packer::countPacks(char arr[], int size)
{
    return countPacks(size - 1);
}

packsContainer Packer::generatePacks(char message[], int size)
{
    int dataLength = size - 1; //Remove empty character from generation
    packsContainer packsCont;
    const unsigned char packsCount = countPacks(dataLength);
    const unsigned char currentPackNumber = getPackNumber();

    if(packsCount > MAX_PACKS_COUNT) {
        outerr((String) F("MAX PACKS COUNT REACHED(") + packsCount + F("/") + MAX_PACKS_COUNT + F(")"));
        return packsCont;
    } 

    pack packs[packsCount];

    for (unsigned char packId = 0; packId < packsCount; packId++)
    {
        packs[packId] = generatePack(message, size, currentPackNumber, packId);
    }
    
    packsCont.count = sizeof(packs)/sizeof(packs[0]);
    packsCont.lastPackNumber = currentPackNumber;

    for(unsigned char k = 0; k < packsCount; k++) {
        packsCont.packs[k] = packs[k];
    }

    return packsCont;
}

/* This can be used internally or globally, so do not delete any validations */
pack Packer::generatePack(char message[], int size, unsigned char packNumber, unsigned char packId) {
    int dataLength = size - 1; //Remove empty character from generation
    const unsigned char packsCount = countPacks(dataLength);
    pack p;

    if(packsCount > MAX_PACKS_COUNT) {
        outerr((String) F("MAX PACKS COUNT REACHED(") + packsCount + F("/") + MAX_PACKS_COUNT + F(")"));
        return p;
    }

    if(packId >= packsCount || packId < 0) {
        outerr((String) F("Invalid packId provided"));
        return;
    }

    //Calculate payload available size
    unsigned char payloadSize = dataLength - (MAX_DATA_SIZE * packId);
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

    for (unsigned char i = 0; i < payloadSize; i++)
        p.payload[i] = message[i + (MAX_DATA_SIZE * packId)];

    return p;
}

builtPack Packer::buildPack(pack p)
{
    builtPack buffer;

    //Build pack
    unsigned char index = 0;
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

    for (unsigned char i = 0; i < MAX_DATA_SIZE; i++)
    {
        buffer.body[index] = p.payload[i];
        index++;
    }

    return buffer;
}

void Packer::printPack(pack p)
{
    outln(F("-------------------------------"));
    out((String) F("Type: ") + (unsigned char)p.type + F(" | "));
    out((String) F("Number: ") + (unsigned char)p.number + F(" | "));
    out((String) F("ID: ") + (unsigned char)p.id + F(" | "));
    outln((String) F("Payload size: ") + (unsigned char)p.payloadSize);
    out(F("Payload: "));
    out(F("+= "));
    for (unsigned char i = 0; i < p.payloadSize; i++)
        out((String)(char)p.payload[i]);
    outln(F(" =+"));
}

void Packer::printPack(builtPack p)
{
    outln(F("-------------------------------"));
    out(F("Pack: "));
    for (unsigned char i = 0; i < p.size; i++)
        out((String)(char)p.body[i]);
    outln();

    out(F("Bin: "));
    for (unsigned char i = 0; i < p.size; i++){
        out(F(" "));
        out((String)(unsigned char)p.body[i]);
    }
    outln();
}

builtPack Packer::getBuiltPack(char data[], unsigned char size) {
    builtPack bp;

    if(size < 0 || (size-1) > PACK_SIZE) {
        outerr((String) F("Provided data snippet is invalid. Cannot generate built pack."));
        return bp;
    }

    for (unsigned char i = 0; i < (size-1); i++)
    {
        bp.body[i] = data[i];
    }
    
    return bp;
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

        for (unsigned char i = 0; i < buffer.payloadSize; i++)
        {
            buffer.payload[i] = p.body[i + 6];
        }
    }
    else
    {
        outerr((String) F("Invalid pack provided for restoring"));
    }
    return buffer;
}

void Packer::pushPack(pack p) {
    if(packsBuffer.count >= MAX_PACKS_COUNT) {
        outerr(F("packsBuffer is full, cannot add more packs."));
        return;
    }
    //Always initialize last pack number when first pack passed
    if(packsBuffer.count == 0) {
        packsBuffer.lastPackNumber = p.number;
    }

    if(packsBuffer.lastPackNumber != p.number) {
        outerr(F("Pushed pack differs from packs in buffer."));
        clearPacksBuffer();
    }

    packsBuffer.packs[packsBuffer.count] = p; //Insert new pack at the end
    packsBuffer.count++;

    checkPacksBuffer();
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

void Packer::onMessageReady( void (*function)(char arr[], int size) )
{
  user_onMessageReady = function;
}

//----- Private ---------------------------------------------------------------------------------------------

void Packer::out(String message) {
    if(!USE_DEBUG) return;

    Serial.print(message);
}

void Packer::outerr(String errorMessage) {
    if(!USE_DEBUG) return;

    Serial.println((String) F("[ ERROR ]: ") + errorMessage);
}

void Packer::outln(String message) {
    if(!USE_DEBUG) return;

    Serial.println(message);
}

void Packer::outln(void) {
    if(!USE_DEBUG) return;

    Serial.println();
}

void Packer::clearPacksBuffer(void) {
    packsBuffer.count = 0; // Clearing static array is not necessary
    out(F("Buffer cleared"));
}

void Packer::checkPacksBuffer(void) {
    bool containsPackOfTypeFour = false;
    bool containsPacksOfTypeOne = false;
    bool containsPacksOfTypeTwo = false;
    bool containsPacksOfTypeThree = false;
    bool containsPacksOfUnknownTypes = false;

    bool containsProperPackIdSequence = true;

    for(unsigned char i = 0; i < packsBuffer.count; i++) {

        //Validations
        if(packsBuffer.packs[i].type == 4)
            containsPackOfTypeFour = true;
        else if(packsBuffer.packs[i].type == 1)
            containsPacksOfTypeOne = true;
        else if(packsBuffer.packs[i].type == 2)
            containsPacksOfTypeTwo = true;
        else if(packsBuffer.packs[i].type == 3)
            containsPacksOfTypeThree = true;
        else containsPacksOfUnknownTypes = true;

        if(packsBuffer.packs[i].id != i) containsProperPackIdSequence = false;
    }

    //Messages output when something go wrong
    if(!containsProperPackIdSequence) {
        outerr(F("Incorrect packId sequence"));
        clearPacksBuffer();
        return;
    }

    if(containsPacksOfUnknownTypes) {
        outerr(F("Unknown type of pack was detected"));
        clearPacksBuffer();
        return;
    }

    if(containsPacksOfTypeOne && containsPacksOfTypeTwo && containsPacksOfTypeThree) {
        outln(F("OK: Lets build a message from packs of type 1, 2 and 3"));
        buildDataFromPacksBuffer();
        clearPacksBuffer();
        return;
    }

    if(containsPackOfTypeFour) {
        outln(F("OK: Lets build a message from packs of type 4"));
        buildDataFromPacksBuffer();
        clearPacksBuffer();
        return;
    }

    //No problems was found but message cannot be generated yet
    // out(F("No problems was found but message cannot be generated yet"));
    return;
}


void Packer::buildDataFromPacksBuffer(void) {

    if(!user_onMessageReady){
        outerr(F("No callback for an event, data will not be tranmitted to a user(will be lost)"));
        return;
    }

    int dataSize = 0;
    for(unsigned char i = 0; i < packsBuffer.count; i++) {
        dataSize += packsBuffer.packs[i].payloadSize;
    }

    char data[dataSize] = {0};
    int dataIndex = 0;

    for(unsigned char i = 0; i < packsBuffer.count; i++) {
        for(unsigned char j = 0; j < packsBuffer.packs[i].payloadSize; j++) {
            data[dataIndex] = packsBuffer.packs[i].payload[j];
            dataIndex++;
        }
    }

    out(F("Generated data: "));
    for (int i = 0; i < sizeof(data); i++)
    {
        out((String) data[i]);
    }
    outln();

    //Call user callback
    user_onMessageReady(data, sizeof(data));
}