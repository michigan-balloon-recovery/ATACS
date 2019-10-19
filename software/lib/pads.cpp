#include <pads.h>

void MSP_Pads::setDir(padpin p, bool dir){
    if(dir){
        *padDir[p.pad-1] |= pinIdx[p.pin];
    } else{
        *padDir[p.pad-1] &= ~pinIdx[p.pin];
    }
}

bool MSP_Pads::read(padpin p){
    return *padIn[p.pad-1] & pinIdx[p.pin];
}

void MSP_Pads::write(padpin p, bool val){
    if(val){
        *padOut[p.pad-1] |= pinIdx[p.pin];
    } else{
        *padOut[p.pad-1] &= ~pinIdx[p.pin];
    }
}
