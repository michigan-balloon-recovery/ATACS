#include <rockblock.h>
#include <pads.h>

RockBLOCK::RockBLOCK(padpin &rxd, padpin &txd, padpin &netav, padpin &ri, padpin &onoff) :
    rxd(rxd), txd(txd), netav(netav), ri(ri), onoff(onoff) {
    // Assumes all pins initialized to outputs
    MSP_Pads::setDir(txd, DIR_IN);
    MSP_Pads::setDir(netav, DIR_IN);
    MSP_Pads::setDir(ri, DIR_IN);
    MSP_Pads::setDir(onoff, DIR_IN);
}

bool RockBLOCK::network_available(){
    return MSP_Pads::read(netav);
}
