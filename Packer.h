#ifndef Packer_h
#define Packer_h

//Standard stuff
#include "Arduino.h"

#include <ArduinoJson.h> /* Process JSON documents, serialization, packs and other*/

#ifndef BITMAPS_H
#define BITMAPS_H



//Own stuff

#define PACK_DIVIDE_SIGN '|'
#define PACK_SIZE 32
#define MAX_DATA_SIZE (PACK_SIZE - 6) // Brackets are required to perform operation before this will be used
#define MAX_PACKS_COUNT 11

struct pack
{
	unsigned char type = 0;
	unsigned char number = 0;
	unsigned char id = 0;
	unsigned char payloadSize = 0;
	char payload[MAX_DATA_SIZE] = {0}; //Initialize buffer with zeros
};

/* Contains array of generated packs and count of them */
struct packsContainer
{
	unsigned char count = 0;
	pack packs[MAX_PACKS_COUNT]; //Initialize buffer with zeros
};

struct builtPack
{
	const unsigned char size = PACK_SIZE;
	char body[PACK_SIZE] = {0}; //Initialize buffer with zeros
};

/*
	Arduino UNO can handle maximum of 271 caracters(out of memory)

	Packer - Use to convert data into special strings structure and decode it later into values.
	There are also available methods to send raw data.

	The protocol used by this library is not the best yet, it transmits data by packages of max size 32 bytes.
	It is not so efficient but it have to work anyway. Future protocol have to be changed to be faster,
	less weight and more compact;

	Current protocol: {"m": [1, 2, 3], "d": ""}
	- "m" stands for meta, contains data in that order(type of payload, package number, package id)
	- "d" stands for data, there is actually piece of JSON document we wont to transmit

	Future protocol: | serialized meatdata here... | serialized data here... |

	Created by:    GT
	Release date: 08.08.2021
*/
class Packer
{
public:
	Packer(void);

	String test(char arr[], int size);

	int countPacks(int dataLength);
	packsContainer generatePacks(char message[], int size);
	builtPack buildPack(pack p);
	void printPack(pack p);
	void printPack(builtPack p);
	pack restorePack(builtPack p);
private:
	unsigned char packNo = 0;

	unsigned char getPackNumber(void);
};

#endif 

#endif