#ifndef Packer_h
#define Packer_h

//Standard stuff
#include "Arduino.h"

#include <ArduinoJson.h> /* Process JSON documents, serialization, packs and other*/

/*
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

    String test(void);

  private:
    int TRANSMITTING_DELAY; //Time for transmitting a message

    int getLastPackId(void);
};

#endif