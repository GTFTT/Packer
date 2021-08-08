#include "Packer.h"

Packer::Packer() {
  TRANSMITTING_DELAY = 0; //Send as fast as possible
}

//----- Public ----------------------------------------------------------------------------------------------


String Packer::test(void) {
  return (String) "Done";
}

//----- Private ---------------------------------------------------------------------------------------------

int Packer::getLastPackId(void) {
  return 0;
}
