#ifndef Packer_h
#define Packer_h

//Standard stuff
#include "Arduino.h"

//Own stuff

#define PACK_DIVIDE_SIGN '|'
#define PACK_SIZE 32
#define MAX_DATA_SIZE (PACK_SIZE - 6) // Brackets are required to perform operation before this will be used
#define MAX_PACKS_COUNT 11

/* For documentation of this type see documentation of a Class */
struct pack
{
	unsigned char type = 0;
	unsigned char number = 0;
	unsigned char id = 0;
	unsigned char payloadSize = 0;
	char payload[MAX_DATA_SIZE] = {0}; //Initialize buffer with zeros
};

/* Contains array of generated packs and count of them. Use this with mind, it can create stack overflow*/
struct packsContainer
{
	unsigned char count = 0;
	pack packs[MAX_PACKS_COUNT]; //Initialize buffer with zeros
};

/* This type of pack is kind of compilled and its body can be sent directly into physical communication system */
struct builtPack
{
	const unsigned char size = PACK_SIZE;
	char body[PACK_SIZE] = {0}; //Initialize buffer with zeros
};

/*
	Arduino UNO can handle maximum up to 271 caracters(out of memory) when generating all packs at a time.
	This libary works ONLY with char arrays as data strings!

	Packer - The purpose of this library if to convert arrays of characters into packs
	that can be converted into 32 bytes data strings. 

	Protocol of package(pack): Pack is a struct value which consists of:
	 - "type" type of a pack
	    = "1" Start of a message(first pack)
	    = "2" Somewhere middle of a message(not first and not last)
	    = "3" End of a message(last pack of a message)
	    = "4" One full message(one pack contains all entire message)
	 - "number" Number of a pack(all packs of a one messages have the same number)
	 - "id" used to determine the sequence when message from packages have to be rebuild
	 - "payloadSize" size of payload that is located in that pack (maximum payload size is 26 characters)
	 - "payload" part of a message(26 characters array)

	Header contains metadata in BYTE format(0-255). Metadata is delimeted with delimiter character
	Message example: |[4][0][0][26]|12345678901234567890123456

	Created by:    GT
	Release date: 12.08.2021
*/
class Packer
{
public:
	Packer(void);

	/* How many packs will be generated from provided data */
	int countPacks(int dataLength);
	int countPacks(char arr[], int size);

	/* Generate array of packs that can be used later. Takes a lot of memory. */
	packsContainer generatePacks(char message[], int size);

	/* Generate a pack which can be built later. Specify id of a pack. */
	pack generatePack(char message[], int size, unsigned char packNumber, unsigned char packId);

	/* "Compiles" pack into version that is ready to be sent. */
	builtPack buildPack(pack p);

	/* Print pack that is not build yet. Uses Serial monitor */
	void printPack(pack p);

	/* Print builded pack. Uses Serial monitor */
	void printPack(builtPack p);

	/* Takes body of a back and generates special built pack. It can be used later to recompile message from packs. */
	builtPack Packer::getBuiltPack(char data[], unsigned char size);

	/* Restore builded pack into normal(easy to read and use) pack */
	pack restorePack(builtPack p);

	/* Get new pack number, automatically increases last pack number. */
	unsigned char getPackNumber(void);

	/* Set USE_DEBUG value, if true, serial output will be enabled */
	void setDebug(bool value);
private:
	/* Last pack number, each message has its set of generated packs with the same number */
	unsigned char packNo = 0;

	/* If true - there will be used Serial for output */
	bool USE_DEBUG = false;

	/* Print message. Available only if USE_DEBUG. */
	void out(String message);

	/* Print with new line. Available only if USE_DEBUG. */
	void outln(String message);
	
	/* Print error message. Available only if USE_DEBUG. */
	void outerr(String errorMessage);

	/* Print empty line. Available only if USE_DEBUG. */
	void outln(void);
};

#endif